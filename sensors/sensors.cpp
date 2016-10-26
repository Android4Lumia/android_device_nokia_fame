/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "Sensors"

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>
#include <linux/input.h>
#include <utils/Atomic.h>
#include <utils/Log.h>
#include <CalibrationManager.h>

#include "sensors.h"
#include "AccelSensor.h"
#include "LightSensor.h"
#include "ProximitySensor.h"
#include "CompassSensor.h"
#include "GyroSensor.h"

#include "NativeSensorManager.h"
#include "sensors_extension.h"
/*****************************************************************************/

/**
 * structure representing a command to be dispatched to the poll thread
 */
struct sensor_command {
	enum cmd_type {
		CMD_ENABLE,
		CMD_SET_PERIOD,
	};
	cmd_type type;
	size_t sz;
	int sensor;
	union {
		bool enabled;
		uint64_t period;
	};
};

static int open_sensors(const struct hw_module_t* module, const char* id,
						struct hw_device_t** device);

static int sensors__get_sensors_list(struct sensors_module_t*,
								 struct sensor_t const** list)
{
	NativeSensorManager& sm(NativeSensorManager::getInstance());

	return sm.getSensorList(list);
}

static struct hw_module_methods_t sensors_module_methods = {
		open: open_sensors
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
		common: {
				tag: HARDWARE_MODULE_TAG,
				version_major: 1,
				version_minor: 0,
				id: SENSORS_HARDWARE_MODULE_ID,
				name: "Quic Sensor module",
				author: "Quic",
				methods: &sensors_module_methods,
				dso: NULL,
				reserved: {0},
		},
		get_sensors_list: sensors__get_sensors_list,
		set_operation_mode: NULL,
};

struct sensors_poll_context_t {
	struct sensors_poll_device_1_ext_t device;// must be first

		sensors_poll_context_t();
		~sensors_poll_context_t();
	int activate(int handle, int enabled);
	int setDelay(int handle, int64_t ns);
	int pollEvents(sensors_event_t* data, int count);
	int handleCommandPipe();
	int calibrate(int handle, cal_cmd_t *para);

private:
	struct pollfd mPollFds[MAX_SENSORS+1];

	/** index of the command read fd in mPollFds */
	int mCmdFdReadIdx;
	/** command write fd */
	int mCmdFdWrite;
	SensorBase* mSensors[MAX_SENSORS];
};

/*****************************************************************************/

sensors_poll_context_t::sensors_poll_context_t()
{
	int numSensors;
	int i;
	const struct sensor_t *slist;
	const struct SensorContext *context;
	NativeSensorManager& sm(NativeSensorManager::getInstance());

	numSensors = sm.getSensorList(&slist);
	if (numSensors > MAX_SENSORS) {
		ALOGW("Native sensor manager returned too many sensors (%d), truncating list %d",
			numSensors, MAX_SENSORS);
		numSensors = MAX_SENSORS;
	}

	/* use the dynamic sensor list */
	for (i = 0; i < numSensors; i++) {
		context = sm.getInfoByHandle(slist[i].handle);

		mPollFds[i].fd = (context == NULL) ? -1 : context->data_fd;
		mPollFds[i].events = POLLIN;
		mPollFds[i].revents = 0;
	}

	mCmdFdReadIdx = numSensors;
	ALOGD("Command pipe fd idx: %d", mCmdFdReadIdx);

	int cmdFds[2];
	int result = pipe(cmdFds);
	ALOGE_IF(result<0, "error creating cmd pipe (%s)", strerror(errno));
	fcntl(cmdFds[0], F_SETFL, O_NONBLOCK);
	fcntl(cmdFds[1], F_SETFL, O_NONBLOCK);
	mCmdFdWrite = cmdFds[1];

	mPollFds[mCmdFdReadIdx].fd = cmdFds[0];
	mPollFds[mCmdFdReadIdx].events = POLLIN;
	mPollFds[mCmdFdReadIdx].revents = 0;
}

sensors_poll_context_t::~sensors_poll_context_t() {
	NativeSensorManager& sm(NativeSensorManager::getInstance());
	close(mPollFds[mCmdFdReadIdx].fd);
	close(mCmdFdWrite);
}

int sensors_poll_context_t::activate(int handle, int enabled) {

	struct sensor_command cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	cmd.type = sensor_command::CMD_ENABLE;
	cmd.sz = sizeof(cmd);
	cmd.sensor = handle;
	cmd.enabled = enabled;

	ssize_t rc = write(mCmdFdWrite, &cmd, sizeof(cmd));

	if (rc < 0) {
		ALOGE("error writing command to pipe (%s)", strerror(errno));
		return -errno;
	}

	if (rc != sizeof(cmd)) {
		ALOGE("error writing command, invalid len (%zd, expect=%zd)",
			rc, sizeof(cmd));
		return -EIO;
	}

	return 0;
}

int sensors_poll_context_t::setDelay(int handle, int64_t ns) {

	struct sensor_command cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	cmd.type = sensor_command::CMD_SET_PERIOD;
	cmd.sz = sizeof(cmd);
	cmd.sensor = handle;
	cmd.period = ns;

	ssize_t rc = write(mCmdFdWrite, &cmd, sizeof(cmd));

	if (rc < 0) {
		ALOGE("error writing command to pipe (%s)", strerror(errno));
		return -errno;
	}

	if (rc != sizeof(cmd)) {
		ALOGE("error writing command, invalid len (%zd, expect=%zd)",
			rc, sizeof(cmd));
		return -EIO;
	}

	return 0;
}

int sensors_poll_context_t::pollEvents(sensors_event_t* data, int count)
{
	int nbEvents = 0;
	int n = 0;
	NativeSensorManager& sm(NativeSensorManager::getInstance());
	const sensor_t *slist;
	int number = sm.getSensorList(&slist);

	do {
		// see if we have some leftover from the last poll()
		for (int i = 0 ; count && i < number ; i++) {
			if ((mPollFds[i].revents & POLLIN) || (sm.hasPendingEvents(slist[i].handle))) {
				int nb = sm.readEvents(slist[i].handle, data, count);
				if (nb < 0) {
					ALOGE("readEvents failed.(%d)", errno);
					return nb;
				}
				if (nb <= count) {
					// no more data for this sensor
					mPollFds[i].revents = 0;
				}
				count -= nb;
				nbEvents += nb;
				data += nb;
			}
		}

		if (count) {
			// we still have some room, so try to see if we can get
			// some events immediately or just wait if we don't have
			// anything to return
			do {
				n = poll(mPollFds, number + 1, nbEvents ? 0 : -1);
			} while (n < 0 && errno == EINTR);
			if (n<0) {
				ALOGE("poll() failed (%s)", strerror(errno));
				return -errno;
			}
			if (mPollFds[mCmdFdReadIdx].revents & POLLIN) {
				int rc = handleCommandPipe();
				if (rc) {
					return rc;
				}
				mPollFds[mCmdFdReadIdx].revents = 0;
			}
		}
		// if we have events and space, go read them
	} while (n && count);

	return nbEvents;
}

static inline double ns2hz(int64_t period_ns) {
	if (period_ns == 0) {
		return 0;
	}
	return 1.0 / (period_ns * 1e-9);
}

int sensors_poll_context_t::handleCommandPipe() {
	struct sensor_command cmd;
	ssize_t rc = read(mPollFds[mCmdFdReadIdx].fd, &cmd, sizeof(cmd));
	if (rc < 0) {
		ALOGE("error reading command from pipe (%s)",
			strerror(errno));
		return -errno;
	}

	if (rc != sizeof(cmd)) {
		ALOGE("error reading command from pipe, invalid len read: %zd/expect %zd)",
			rc, sizeof(cmd));
		return -EIO;
	}

	// sanity check
	if (cmd.sz != sizeof(cmd)) {
		ALOGE("invalid command read from pipe, unxpected sz: %zu/expect %zu",
			cmd.sz, sizeof(cmd));
		return -EIO;
	}

	NativeSensorManager& sm(NativeSensorManager::getInstance());
	SensorContext *ctx = sm.getInfoByHandle(cmd.sensor);
	if (!ctx) {
		ALOGW("Invalid sensor handle in command pipe: %d", cmd.sensor);
		return -EIO;
	}

	const char * name = ctx->sensor->name;
	switch (cmd.type) {
		case sensor_command::CMD_ENABLE:
			ALOGI("%s: activate: %d", name, cmd.enabled);
			rc = sm.activate(cmd.sensor, cmd.enabled);
			break;
		case sensor_command::CMD_SET_PERIOD:
			ALOGI("%s: setDelay: %.2f Hz", name, ns2hz(cmd.period));
			rc = sm.setDelay(cmd.sensor, cmd.period);
			break;
		default:
			ALOGE("%s: invalid command id received: %d", name, cmd.type);
			return -EIO;
	}

	ALOGE_IF(rc, "%s: error (%d) processing command", name, rc);
	return rc;
}

int sensors_poll_context_t::calibrate(int handle, struct cal_cmd_t *para)
{

	int err = -1;
	NativeSensorManager& sm(NativeSensorManager::getInstance());

	err = sm.calibrate(handle, para);

	return err;
}

/*****************************************************************************/

static int poll__close(struct hw_device_t *dev)
{
	sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	if (ctx) {
		delete ctx;
	}
	return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev,
		int handle, int enabled) {
	sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	return ctx->activate(handle, enabled);
}

static int poll__setDelay(struct sensors_poll_device_t *dev,
		int handle, int64_t ns) {
	sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	return ctx->setDelay(handle, ns);
}

static int poll__poll(struct sensors_poll_device_t *dev,
		sensors_event_t* data, int count) {
	sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	return ctx->pollEvents(data, count);
}

static int poll_calibrate(struct sensors_poll_device_1_ext_t *dev,
		int handle, struct cal_cmd_t *para)
{
	sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	return ctx->calibrate(handle, para);
}
/*****************************************************************************/

/** Open a new instance of a sensor device using name */
static int open_sensors(const struct hw_module_t* module, const char*,
						struct hw_device_t** device)
{
		int status = -EINVAL;
		sensors_poll_context_t *dev = new sensors_poll_context_t();

		memset(&dev->device, 0, sizeof(sensors_poll_device_1_ext_t));

		dev->device.common.tag = HARDWARE_DEVICE_TAG;
		dev->device.common.version  = 0;
		dev->device.common.module   = const_cast<hw_module_t*>(module);
		dev->device.common.close	= poll__close;
		dev->device.activate		= poll__activate;
		dev->device.setDelay		= poll__setDelay;
		dev->device.poll			= poll__poll;
		dev->device.calibrate		= poll_calibrate;

		*device = &dev->device.common;
		status = 0;

		return status;
}


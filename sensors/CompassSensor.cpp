/*--------------------------------------------------------------------------
Copyright (c) 2014, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of The Linux Foundation nor
      the names of its contributors may be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------*/

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>

#include "CompassSensor.h"
#include "sensors.h"

#define FETCH_FULL_EVENT_BEFORE_RETURN	1
#define IGNORE_EVENT_TIME				10000000

#define EVENT_TYPE_MAG_X		ABS_X
#define EVENT_TYPE_MAG_Y		ABS_Y
#define EVENT_TYPE_MAG_Z		ABS_Z
#define EVENT_TYPE_MAG_STATUS	ABS_MISC

#define EVENT_TYPE_YAW			ABS_X
#define EVENT_TYPE_PITCH		ABS_Y
#define EVENT_TYPE_ROLL			ABS_Z

// conversion of magnetic data to uT units
#define CONVERT_MAG				(1.0f/16.0f)

/*****************************************************************************/
CompassSensor::CompassSensor(struct SensorContext *context)
	: SensorBase(NULL, NULL, context),
	  mEnabled(0),
	  mInputReader(6),
	  mHasPendingEvent(false),
	  mEnabledTime(0),
	  res(CONVERT_MAG)
{
	int handle = -1;

	res = context->sensor->resolution;

	memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
	mPendingEvent.version = sizeof(sensors_event_t);
	mPendingEvent.sensor = context->sensor->handle;
	mPendingEvent.type = SENSOR_TYPE_MAGNETIC_FIELD;
	mPendingEvent.magnetic.status = SENSOR_STATUS_UNRELIABLE;

	data_fd = context->data_fd;
	strlcpy(input_sysfs_path, context->enable_path, sizeof(input_sysfs_path));
	input_sysfs_path_len = strlen(input_sysfs_path);

	enable(0, 1);
}

CompassSensor::~CompassSensor() {
	if (mEnabled) {
		enable(0, 0);
	}
}

int CompassSensor::enable(int32_t, int en) {
	int flags = en ? 1 : 0;
	compass_algo_args arg;
	arg.common.enable = flags;

	if (flags != mEnabled) {
		int fd;

		if ((algo != NULL) && (algo->methods->config != NULL)) {
			if (algo->methods->config(CMD_ENABLE, (sensor_algo_args*)&arg)) {
				ALOGW("Calling enable config failed for compass");
			}
		}

		strlcpy(&input_sysfs_path[input_sysfs_path_len],
				SYSFS_ENABLE, SYSFS_MAXLEN);
		fd = open(input_sysfs_path, O_RDWR);
		if (fd >= 0) {
			char buf[2];
			int err;
			buf[1] = 0;
			if (flags) {
				buf[0] = '1';
				mEnabledTime = getTimestamp() + IGNORE_EVENT_TIME;
			} else {
				buf[0] = '0';
			}
			err = write(fd, buf, sizeof(buf));
			close(fd);
			mEnabled = flags;
			return 0;
		}
		ALOGE("CompassSensor: failed to open %s", input_sysfs_path);
		return -1;
	}
	return 0;
}

bool CompassSensor::hasPendingEvents() const {
	return mHasPendingEvent;
}

int CompassSensor::setDelay(int32_t, int64_t delay_ns)
{
	int fd;
	int delay_ms = delay_ns / 1000000;
	compass_algo_args arg;
	arg.common.delay_ms = delay_ms;

	if ((algo != NULL) && (algo->methods->config != NULL)) {
		if (algo->methods->config(CMD_DELAY, (sensor_algo_args*)&arg)) {
			ALOGW("Calling delay config failed for compass");
		}
	}

	strlcpy(&input_sysfs_path[input_sysfs_path_len],
			SYSFS_POLL_DELAY, SYSFS_MAXLEN);
	fd = open(input_sysfs_path, O_RDWR);
	if (fd >= 0) {
		char buf[80];
		sprintf(buf, "%d", delay_ms);
		write(fd, buf, strlen(buf)+1);
		close(fd);
		return 0;
	}
	return -1;
}

int CompassSensor::readEvents(sensors_event_t* data, int count)
{
	if (count < 1)
		return -EINVAL;

	if (mHasPendingEvent) {
		mHasPendingEvent = false;
		mPendingEvent.timestamp = getTimestamp();
		*data = mPendingEvent;
		return mEnabled ? 1 : 0;
	}

	ssize_t n = mInputReader.fill(data_fd);
	if (n < 0)
		return n;

	int numEventReceived = 0;
	input_event const* event;
	sensors_event_t raw, result;

#if FETCH_FULL_EVENT_BEFORE_RETURN
again:
#endif
	while (count && mInputReader.readEvent(&event)) {
		int type = event->type;
		if (type == EV_ABS) {
			float value = event->value;
			if (event->code == EVENT_TYPE_MAG_X) {
				mPendingEvent.magnetic.x = value * res;
			} else if (event->code == EVENT_TYPE_MAG_Y) {
				mPendingEvent.magnetic.y = value * res;
			} else if (event->code == EVENT_TYPE_MAG_Z) {
				mPendingEvent.magnetic.z = value * res;
			}
		} else if (type == EV_SYN) {
			if (event->code ==  SYN_TIME_SEC) {
				mUseAbsTimeStamp = true;
				report_time = event->value*1000000000LL;
			} else if (event->code ==  SYN_TIME_NSEC) {
				mUseAbsTimeStamp = true;
				mPendingEvent.timestamp = report_time+event->value;
			} else if (mEnabled) {
				if (!mUseAbsTimeStamp) {
					ALOGE("CompassSensor: timestamp not received");
				} else if (mPendingEvent.timestamp >= mEnabledTime) {
					raw = mPendingEvent;

					if (algo != NULL) {
						if (algo->methods->convert(&raw, &result, NULL)) {
							ALOGW("Calibration in progress...");
							result = raw;
							result.magnetic.status = SENSOR_STATUS_UNRELIABLE;
						}
					} else {
						result = raw;
					}

					*data = result;
					data->version = sizeof(sensors_event_t);
					data->sensor = mPendingEvent.sensor;
					data->type = SENSOR_TYPE_MAGNETIC_FIELD;
					data->timestamp = mPendingEvent.timestamp;

					/* The raw data is stored inside sensors_event_t.data after
					 * sensors_event_t.magnetic. Notice that the raw data is
					 * required to composite the virtual sensor uncalibrated
					 * magnetic field sensor.
					 *
					 * data[0~2]: calibrated magnetic field data.
					 * data[3]: magnetic field data accuracy.
					 * data[4~6]: uncalibrated magnetic field data.
					 */
					data->data[4] = mPendingEvent.data[0];
					data->data[5] = mPendingEvent.data[1];
					data->data[6] = mPendingEvent.data[2];

					data++;
					numEventReceived++;
				}
				count--;
			}
		} else {
			ALOGE("CompassSensor: unknown event (type=%d, code=%d)",
					type, event->code);
		}
		mInputReader.next();
	}

#if FETCH_FULL_EVENT_BEFORE_RETURN
	/* if we didn't read a complete event, see if we can fill and
	   try again instead of returning with nothing and redoing poll. */
	if (numEventReceived == 0 && mEnabled == 1) {
		n = mInputReader.fill(data_fd);
		if (n)
			goto again;
	}
#endif

	return numEventReceived;
}


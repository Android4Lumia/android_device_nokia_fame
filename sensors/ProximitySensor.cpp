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

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#include "ProximitySensor.h"
#include "sensors.h"

#define EVENT_TYPE_PROXIMITY		ABS_DISTANCE

#define PROXIMITY_THRESHOLD			5.0f
/*****************************************************************************/

enum input_device_name {
    GENERIC_PSENSOR = 0,
    LEGACY_PSENSOR,
    CM36283_PS,
    SUPPORTED_PSENSOR_COUNT,
};

static const char *data_device_name[SUPPORTED_PSENSOR_COUNT] = {
   [GENERIC_PSENSOR] = "proximity",
    [LEGACY_PSENSOR] = "proximity",
        [CM36283_PS] = "cm36283-ps",
};

static const char *input_sysfs_path_list[SUPPORTED_PSENSOR_COUNT] = {
   /* This is not used by generic HAL. Just for back compatibility */
   [GENERIC_PSENSOR] = "/sys/class/input/%s/device/",
    [LEGACY_PSENSOR] = "/sys/class/input/%s/device/",
        [CM36283_PS] = "/sys/class/input/%s/device/",
};

static const char *input_sysfs_enable_list[SUPPORTED_PSENSOR_COUNT] = {
   [GENERIC_PSENSOR] = "enable",
    [LEGACY_PSENSOR] = "enable",
        [CM36283_PS] = "enable",
};


ProximitySensor::ProximitySensor()
    : SensorBase(NULL, NULL),
      mEnabled(0),
      mInputReader(4),
      mHasPendingEvent(false),
      sensor_index(-1),
      mThreshold_h(0),
      mThreshold_l(0),
      mBias(0)
{
    int i;
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = SENSORS_PROXIMITY_HANDLE;
    mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

    for(i = 0; i < SUPPORTED_PSENSOR_COUNT; i++) {
        data_name = data_device_name[i];

        // data_fd is not initialized if data_name passed
        // to SensorBase is NULL.
        data_fd = openInput(data_name);
        if (data_fd > 0) {
            sensor_index = i;
            break;
        }
    }

    if (data_fd > 0) {
            snprintf(input_sysfs_path, sizeof(input_sysfs_path),
                            input_sysfs_path_list[i], input_name);
        input_sysfs_path_len = strlen(input_sysfs_path);
        enable(0, 1);
    }

    ALOGI("The proximity sensor path is %s",input_sysfs_path);
}

ProximitySensor::ProximitySensor(struct SensorContext *context)
	: SensorBase(NULL, NULL),
	  mEnabled(0),
	  mInputReader(4),
	  mHasPendingEvent(false),
	  sensor_index(GENERIC_PSENSOR),
    mThreshold_h(0),
    mThreshold_l(0),
    mBias(0)
{
	mPendingEvent.version = sizeof(sensors_event_t);
	mPendingEvent.sensor = context->sensor->handle;
	mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
	memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

	data_fd = context->data_fd;
	strlcpy(input_sysfs_path, context->enable_path, sizeof(input_sysfs_path));
	input_sysfs_path_len = strlen(input_sysfs_path);
}

ProximitySensor::ProximitySensor(char *name)
	: SensorBase(NULL, data_device_name[GENERIC_PSENSOR]),
	  mEnabled(0),
	  mInputReader(4),
	  mHasPendingEvent(false),
	  sensor_index(GENERIC_PSENSOR),
    mThreshold_h(0),
    mThreshold_l(0),
    mBias(0)
{
	mPendingEvent.version = sizeof(sensors_event_t);
	mPendingEvent.sensor = SENSORS_PROXIMITY_HANDLE;
	mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
	memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

	if (data_fd) {
		strlcpy(input_sysfs_path, SYSFS_CLASS, sizeof(input_sysfs_path));
		strlcat(input_sysfs_path, name, sizeof(input_sysfs_path));
		strlcat(input_sysfs_path, "/", sizeof(input_sysfs_path));
		input_sysfs_path_len = strlen(input_sysfs_path);
		ALOGI("The proximity sensor path is %s",input_sysfs_path);
		enable(0, 1);
	}
}
ProximitySensor::~ProximitySensor() {
    if (mEnabled) {
        enable(0, 0);
    }
}

int ProximitySensor::enable(int32_t, int en) {
    int flags = en ? 1 : 0;
    char propBuf[PROPERTY_VALUE_MAX];
    property_get("sensors.proxymity.loopback", propBuf, "0");
    if (strcmp(propBuf, "1") == 0) {
        mEnabled = flags;
        ALOGE("sensors.proxymity.loopback is set");
        return 0;
    }

    if (flags != mEnabled) {
        int fd;
        if (sensor_index >= 0) {
            strlcpy(&input_sysfs_path[input_sysfs_path_len], input_sysfs_enable_list[sensor_index],
                            sizeof(input_sysfs_path) - input_sysfs_path_len);
        } else {
            ALOGE("invalid sensor index:%d\n", sensor_index);
            return -1;
        }

        mEnabled = flags;
        /**
         * this is an on-change sensor. it might not get a reading for a while
         * and Android requires a reading as soon as it is turned on.
         * re-send the last reading.
         */
        mHasPendingEvent = flags;
        fd = open(input_sysfs_path, O_RDWR);
        if (fd >= 0) {
            char buf[2];
            buf[1] = 0;
            if (flags) {
                buf[0] = '1';
            } else {
                buf[0] = '0';
            }
            write(fd, buf, sizeof(buf));
            close(fd);
        } else {
            ALOGE("open %s failed.(%s)\n", input_sysfs_path, strerror(errno));
            return -1;
        }

        if (mEnabled) {
            struct input_absinfo absinfo;
            int rc = ioctl(data_fd, EVIOCGABS(EVENT_TYPE_PROXIMITY), &absinfo);
            if (rc < 0) {
                ALOGE("ProximitySensor: EVIOCGABS error: %d", errno);
                return -errno;
            }

            mPendingEvent.distance = indexToValue(absinfo.value);
            return 0;
        }
    }
    return 0;
}

bool ProximitySensor::hasPendingEvents() const {
    return mHasPendingEvent;
}

int ProximitySensor::readEvents(sensors_event_t* data, int count)
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

    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            if (event->code == EVENT_TYPE_PROXIMITY) {
                if (event->value != -1) {
                    // FIXME: not sure why we're getting -1 sometimes
                    mPendingEvent.distance = indexToValue(event->value);
                }
            }
        } else if (type == EV_SYN) {
            switch ( event->code ){
                case SYN_TIME_SEC:
                    {
                        mUseAbsTimeStamp = true;
                        report_time = event->value*1000000000LL;
                    }
                break;
                case SYN_TIME_NSEC:
                    {
                        mUseAbsTimeStamp = true;
                        mPendingEvent.timestamp = report_time+event->value;
                    }
                break;
                case SYN_REPORT:
                    {
                        if (mEnabled && mUseAbsTimeStamp) {
                            *data++ = mPendingEvent;
                            numEventReceived++;
                            count--;
                            mUseAbsTimeStamp = false;
                        } else {
                            ALOGE_IF(!mUseAbsTimeStamp, "ProximitySensor: timestamp not received");
                        }
                    }
                break;
            }
        } else {
            ALOGE("ProximitySensor: unknown event (type=%d, code=%d)",
                    type, event->code);
        }
        mInputReader.next();
    }

    return numEventReceived;
}

int ProximitySensor::setDelay(int32_t, int64_t ns)
{
    int fd;
    char propBuf[PROPERTY_VALUE_MAX];
    char buf[80];
    int len;

    property_get("sensors.light.loopback", propBuf, "0");
    if (strcmp(propBuf, "1") == 0) {
        ALOGE("sensors.light.loopback is set");
        return 0;
    }
    int delay_ms = ns / 1000000;
    strlcpy(&input_sysfs_path[input_sysfs_path_len],
                SYSFS_POLL_DELAY, SYSFS_MAXLEN);
    fd = open(input_sysfs_path, O_RDWR);
    if (fd < 0) {
        ALOGE("open %s failed.(%s)\n", input_sysfs_path, strerror(errno));
        return -1;
    }
    snprintf(buf, sizeof(buf), "%d", delay_ms);
    len = write(fd, buf, ssize_t(strlen(buf)+1));
    if (len < ssize_t(strlen(buf) + 1)) {
        ALOGE("write %s failed\n", buf);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

float ProximitySensor::indexToValue(size_t index) const
{
    return index * PROXIMITY_THRESHOLD;
}

int ProximitySensor::calibrate(int32_t, struct cal_cmd_t *para,
                struct cal_result_t *cal_result)
{
        int fd;
        char temp[3][LENGTH];
        char buf[3 * LENGTH];
        char *token, *strsaveptr, *endptr;
        int i, err;
        off_t offset;
        int para1 = 0;
        if (para == NULL || cal_result == NULL) {
                ALOGE("Null pointer calibrate parameters\n");
                return -1;
        }
        para1 = CMD_CAL(para->axis, para->apply_now);
        strlcpy(&input_sysfs_path[input_sysfs_path_len],
                        SYSFS_CALIBRATE, SYSFS_MAXLEN);
        fd = open(input_sysfs_path, O_RDWR);
        if (fd >= 0) {
                snprintf(buf, sizeof(buf), "%d", para1);
                err = write(fd, buf, strlen(buf)+1);
                if(err < 0) {
                        ALOGE("write error\n");
                        close(fd);
                        return err;
                }
        } else {
                return -1;
        }
        if (fd >= 0) {
                offset = lseek(fd, 0, SEEK_SET);
                char *p = buf;
                memset(buf, 0, sizeof(buf));
                err = read(fd, buf, sizeof(buf)-1);
                if(err < 0) {
                        ALOGE("proximity read error\n");
                        close(fd);
                        return err;
                }
                for(i = 0; i < (int)(sizeof(temp) / LENGTH); i++, p = NULL) {
                        token = strtok_r(p, ",", &strsaveptr);
                        if(token == NULL)
                                break;
                        if(strlen(token) > LENGTH - 1) {
                                ALOGE("token is too long\n");
                                close(fd);
                                return -1;
                        }
                        strlcpy(temp[i], token, sizeof(temp[i]));
                }
                close(fd);
                if (para->axis == 0) {
                        mThreshold_h = strtol(temp[0], &endptr, 0);
                        if (mThreshold_h == LONG_MAX || mThreshold_h == LONG_MIN) {
                                ALOGE("mThreshold_h error value\n");
                                return -1;
                        }
                        if (endptr == temp[0]) {
                                ALOGE("No digits were found\n");
                                return -1;
                        }
                } else if (para->axis == 1) {
                        mThreshold_l = strtol(temp[1], &endptr, 0);
                        if (mThreshold_l == LONG_MAX || mThreshold_l == LONG_MIN) {
                                ALOGE("mThreshold_l error value\n");
                                return -1;
                        }
                        if (endptr == temp[1]) {
                                ALOGE("No digits were found\n");
                                return -1;
                        }
                } else if (para->axis == 2) {
                        mBias = strtol(temp[2], &endptr, 0);
                        if (mBias == LONG_MAX || mBias == LONG_MIN) {
                                ALOGE("mBias error value\n");
                                return -1;
                        }
                        if (endptr == temp[2]) {
                                ALOGE("No digits were found\n");
                                return -1;
                        }
                }
                cal_result->threshold_h = mThreshold_h;
                cal_result->threshold_l = mThreshold_l;
                cal_result->bias = mBias;
                return 0;
        } else {
                ALOGE("open %s error\n", input_sysfs_path);
                return -1;
        }
        return 0;
}

int ProximitySensor::initCalibrate(int32_t, struct cal_result_t *cal_result)
{
        int fd , i, err;
        char buf[33];
        int arry[] = {CMD_W_THRESHOLD_H, CMD_W_THRESHOLD_L, CMD_W_BIAS};
        if (cal_result == NULL) {
                ALOGE("Null pointer initcalibrate parameter\n");
                return -1;
        }
        strlcpy(&input_sysfs_path[input_sysfs_path_len],
                        SYSFS_CALIBRATE, SYSFS_MAXLEN);
        fd = open(input_sysfs_path, O_RDWR);
        if (fd >= 0) {
                int temp, para1 = 0;
                for(i = 0; i < (int)(sizeof(arry) / sizeof(int)); ++i) {
                        para1 = SET_CMD_H(cal_result->offset[i], arry[i]);
                        snprintf(buf, sizeof(buf), "%d",
                                        para1);
                        err = write(fd, buf, strlen(buf)+1);
                        if(err < 0) {
                                ALOGE("write error\n");
                                close(fd);
                                return err;
                        }

                        memset(buf, 0, sizeof(buf));
                        para1 = SET_CMD_L(cal_result->offset[i], arry[i]);
                        snprintf(buf, sizeof(buf), "%d",
                                        para1);
                        write(fd, buf, strlen(buf)+1);
                        if(err < 0) {
                                ALOGE("write error\n");
                                close(fd);
                                return err;
                        }

                }
                memset(buf, 0, sizeof(buf));
                snprintf(buf, sizeof(buf), "%d", CMD_COMPLETE);
                write(fd, buf, strlen(buf)+1);
                close(fd);
                return 0;
        }
        ALOGE("open %s error\n", input_sysfs_path);
        return -1;
}

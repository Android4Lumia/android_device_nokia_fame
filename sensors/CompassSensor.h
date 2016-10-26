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

#ifndef ANDROID_COMPASS_SENSOR_H
#define ANDROID_COMPASS_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "SensorBase.h"
#include "InputEventReader.h"
#include "NativeSensorManager.h"

/*****************************************************************************/

struct input_event;

class CompassSensor : public SensorBase {
	int mEnabled;
	InputEventCircularReader mInputReader;
	sensors_event_t mPendingEvent;
	bool mHasPendingEvent;
	char input_sysfs_path[PATH_MAX];
	int input_sysfs_path_len;
	int64_t mEnabledTime;
	float res;

public:
	CompassSensor(struct SensorContext *context);
	virtual ~CompassSensor();
	virtual int readEvents(sensors_event_t* data, int count);
	virtual bool hasPendingEvents() const;
	virtual int setDelay(int32_t handle, int64_t ns);
	virtual int enable(int32_t handle, int enabled);
};

/*****************************************************************************/

#endif  // ANDROID_COMPASS_SENSOR_H

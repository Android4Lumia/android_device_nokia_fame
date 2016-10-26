/*--------------------------------------------------------------------------
Copyright (c) 2014, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------*/
#ifndef ANDROID_VIRTUAL_SENSOR_H
#define ANDROID_VIRTUAL_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "SensorBase.h"
#include "InputEventReader.h"
#include "NativeSensorManager.h"

/*****************************************************************************/

#define MAX_EVENTS 250

struct input_event;

class VirtualSensor : public SensorBase {
	int mEnabled;
	sensors_vec_t dm;
	sensors_vec_t da;
	bool mHasPendingEvent;
	int64_t mEnabledTime;
	const SensorContext *context;
	sensors_event_t mBuffer[MAX_EVENTS];
	sensors_event_t* mRead;
	sensors_event_t* mWrite;
	sensors_event_t* mBufferEnd;
	ssize_t mFreeSpace;
public:
	VirtualSensor(const struct SensorContext *i);
	virtual ~VirtualSensor();
	virtual int readEvents(sensors_event_t* data, int count);
	virtual bool hasPendingEvents() const;
	virtual int enable(int32_t handle, int enabled);
	virtual int injectEvents(sensors_event_t* data, int count);
};

/*****************************************************************************/

#endif  // ANDROID_VIRTUAL_SENSOR_H

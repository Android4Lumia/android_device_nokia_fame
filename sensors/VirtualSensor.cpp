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

#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <float.h>
#include <sys/select.h>
#include <cutils/log.h>

#include "VirtualSensor.h"
#include "sensors.h"

/*****************************************************************************/

VirtualSensor::VirtualSensor(const struct SensorContext *ctx)
	: SensorBase(NULL, NULL, ctx),
	  mEnabled(0),
	  mHasPendingEvent(false),
	  mEnabledTime(0),
	  context(ctx),
	  mRead(mBuffer),
	  mWrite(mBuffer),
	  mBufferEnd(mBuffer + MAX_EVENTS),
	  mFreeSpace(MAX_EVENTS)

{
	enable(0, 1);
}

VirtualSensor::~VirtualSensor() {
	if (mEnabled) {
		enable(0, 0);
	}
}

int VirtualSensor::enable(int32_t, int en) {
	mEnabled = en? 1 : 0;
	return 0;
}

bool VirtualSensor::hasPendingEvents() const {
	return mBufferEnd - mBuffer - mFreeSpace;
}

int VirtualSensor::readEvents(sensors_event_t* data, int count)
{
	int number = 0;

	if (count < 1)
		return -EINVAL;

	while (count && (mBufferEnd - mBuffer - mFreeSpace)) {
		*data++ = *mRead++;
		if (mRead >= mBufferEnd)
			mRead = mBuffer;
		number++;
		mFreeSpace++;
		count--;
	}

	return number;
}

int VirtualSensor::injectEvents(sensors_event_t* data, int count)
{
	int i;
	sensors_event_t event;

	if (algo == NULL)
		return 0;

	for (i = 0; i < count; i++) {
		event = data[i];

		if (mFreeSpace) {
			sensors_event_t out;
			if (algo->methods->convert(&event, &out, NULL))
				continue;

			out.version = sizeof(sensors_event_t);
			out.sensor = context->sensor->handle;
			out.type = context->sensor->type;
			out.timestamp = event.timestamp;

			*mWrite++ = out;
			mFreeSpace--;
			if (mWrite >= mBufferEnd) {
				mWrite = mBuffer;
			}

		} else {
			ALOGW("Circular buffer is full\n");
		}
	}

	return 0;
}


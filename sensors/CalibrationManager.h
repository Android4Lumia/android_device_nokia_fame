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

#ifndef _SENSOR_CALIBRATION_MANAGER_H
#define _SENSOR_CALIBRATION_MANAGER_H

#include <utils/Singleton.h>
#include "CalibrationModule.h"

using namespace android;

#define MAX_CAL_LIBS	32
#define MAX_CAL_CFG_LEN	1024

/* Calibration library config files */
#define CAL_LIB_CFG_PATH	"/system/vendor/etc/calmodule.cfg"
#define DEFAULT_CAL_LIB		"libcalmodule_common.so"
#if defined(__LP64__)
#define CAL_LIB_PATH	"/system/vendor/lib64/"
#else
#define CAL_LIB_PATH	"/system/vendor/lib/"
#endif

class CalibrationManager : public Singleton<CalibrationManager> {
	public:
		/* Get the whole algo list provided by the calibration library */
		const sensor_cal_algo_t** getCalAlgoList();
		/* Retrive a compatible calibration algo for sensor specified by t */
		const sensor_cal_algo_t* getCalAlgo(const sensor_t *s);
		/* Dump the calibration manager status */
		void dump();
		~CalibrationManager();
	private:
		friend class Singleton<CalibrationManager>;
		/* Check if the algo provided by list is compatible */
		static int check_algo(const sensor_cal_algo_t *list);
		CalibrationManager();
		void loadCalLibs();
		/* Point to a whole list of all the algo provided by calibration library */
		const sensor_cal_algo_t **algo_list;
		/* Number of algo */
		uint32_t algo_count;
};

#endif

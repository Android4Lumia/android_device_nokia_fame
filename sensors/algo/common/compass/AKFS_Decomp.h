/******************************************************************************
 *
 * Copyright (C) 2012 Asahi Kasei Microdevices Corporation, Japan
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
 *
 ******************************************************************************/
#ifndef AKFS_INC_DECOMP_H
#define AKFS_INC_DECOMP_H

#include "AKFS_Device.h"

/***** Constant definition ****************************************************/
#if defined(AKM_DEVICE_AK8963)
#define AKM_SENSITIVITY			0.15f
#define AKM_ST_ERROR(st)		(((st)&0x19) != 0x11)
#define AKM_HDATA_CONVERTER(data, asa)			\
	(AKFLOAT)(((data)*(((asa)/256.0f) + 0.5f)))

#elif defined(AKM_DEVICE_AK8975)
#define AKM_SENSITIVITY			0.3f
#define AKM_ST_ERROR(st)		(((st)&0x09) != 0x01)
#define AKM_HDATA_CONVERTER(data, asa)			\
	(AKFLOAT)(((data)*(((asa)/256.0f) + 0.5f)))

#elif defined(AKM_DEVICE_AK09911)
#define AKM_SENSITIVITY			0.6f
#define AKM_ST_ERROR(st)		(((st)&0x09) != 0x01)
#define AKM_HDATA_CONVERTER(data, asa)			\
	(AKFLOAT)(((data)*(((asa)/128.0f) + 1.0f)))

#endif


/***** Type declaration *******************************************************/

/***** Prototype of function **************************************************/
AKLIB_C_API_START
int16 AKFS_Decomp(
	const	int16		mag[3],
	const	int16		status,
	const	uint8vec	*asa,
	const	int16		nhdata,
			AKFVEC		hdata[]
);
AKLIB_C_API_END

#endif

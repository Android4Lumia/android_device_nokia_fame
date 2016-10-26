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
#include "AKFS_Decomp.h"
#include "AKFS_Device.h"

/******************************************************************************/
/*! Convert from sensor local data unit to micro tesla, then buffer the data.
  @return #AKFS_SUCCESS on success. Otherwise the return value is #AKFS_ERROR.
  @param[in] mag
  @param[in] status
  @param[in] asa
  @param[in] nhdata
  @param[out] hdata
 */
int16 AKFS_Decomp(
	const	int16		mag[3],
	const	int16		status,
	const	uint8vec	*asa,
	const	int16		nhdata,
			AKFVEC		hdata[]
)
{
	/* put st1 and st2 value */
	if (AKM_ST_ERROR(status)) {
		return AKFS_ERROR;
	}

	/* magnetic */
	AKFS_BufShift(nhdata, 1, hdata);
	hdata[0].u.x = AKM_HDATA_CONVERTER(mag[0], asa->u.x) * AKM_SENSITIVITY;
	hdata[0].u.y = AKM_HDATA_CONVERTER(mag[1], asa->u.y) * AKM_SENSITIVITY;
	hdata[0].u.z = AKM_HDATA_CONVERTER(mag[2], asa->u.z) * AKM_SENSITIVITY;

	return AKFS_SUCCESS;
}


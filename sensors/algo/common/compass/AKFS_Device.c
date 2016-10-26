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
#include "AKFS_Device.h"

/******************************************************************************/
/*! Initialize #AKFVEC array.
  @return #AKFS_SUCCESS on success. Otherwise the return value is #AKFS_ERROR.
  @param[in] ndata
  @param[out] vdata
 */
int16 AKFS_InitBuffer(
	const	int16	ndata,		/*!< Size of vector buffer */
			AKFVEC	vdata[]		/*!< Vector buffer */
)
{
	int i;

	/* size check */
	if (ndata <= 0) {
		return AKFS_ERROR;
	}

	for (i = 0; i < ndata; i++) {
		vdata[i].u.x = AKFS_INIT_VALUE_F;
		vdata[i].u.y = AKFS_INIT_VALUE_F;
		vdata[i].u.z = AKFS_INIT_VALUE_F;
	}

	return AKFS_SUCCESS;
}

/******************************************************************************/
/*! Shift #AKFVEC array.
  @return #AKFS_SUCCESS on success. Otherwise the return value is #AKFS_ERROR.
  @param[in] len
  @param[in] shift
  @param[in/out] v
 */
int16 AKFS_BufShift(
	const	int16	len,	/*!< size of buffer */
	const	int16	shift,	/*!< shift size */
			AKFVEC	v[] /*!< buffer */
)
{
	int16 i;

	if ((shift < 1) || (len < shift)) {
		return AKFS_ERROR;
	}
	for (i = len-1; i >= shift; i--) {
		v[i] = v[i-shift];
	}
	return AKFS_SUCCESS;
}

/******************************************************************************/
/*! Rotate vector according to the specified layout pattern number.
  @return #AKFS_SUCCESS on success. Otherwise the return value is #AKFS_ERROR.
  @param[in] pat
  @param[in/out] vec
 */
int16 AKFS_Rotate(
	const	AKFS_PATNO	pat,
			AKFVEC		*vec
)
{
	AKFLOAT tmp;
	switch (pat) {
	/* Obverse */
	case PAT1:
		/* This is Android default */
		break;
	case PAT2:
		tmp = vec->u.x;
		vec->u.x = vec->u.y;
		vec->u.y = -tmp;
		break;
	case PAT3:
		vec->u.x = -(vec->u.x);
		vec->u.y = -(vec->u.y);
		break;
	case PAT4:
		tmp = vec->u.x;
		vec->u.x = -(vec->u.y);
		vec->u.y = tmp;
		break;
	/* Reverse */
	case PAT5:
		vec->u.x = -(vec->u.x);
		vec->u.z = -(vec->u.z);
		break;
	case PAT6:
		tmp = vec->u.x;
		vec->u.x = vec->u.y;
		vec->u.y = tmp;
		vec->u.z = -(vec->u.z);
		break;
	case PAT7:
		vec->u.y = -(vec->u.y);
		vec->u.z = -(vec->u.z);
		break;
	case PAT8:
		tmp = vec->u.x;
		vec->u.x = -(vec->u.y);
		vec->u.y = -tmp;
		vec->u.z = -(vec->u.z);
		break;
	default:
		return AKFS_ERROR;
	}

	return AKFS_SUCCESS;
}

/******************************************************************************/
/*! Rotate vector according to the specified layout matrix.
  @return #AKFS_SUCCESS on success. Otherwise the return value is #AKFS_ERROR.
  @param[in] pat
  @param[in/out] vec
 */
int16 AKFS_RotateMat(
	const   int16		layout[3][3],
			AKFVEC		*vec
)
{
	AKFVEC tmp;

	tmp.u.x = layout[0][0]*vec->u.x + layout[0][1]*vec->u.y + layout[0][2]*vec->u.z;
	tmp.u.y = layout[1][0]*vec->u.x + layout[1][1]*vec->u.y + layout[1][2]*vec->u.z;
	tmp.u.z = layout[2][0]*vec->u.x + layout[2][1]*vec->u.y + layout[2][2]*vec->u.z;

	*vec = tmp;

	return AKFS_SUCCESS;
}

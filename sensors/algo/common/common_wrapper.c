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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <CalibrationModule.h>

#define LOG_TAG "sensor_cal.common"
#include <utils/Log.h>

#include "compass/AKFS_Device.h"
#include "compass/AKFS_Decomp.h"
#include "compass/AKFS_AOC.h"
#include "compass/AKFS_Math.h"
#include "compass/AKFS_VNorm.h"

#define SENSOR_CAL_ALGO_VERSION 1
#define AKM_MAG_SENSE                   (1.0)
#define CSPEC_HNAVE_V   8
#define AKFS_GEOMAG_MAX 70

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct sensor_vec {
	union {
		struct {
			float data[4];
		};
		struct {
			float x;
			float y;
			float z;
		};
	};
};

struct sensor_cal_module_t SENSOR_CAL_MODULE_INFO;
static struct sensor_cal_algo_t algo_list[];

/*! A parameter structure. */
/* ix*_ : x-bit integer */
/* f**_ : floating value */
/* p**_ : pointer */
/* e**_ : enum */
/* *s*_ : struct */
/* *v*_ : vector (special type of struct) */
/* **a_ : array */
typedef struct _AKMPRMS{

	/* Variables for Decomp. */
	AKFVEC                  fva_hdata[AKFS_HDATA_SIZE];
	uint8vec                i8v_asa;

	/* Variables forAOC. */
	AKFS_AOC_VAR    s_aocv;

	/* Variables for Magnetometer buffer. */
	AKFVEC                  fva_hvbuf[AKFS_HDATA_SIZE];
	AKFVEC                  fv_ho;
	AKFVEC                  fv_hs;
	AKFS_PATNO              e_hpat;

	/* Variables for Accelerometer buffer. */
	AKFVEC                  fva_avbuf[AKFS_ADATA_SIZE];
	AKFVEC                  fv_ao;
	AKFVEC                  fv_as;

	/* Variables for Direction. */
	AKFLOAT                 f_azimuth;
	AKFLOAT                 f_pitch;
	AKFLOAT                 f_roll;

	/* Variables for vector output */
	AKFVEC                  fv_hvec;
	AKFVEC                  fv_avec;
	int16                   i16_hstatus;

} AKMPRMS;

static AKMPRMS g_prms;

static int convert_magnetic(sensors_event_t *raw, sensors_event_t *result,
		struct sensor_algo_args *args __attribute__((unused)))
{
	int16 akret;
	int16 aocret;
	AKFLOAT radius;
	AKMPRMS *prms = &g_prms;
	int i;

	/* Shift out old data from the buffer for better calibration */
	for (i = AKFS_HDATA_SIZE - 1; i >= 1; i--) {
		prms->fva_hdata[i] = prms->fva_hdata[i - 1];
	}

	prms->fva_hdata[0].u.x = raw->magnetic.x;
	prms->fva_hdata[0].u.y = raw->magnetic.y;
	prms->fva_hdata[0].u.z = raw->magnetic.z;

	/* Offset calculation is done in this function */
	/* hdata[in] : Android coordinate, sensitivity adjusted. */
	/* ho   [out]: Android coordinate, sensitivity adjusted. */
	aocret = AKFS_AOC(
		&prms->s_aocv,
		prms->fva_hdata,
		&prms->fv_ho
	);

	/* Subtract offset */
	/* hdata[in] : Android coordinate, sensitivity adjusted. */
	/* ho   [in] : Android coordinate, sensitivity adjusted. */
	/* hvbuf[out]: Android coordinate, sensitivity adjusted, */
	/*			   offset subtracted. */
	akret = AKFS_VbNorm(
		AKFS_HDATA_SIZE,
		prms->fva_hdata,
		1,
		&prms->fv_ho,
		&prms->fv_hs,
		AKM_MAG_SENSE,
		AKFS_HDATA_SIZE,
		prms->fva_hvbuf
	);
	if (akret == AKFS_ERROR) {
		ALOGE("error here!");
		return -1;
	}

	/* Averaging */
	/* hvbuf[in] : Android coordinate, sensitivity adjusted, */
	/*			   offset subtracted. */
	/* hvec [out]: Android coordinate, sensitivity adjusted, */
	/*			   offset subtracted, averaged. */
	akret = AKFS_VbAve(
		AKFS_HDATA_SIZE,
		prms->fva_hvbuf,
		CSPEC_HNAVE_V,
		&prms->fv_hvec
	);
	if (akret == AKFS_ERROR) {
		ALOGE("error here!");
		return -1;
	}

	/* Check the size of magnetic vector */
	radius = AKFS_SQRT(
			(prms->fv_hvec.u.x * prms->fv_hvec.u.x) +
			(prms->fv_hvec.u.y * prms->fv_hvec.u.y) +
			(prms->fv_hvec.u.z * prms->fv_hvec.u.z));

	if (radius > AKFS_GEOMAG_MAX) {
		prms->i16_hstatus = 0;
	} else {
		if (aocret == AKFS_SUCCESS) {
			prms->i16_hstatus = 3;
		}
	}

	result->magnetic.x = prms->fv_hvec.u.x;
	result->magnetic.y = prms->fv_hvec.u.y;
	result->magnetic.z = prms->fv_hvec.u.z;
	result->magnetic.status = prms->i16_hstatus;

	return 0;
}

static int convert_orientation(sensors_event_t *raw, sensors_event_t *result,
		struct sensor_algo_args *args __attribute__((unused)))
{
	float av;
	float pitch, roll, azimuth;
	const float rad2deg = 180 / M_PI;

	static struct sensor_vec mag, acc;

	if (raw->type == SENSOR_TYPE_MAGNETIC_FIELD) {
		mag.x = raw->magnetic.x;
		mag.y = raw->magnetic.y;
		mag.z = raw->magnetic.z;
	}

	if (raw->type == SENSOR_TYPE_ACCELEROMETER) {
		acc.x = raw->acceleration.x;
		acc.y = raw->acceleration.y;
		acc.z = raw->acceleration.z;
	}

	av = sqrtf(acc.x*acc.x + acc.y*acc.y + acc.z*acc.z);
	if (av >= DBL_EPSILON) {
		pitch = asinf(-acc.y / av);
		roll = asinf(acc.x / av);
		result->orientation.pitch = pitch * rad2deg;
		result->orientation.roll = roll * rad2deg;
		azimuth = atan2(-(mag.x) * cosf(roll) + mag.z * sinf(roll),
				mag.x*sinf(pitch)*sinf(roll) + mag.y*cosf(pitch) + mag.z*sinf(pitch)*cosf(roll));
		result->orientation.azimuth =  azimuth * rad2deg;
		result->orientation.status = 3;
	}

	if (raw->type != SENSOR_TYPE_MAGNETIC_FIELD)
		return -EAGAIN;

	return 0;

}

static int convert_rotation_vector(sensors_event_t *raw, sensors_event_t *result,
		struct sensor_algo_args *args __attribute__((unused)))
{
	float av;
	float pitch, roll, azimuth;
	int i;

	static struct sensor_vec mag, acc;

	if (raw->type == SENSOR_TYPE_MAGNETIC_FIELD) {
		mag.x = raw->magnetic.x;
		mag.y = raw->magnetic.y;
		mag.z = raw->magnetic.z;
	}

	if (raw->type == SENSOR_TYPE_ACCELEROMETER) {
		acc.x = raw->acceleration.x;
		acc.y = raw->acceleration.y;
		acc.z = raw->acceleration.z;
	}


	av = sqrtf(acc.x*acc.x + acc.y*acc.y + acc.z*acc.z);
	pitch = asinf(-acc.y / av);
	roll = asinf(acc.x / av);
	azimuth = atan2(-(mag.x) * cosf(roll) + mag.z * sinf(roll),
			mag.x*sinf(pitch)*sinf(roll) + mag.y*cosf(pitch) + mag.z*sinf(pitch)*cosf(roll));

	float halfAzi = azimuth / 2;
	float halfPitch = pitch / 2;
	float halfRoll = -roll / 2;

	float c1 = cosf(halfAzi);
	float s1 = sinf(halfAzi);
	float c2 = cosf(halfPitch);
	float s2 = sinf(halfPitch);
	float c3 = cosf(halfRoll);
	float s3 = sinf(halfRoll);

	result->data[0] = c1*c2*c3 - s1*s2*s3;
	result->data[1] = c1*s2*c3 - s1*c2*s3;
	result->data[2] = c1*c2*s3 + s1*s2*c3;
	result->data[3] = s1*c2*c3 + c1*s2*s3;

	if (halfAzi < M_PI / 2) {
		result->data[1] = -result->data[1];
		result->data[3] = -result->data[3];
	} else {
		result->data[2] = -result->data[2];
	}

	if (raw->type != SENSOR_TYPE_MAGNETIC_FIELD)
		return -1;

	return 0;
}

static int config_magnetic(int cmd, struct sensor_algo_args *args __attribute__((unused)))
{
	struct compass_algo_args *param = (struct compass_algo_args*)args;

	switch (cmd) {
		case CMD_ENABLE:
			ALOGD("Enable status changed to %d\n", param->common.enable);
			break;
		case CMD_DELAY:
			ALOGD("Polling rate changed to %d\n", param->common.delay_ms);
			break;
		case CMD_BATCH:
			break;
	}

	return 0;
}

/* The magnetic field raw data is supposed to store at the sensors_event_t:data[4~6]*/
static int convert_uncalibrated_magnetic(sensors_event_t *raw, sensors_event_t *result,
		struct sensor_algo_args *args __attribute__((unused)))
{
	if (raw->type == SENSOR_TYPE_MAGNETIC_FIELD) {
		result->uncalibrated_magnetic.x_uncalib = raw->data[4];
		result->uncalibrated_magnetic.y_uncalib = raw->data[5];
		result->uncalibrated_magnetic.z_uncalib = raw->data[6];

		result->uncalibrated_magnetic.x_bias = raw->data[4] - raw->data[0];
		result->uncalibrated_magnetic.y_bias = raw->data[5] - raw->data[1];
		result->uncalibrated_magnetic.z_bias = raw->data[6] - raw->data[2];

		return 0;
	}

	return -1;
}

static int cal_init(const struct sensor_cal_module_t *module __attribute__((unused)))
{
	AKMPRMS *prms = &g_prms;

	/* Clear all data. */
	memset(prms, 0, sizeof(AKMPRMS));

	/* Sensitivity */
	prms->fv_hs.u.x = AKM_MAG_SENSE;
	prms->fv_hs.u.y = AKM_MAG_SENSE;
	prms->fv_hs.u.z = AKM_MAG_SENSE;

	/* Initialize buffer */
	AKFS_InitBuffer(AKFS_HDATA_SIZE, prms->fva_hdata);
	AKFS_InitBuffer(AKFS_HDATA_SIZE, prms->fva_hvbuf);
	AKFS_InitBuffer(AKFS_ADATA_SIZE, prms->fva_avbuf);

	/* Initialize for AOC */
	AKFS_InitAOC(&prms->s_aocv);
	/* Initialize magnetic status */
	prms->i16_hstatus = 0;

	return 0;
}

static void cal_deinit()
{
	ALOGI("%s called\n", __func__);
}

static int cal_get_algo_list(const struct sensor_cal_algo_t **algo)
{
	*algo = algo_list;
	return 0;
}

static struct sensor_algo_methods_t compass_methods = {
	.convert = convert_magnetic,
	.config = config_magnetic,
};

static const char* compass_match_table[] = {
	COMPASS_NAME,
	NULL
};

static struct sensor_algo_methods_t orientation_methods = {
	.convert = convert_orientation,
	.config = NULL,
};

static const char* orientation_match_table[] = {
	ORIENTATION_NAME,
	NULL
};

static struct sensor_algo_methods_t rotation_vector_methods = {
	.convert = convert_rotation_vector,
	.config = NULL,
};

static const char* rotation_vector_match_table[] = {
	ROTATION_VECTOR_NAME,
	NULL
};

static struct sensor_algo_methods_t mag_uncalib_methods = {
	.convert = convert_uncalibrated_magnetic,
	.config = NULL,
};

static const char* mag_uncalib_match_table[] = {
	MAGNETIC_FIELD_UNCALIBRATED_NAME,
	NULL
};

static struct sensor_cal_algo_t algo_list[] = {
	{
		.tag = SENSOR_CAL_ALGO_TAG,
		.version = SENSOR_CAL_ALGO_VERSION,
		.type = SENSOR_TYPE_MAGNETIC_FIELD,
		.compatible = compass_match_table,
		.module = &SENSOR_CAL_MODULE_INFO,
		.methods = &compass_methods,
	},

	{
		.tag = SENSOR_CAL_ALGO_TAG,
		.version = SENSOR_CAL_ALGO_VERSION,
		.type = SENSOR_TYPE_ORIENTATION,
		.compatible = orientation_match_table,
		.module = &SENSOR_CAL_MODULE_INFO,
		.methods = &orientation_methods,
	},

	{
		.tag = SENSOR_CAL_ALGO_TAG,
		.version = SENSOR_CAL_ALGO_VERSION,
		.type = SENSOR_TYPE_ROTATION_VECTOR,
		.compatible = rotation_vector_match_table,
		.module = &SENSOR_CAL_MODULE_INFO,
		.methods = &rotation_vector_methods,
	},

	{
		.tag = SENSOR_CAL_ALGO_TAG,
		.version = SENSOR_CAL_ALGO_VERSION,
		.type = SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED,
		.compatible = mag_uncalib_match_table,
		.module = &SENSOR_CAL_MODULE_INFO,
		.methods = &mag_uncalib_methods,
	},

};

static struct sensor_cal_methods_t cal_methods = {
	.init = cal_init,
	.deinit = cal_deinit,
	.get_algo_list = cal_get_algo_list,
};

struct sensor_cal_module_t SENSOR_CAL_MODULE_INFO = {
	.tag = SENSOR_CAL_MODULE_TAG,
	.id = "cal_module_common",
	.version = SENSOR_CAL_MODULE_VERSION,
	.vendor = "common",
	.dso = NULL,
	.number = ARRAY_SIZE(algo_list),
	.methods = &cal_methods,
	.reserved = {0},
};

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

#ifndef _SENSOR_CAL_H
#define _SENSOR_CAL_H

#include <sys/cdefs.h>
#include <hardware/hardware.h>
#include <hardware/sensors.h>

__BEGIN_DECLS

#define ACCELEROMETER_NAME	"accelerometer"
#define COMPASS_NAME		"compass"
#define ORIENTATION_NAME	"orientation"
#define GYROSCOPE_NAME		"gyroscope"
#define LIGHT_NAME		"light"
#define PRESSURE_NAME		"pressure"
#define TEMPERATURE_NAME	"temperature"
#define PROXIMITY_NAME		"proximity"
#define GRAVITY_NAME		"gravity"
#define LINEAR_ACCELERATION_NAME	"liner_acceleration"
#define ROTATION_VECTOR_NAME	"rotation_vector"
#define RELATIVE_HUMIDITY_NAME	"relative_humidity"
#define AMBIENT_TEMPERATURE_NAME	"ambient_temperature"
#define MAGNETIC_FIELD_UNCALIBRATED_NAME	"magnetic_field_uncalibrated"
#define GAME_ROTATION_VECTOR_NAME	"game_rotation_vector"
#define GYROSCOPE_UNCALIBRATED_NAME	"gyroscope_uncalibrated"
#define SIGNIFICANT_MOTION_NAME		"significant_motion"
#define STEP_COUNTER_NAME		"step_counter"
#define STEP_DETECTOR_NAME		"step_detector"
#define GEOMAGNETIC_ROTATION_VECTOR_NAME	"geomagnetic_field"

/* TAG for the module. Should be "scmt" */
#define SENSOR_CAL_MODULE_TAG	0x746d6373
#define SENSOR_CAL_ALGO_TAG	0x74616373

/* Super symbol of the calibration lib */
#define SENSOR_CAL_MODULE_INFO	scmi
#define SENSOR_CAL_MODULE_INFO_AS_STR	"scmi"

#define SENSOR_CAL_MODULE_VERSION	1

enum {
	CMD_ENABLE = 0, /* Enable status changed */
	CMD_DELAY, /* Polling rate changed */
	CMD_BATCH, /* Batching parameter changed */
};

struct sensor_cal_algo_t;
struct sensor_cal_module_t;

struct sensor_algo_args {
	int enable;
	int delay_ms;
};

struct compass_algo_args {
	struct sensor_algo_args common;
	uint32_t reserved[16];
};

struct sensor_algo_methods_t {
	int (*convert)(sensors_event_t *raw, sensors_event_t *result, struct sensor_algo_args *args);
	/* Note that the config callback is called from a different thread as convert */
	int (*config)(int cmd, struct sensor_algo_args *args);
};

struct sensor_cal_methods_t {
	int (*init)(const struct sensor_cal_module_t* module);
	void (*deinit)();
	/* Return 0 on success */
	int (*get_algo_list)(const struct sensor_cal_algo_t **algo);
};

struct sensor_cal_algo_t {
	/* Tag of the algo */
	int		tag;
	/* Version of the algo */
	int		version;
	/* Type of sensor this algo supported*/
	int		type;
	/* The compatible sensors */
	const char	**compatible;
	/* Sensor calibration module */
	struct sensor_cal_module_t	*module;
	/* Sensor algo methods */
	struct sensor_algo_methods_t	*methods;
};

struct sensor_cal_module_t {
	/* Tag of the module */
	uint32_t	tag;
	/* Id of the module */
	char		*id;
	/* Version of the calibration module */
	uint32_t	version;
	/* Vendor of the calibration lib */
	char		*vendor;
	/* Point to the handle of this module */
	void		*dso;
	/* Number of algos */
	uint32_t	number;
	/* Callbacks of the calibration lib provided */
	struct sensor_cal_methods_t	*methods;
	/* The compatible sensors list for this library */
	int		reserved[6];
};

__END_DECLS
#endif

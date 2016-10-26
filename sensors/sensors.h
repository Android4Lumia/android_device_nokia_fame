/*
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

#ifndef ANDROID_SENSORS_H
#define ANDROID_SENSORS_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <cstring>

#include <linux/input.h>

#include <hardware/hardware.h>
#include <hardware/sensors.h>

__BEGIN_DECLS

/*****************************************************************************/

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define SENSORS_ACCELERATION_HANDLE		0
#define SENSORS_MAGNETIC_FIELD_HANDLE		1
#define SENSORS_ORIENTATION_HANDLE		2
#define SENSORS_LIGHT_HANDLE			3
#define SENSORS_PROXIMITY_HANDLE		4
#define SENSORS_GYROSCOPE_HANDLE		5
#define SENSORS_PRESSURE_HANDLE			6

#define MAX_SENSORS		(20)
#define SYSFS_MAXLEN		(20)
#define SYSFS_CLASS		"/sys/class/sensors/"
#define SYSFS_NAME		"name"
#define SYSFS_VENDOR		"vendor"
#define SYSFS_VERSION		"version"
#define SYSFS_HANDLE		"handle"
#define SYSFS_TYPE		"type"
#define SYSFS_MAXRANGE		"max_range"
#define SYSFS_RESOLUTION	"resolution"
#define SYSFS_POWER		"sensor_power"
#define SYSFS_MINDELAY		"min_delay"
#define SYSFS_ENABLE		"enable"
#define SYSFS_POLL_DELAY	"poll_delay"
#define SYSFS_CALIBRATE		"calibrate"
#define SYSFS_CAL_PARAMS	"cal_params"

#define COMPASS_VENDOR_AKM		"AKM"
#define COMPASS_VENDOR_ALPS		"Alps"
#define COMPASS_VENDOR_YAMAHA		"Yamaha"
#define COMPASS_VENDOR_MEMSIC		"Memsic"
#define COMPASS_VENDOR_ST		"STMicro"
#define COMPASS_VENDOR_BOSCH		"Bosch"
#define COMPASS_VENDOR_KIONIX		"Kionix"
#define COMPASS_VENDOR_INVENSENSE	"Invensense"

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

/* The hardware sensor type supported by HAL */
#define SUPPORTED_SENSORS_TYPE	( \
		(1ULL << SENSOR_TYPE_ACCELEROMETER) | \
		(1ULL << SENSOR_TYPE_MAGNETIC_FIELD) | \
		(1ULL << SENSOR_TYPE_PROXIMITY) | \
		(1ULL << SENSOR_TYPE_LIGHT) | \
		(1ULL << SENSOR_TYPE_GYROSCOPE ) | \
		(1ULL << SENSOR_TYPE_PRESSURE))


/*****************************************************************************/
/* Helper function to convert sensor type to common sensor name */
static inline const char* type_to_name(int type)
{
	switch (type) {
		case SENSOR_TYPE_ACCELEROMETER:
			return ACCELEROMETER_NAME;
		case SENSOR_TYPE_GEOMAGNETIC_FIELD:
			return COMPASS_NAME;
		case SENSOR_TYPE_ORIENTATION:
			return ORIENTATION_NAME;
		case SENSOR_TYPE_GYROSCOPE:
			return GYROSCOPE_NAME;
		case SENSOR_TYPE_LIGHT:
			return LIGHT_NAME;
		case SENSOR_TYPE_PRESSURE:
			return PRESSURE_NAME;
		case SENSOR_TYPE_TEMPERATURE:
			return TEMPERATURE_NAME;
		case SENSOR_TYPE_PROXIMITY:
			return PROXIMITY_NAME;
		case SENSOR_TYPE_GRAVITY:
			return GRAVITY_NAME;
		case SENSOR_TYPE_LINEAR_ACCELERATION:
			return LINEAR_ACCELERATION_NAME;
		case SENSOR_TYPE_ROTATION_VECTOR:
			return ROTATION_VECTOR_NAME;
		case SENSOR_TYPE_RELATIVE_HUMIDITY:
			return RELATIVE_HUMIDITY_NAME;
		case SENSOR_TYPE_AMBIENT_TEMPERATURE:
			return AMBIENT_TEMPERATURE_NAME;
		case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
			return MAGNETIC_FIELD_UNCALIBRATED_NAME;
		case SENSOR_TYPE_GAME_ROTATION_VECTOR:
			return GAME_ROTATION_VECTOR_NAME;
		case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
			return GYROSCOPE_UNCALIBRATED_NAME;
		case SENSOR_TYPE_SIGNIFICANT_MOTION:
			return SIGNIFICANT_MOTION_NAME;
		case SENSOR_TYPE_STEP_DETECTOR:
			return STEP_DETECTOR_NAME;
		case SENSOR_TYPE_STEP_COUNTER:
			return STEP_COUNTER_NAME;
		case SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR:
			return GEOMAGNETIC_ROTATION_VECTOR_NAME;
		default:
			return "";
	}
}

__END_DECLS

#endif  // ANDROID_SENSORS_H

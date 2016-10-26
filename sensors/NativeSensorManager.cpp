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
#include "NativeSensorManager.h"

ANDROID_SINGLETON_STATIC_INSTANCE(NativeSensorManager);

enum {
	ORIENTATION = 0,
	PSEUDO_GYROSCOPE,
	ROTATION_VECTOR,
	LINEAR_ACCELERATION,
	GRAVITY,
	VIRTUAL_SENSOR_COUNT,
};

const struct sensor_t NativeSensorManager::virtualSensorList [VIRTUAL_SENSOR_COUNT] = {
	[ORIENTATION] = {
		.name = "oem-orientation",
		.vendor = "oem",
		.version = 1,
		.handle = '_dmy',
		.type = SENSOR_TYPE_ORIENTATION,
		.maxRange = 360.0f,
		.resolution = 1.0f/256.0f,
		.power = 1,
		.minDelay = 10000,
		.fifoReservedEventCount = 0,
		.fifoMaxEventCount = 0,
#if defined(SENSORS_DEVICE_API_VERSION_1_3)
		.stringType = NULL,
		.requiredPermission = NULL,
		.maxDelay = 0,
		.flags = 0,
#endif
		.reserved = {},
	},

	[PSEUDO_GYROSCOPE] = {
		.name = "oem-pseudo-gyro",
		.vendor = "oem",
		.version = 1,
		.handle = '_dmy',
		.type = SENSOR_TYPE_GYROSCOPE,
		.maxRange = 50.0f,
		.resolution = 0.01f,
		.power = 1,
		.minDelay = 10000,
		.fifoReservedEventCount = 0,
		.fifoMaxEventCount = 0,
#if defined(SENSORS_DEVICE_API_VERSION_1_3)
		.stringType = NULL,
		.requiredPermission = NULL,
		.maxDelay = 0,
		.flags = 0,
#endif
		.reserved = {},
	},

	[ROTATION_VECTOR] = {
		.name = "oem-rotation-vector",
		.vendor = "oem",
		.version = 1,
		.handle = '_dmy',
		.type = SENSOR_TYPE_ROTATION_VECTOR,
		.maxRange = 1,
		.resolution = 1.0f / (1<<24),
		.power = 1,
		.minDelay = 10000,
		.fifoReservedEventCount = 0,
		.fifoMaxEventCount = 0,
#if defined(SENSORS_DEVICE_API_VERSION_1_3)
		.stringType = NULL,
		.requiredPermission = NULL,
		.maxDelay = 0,
		.flags = 0,
#endif
		.reserved = {},
	},

	[LINEAR_ACCELERATION] = {
		.name = "oem-linear-acceleration",
		.vendor = "oem",
		.version = 1,
		.handle = '_dmy',
		.type = SENSOR_TYPE_LINEAR_ACCELERATION,
		.maxRange = 40.0f,
		.resolution = 0.01f,
		.power = 1,
		.minDelay = 10000,
		.fifoReservedEventCount = 0,
		.fifoMaxEventCount = 0,
#if defined(SENSORS_DEVICE_API_VERSION_1_3)
		.stringType = NULL,
		.requiredPermission = NULL,
		.maxDelay = 0,
		.flags = 0,
#endif
		.reserved = {},
	},

	[GRAVITY] = {
		.name = "oem-gravity",
		.vendor = "oem",
		.version = 1,
		.handle = '_dmy',
		.type = SENSOR_TYPE_GRAVITY,
		.maxRange = 40.0f,
		.resolution = 0.01f,
		.power = 1,
		.minDelay = 10000,
		.fifoReservedEventCount = 0,
		.fifoMaxEventCount = 0,
#if defined(SENSORS_DEVICE_API_VERSION_1_3)
		.stringType = NULL,
		.requiredPermission = NULL,
		.maxDelay = 0,
		.flags = 0,
#endif
		.reserved = {},
	},
};

int NativeSensorManager::initVirtualSensor(struct SensorContext *ctx, int handle, int64_t dep,
		struct sensor_t info)
{
	CalibrationManager& cm(CalibrationManager::getInstance());
	SensorRefMap *item;
	struct SensorContext *ref;
	unsigned int i;

	*(ctx->sensor) = info;
	if (cm.getCalAlgo(ctx->sensor) == NULL) {
		return -1;
	}

	ctx->sensor->handle = handle;
	ctx->driver = new VirtualSensor(ctx);
	ctx->data_fd = -1;
	ctx->data_path = NULL;
	ctx->enable_path = NULL;
	ctx->is_virtual = true;

	for (i = 0; i < sizeof(dep) * 8; i++) {
		if (dep & (1ULL << i)) {
			ref = getInfoByType(i);
			if (ref != NULL) {
				item = new SensorRefMap;
				item->ctx = ref;
				list_add_tail(&ctx->dep_list, &item->list);
			}
		}
	}

	type_map.add(ctx->sensor->type, ctx);
	handle_map.add(ctx->sensor->handle, ctx);

	return 0;
}


const struct SysfsMap NativeSensorManager::node_map[] = {
	{offsetof(struct sensor_t, name), SYSFS_NAME, TYPE_STRING},
	{offsetof(struct sensor_t, vendor), SYSFS_VENDOR, TYPE_STRING},
	{offsetof(struct sensor_t, version), SYSFS_VERSION, TYPE_INTEGER},
	{offsetof(struct sensor_t, type), SYSFS_TYPE, TYPE_INTEGER},
	{offsetof(struct sensor_t, maxRange), SYSFS_MAXRANGE, TYPE_FLOAT},
	{offsetof(struct sensor_t, resolution), SYSFS_RESOLUTION, TYPE_FLOAT},
	{offsetof(struct sensor_t, power), SYSFS_POWER, TYPE_FLOAT},
	{offsetof(struct sensor_t, minDelay), SYSFS_MINDELAY, TYPE_INTEGER},
};

NativeSensorManager::NativeSensorManager():
	mSensorCount(0), type_map(NULL), handle_map(NULL), fd_map(NULL)
{
	int i;

	memset(sensor_list, 0, sizeof(sensor_list));
	memset(context, 0, sizeof(context));

	type_map.setCapacity(MAX_SENSORS);
	handle_map.setCapacity(MAX_SENSORS);
	fd_map.setCapacity(MAX_SENSORS);

	for (i = 0; i < MAX_SENSORS; i++) {
		context[i].sensor = &sensor_list[i];
		sensor_list[i].name = context[i].name;
		sensor_list[i].vendor = context[i].vendor;
		list_init(&context[i].listener);
		list_init(&context[i].dep_list);
	}

	if(getDataInfo()) {
		ALOGE("Get data info failed\n");
	}

	dump();
}

NativeSensorManager::~NativeSensorManager()
{
	int i;
	int number = getSensorCount();
	struct listnode *node;
	struct listnode *n;
	struct SensorContext *ctx;
	struct SensorRefMap *item;

	for (i = 0; i < number; i++) {
		if (context[i].driver != NULL) {
			delete context[i].driver;
		}

		list_for_each_safe(node, n, &context[i].listener) {
			item = node_to_item(node, struct SensorRefMap, list);
			if (item != NULL) {
				list_remove(&item->list);
				delete item;
			}
		}

		list_for_each_safe(node, n, &context[i].dep_list) {
			item = node_to_item(node, struct SensorRefMap, list);
			if (item != NULL) {
				list_remove(&item->list);
				delete item;
			}
		}
	}
}

void NativeSensorManager::dump()
{
	int i;
	struct listnode *node;
	struct SensorRefMap* ref;

	for (i = 0; i < mSensorCount; i++) {
		ALOGI("\nname:%s\ntype:%d\nhandle:%d\ndata_fd=%d\nis_virtual=%d",
				context[i].sensor->name,
				context[i].sensor->type,
				context[i].sensor->handle,
				context[i].data_fd,
				context[i].is_virtual);

		ALOGI("data_path=%s\nenable_path=%s\ndelay_ns:%lld\nenable=%d\n",
				context[i].data_path,
				context[i].enable_path,
				context[i].delay_ns,
				context[i].enable);

		ALOGI("Listener:");
		list_for_each(node, &context[i].listener) {
			ref = node_to_item(node, struct SensorRefMap, list);
			ALOGI("name:%s handle:%d\n", ref->ctx->sensor->name, ref->ctx->sensor->handle);
		}

		ALOGI("Dependency:");
		list_for_each(node, &context[i].dep_list) {
			ref = node_to_item(node, struct SensorRefMap, list);
			ALOGI("name:%s handle:%d", ref->ctx->sensor->name, ref->ctx->sensor->handle);
		}
	}

	ALOGI("\n");
}

int NativeSensorManager::getDataInfo() {
	struct dirent **namelist;
	char *file;
	char path[PATH_MAX];
	char name[80];
	int nNodes;
	int i, j;
	int fd = -1;
	struct SensorContext *list;
	int has_acc = 0;
	int has_compass = 0;
	int has_gyro = 0;
	int event_count = 0;
	struct sensor_t sensor_mag;

	strlcpy(path, EVENT_PATH, sizeof(path));
	file = path + strlen(EVENT_PATH);
	nNodes = scandir(path, &namelist, 0, alphasort);
	if (nNodes < 0) {
		ALOGE("scan %s failed.(%s)\n", EVENT_PATH, strerror(errno));
		return -1;
	}

	for (event_count = 0, j = 0; (j < nNodes) && (j < MAX_SENSORS); j++) {
		if (namelist[j]->d_type != DT_CHR) {
			continue;
		}

		strlcpy(file, namelist[j]->d_name, sizeof(path) - strlen(EVENT_PATH));

		fd = open(path, O_RDONLY);
		if (fd < 0) {
			ALOGE("open %s failed(%s)", path, strerror(errno));
			continue;
		}

		if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
			name[0] = '\0';
		}

		strlcpy(event_list[event_count].data_name, name, sizeof(event_list[0].data_name));
		strlcpy(event_list[event_count].data_path, path, sizeof(event_list[0].data_path));
		close(fd);
		event_count++;
	}

	for (j = 0; j <nNodes; j++ ) {
		free(namelist[j]);
	}

	free(namelist);

	mSensorCount = getSensorListInner();
	for (i = 0; i < mSensorCount; i++) {
		struct SensorRefMap *item;
		list = &context[i];
		list->is_virtual = false;

		item = new struct SensorRefMap;
		item->ctx = list;
		list_add_tail(&list->dep_list, &item->list);

		/* Initialize data_path and data_fd */
		for (j = 0; (j < event_count) && (j < MAX_SENSORS); j++) {
			if (strcmp(list->sensor->name, event_list[j].data_name) == 0) {
				list->data_path = strdup(event_list[j].data_path);
				break;
			}

			if (strcmp(event_list[j].data_name, type_to_name(list->sensor->type)) == 0) {
				list->data_path = strdup(event_list[j].data_path);
			}
		}

		if (list->data_path != NULL)
			list->data_fd = open(list->data_path,O_RDONLY | O_CLOEXEC | O_NONBLOCK);
		else
			list->data_fd = -1;

		if (list->data_fd > 0) {
			fd_map.add(list->data_fd, list);
		} else {
			ALOGE("open %s failed, continue anyway.(%s)\n", list->data_path, strerror(errno));
		}

		type_map.add(list->sensor->type, list);
		handle_map.add(list->sensor->handle, list);

		switch (list->sensor->type) {
			case SENSOR_TYPE_ACCELEROMETER:
				has_acc = 1;
				list->driver = new AccelSensor(list);
				break;
			case SENSOR_TYPE_MAGNETIC_FIELD:
				has_compass = 1;
				list->driver = new CompassSensor(list);
				sensor_mag = *(list->sensor);
				break;
			case SENSOR_TYPE_PROXIMITY:
				list->driver = new ProximitySensor(list);
				break;
			case SENSOR_TYPE_LIGHT:
				list->driver = new LightSensor(list);
				break;
			case SENSOR_TYPE_GYROSCOPE:
				has_gyro = 1;
				list->driver = new GyroSensor(list);
				break;
			default:
				list->driver = NULL;
				ALOGE("No handle %d for this type sensor!", i);
				break;
		}
		initCalibrate(list);
	}


	/* Some vendor or the reference design implements some virtual sensors
	 * or pseudo sensors. These sensors are required by some of the applications.
	 * Here we check the CalibratoinManager to decide whether to enable them.
	 */
	CalibrationManager &cm(CalibrationManager::getInstance());
	struct SensorRefMap *ref;

	if (has_compass) {
		/* The uncalibrated magnetic field sensor shares the same vendor/name as the
		 * calibrated one. */
		sensor_mag.type = SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
		if (!initVirtualSensor(&context[mSensorCount], SENSORS_HANDLE(mSensorCount),
					1ULL << SENSOR_TYPE_MAGNETIC_FIELD, sensor_mag)) {
			mSensorCount++;
		}
	}

	if (has_acc && has_compass) {
		int dep = (1ULL << SENSOR_TYPE_ACCELEROMETER) | (1ULL << SENSOR_TYPE_MAGNETIC_FIELD);

		/* HAL implemented orientation. Android will replace it for
		 * platform with Gyro with SensorFusion.
		 * The calibration manager will first match "oem-orientation" and
		 * then match "orientation" to select the algorithms. */
		if (!initVirtualSensor(&context[mSensorCount], SENSORS_HANDLE(mSensorCount), dep,
					virtualSensorList[ORIENTATION])) {
			mSensorCount++;
		}

		if (!has_gyro) {
			/* Pseudo gyroscope is a pseudo sensor which implements by accelerometer and
			 * magnetometer. Some sensor vendors provide such implementations. The pseudo
			 * gyroscope sensor is low cost but the performance is worse than the actual
			 * gyroscope. So disable it for the system with actual gyroscope. */
			if (!initVirtualSensor(&context[mSensorCount], SENSORS_HANDLE(mSensorCount), dep,
						virtualSensorList[PSEUDO_GYROSCOPE])) {
				mSensorCount++;
			}

			/* For linear acceleration */
			if (!initVirtualSensor(&context[mSensorCount], SENSORS_HANDLE(mSensorCount), dep,
						virtualSensorList[LINEAR_ACCELERATION])) {
				mSensorCount++;
			}

			/* For rotation vector */
			if (!initVirtualSensor(&context[mSensorCount], SENSORS_HANDLE(mSensorCount), dep,
						virtualSensorList[ROTATION_VECTOR])) {
				mSensorCount++;
			}

			/* For gravity */
			if (!initVirtualSensor(&context[mSensorCount], SENSORS_HANDLE(mSensorCount), dep,
						virtualSensorList[GRAVITY])) {
				mSensorCount++;
			}
		}
	}

	return 0;
}

/* Register a listener on "hw" for "virt".
 * The "hw" specify the actual background sensor type, and "virt" is one kind of virtual sensor.
 * Generally the virtual sensor specified by "virt" can only work when the hardware sensor specified
 * by "hw" is activiated.
 */
int NativeSensorManager::registerListener(struct SensorContext *hw, struct SensorContext *virt)
{
	struct listnode *node;
	struct SensorContext *ctx;
	struct SensorRefMap *item;

	list_for_each(node, &hw->listener) {
		item = node_to_item(node, struct SensorRefMap, list);
		if (item->ctx->sensor->handle == virt->sensor->handle) {
			ALOGE("Already registered as listener for %s:%s\n", hw->sensor->name, virt->sensor->name);
			return -1;
		}
	}

	item = new SensorRefMap;
	item->ctx = virt;

	list_add_tail(&hw->listener, &item->list);

	return 0;
}

/* Remove the virtual sensor listener from the list specified by "hw" */
int NativeSensorManager::unregisterListener(struct SensorContext *hw, struct SensorContext *virt)
{
	struct listnode *node;
	struct listnode *n;
	struct SensorContext *ctx;
	struct SensorRefMap *item;

	list_for_each_safe(node, n, &hw->listener) {
		item = node_to_item(node, struct SensorRefMap, list);
		if (item->ctx == virt) {
			list_remove(&item->list);
			delete item;
			return 0;
		}
	}

	ALOGE("%s is not a listener of %s\n", virt->sensor->name, hw->sensor->name);
	return -1;
}

int NativeSensorManager::getSensorList(const sensor_t **list) {
	*list = mSensorCount ? sensor_list:NULL;

	return mSensorCount;
}

int NativeSensorManager::getNode(char *buf, char *path, const struct SysfsMap *map) {
	char * fret;
	ssize_t len = 0;
	int fd;
	char tmp[SYSFS_MAXLEN];

	if (NULL == buf || NULL == path)
		return -1;

	memset(tmp, 0, sizeof(tmp));

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		ALOGE("open %s failed.(%s)\n", path, strerror(errno));
		return -1;
	}

	len = read(fd, tmp, sizeof(tmp) - 1);
	if ((len <= 0) || (strlen(tmp) == 0)) {
		ALOGE("read %s failed.(%s)\n", path, strerror(errno));
		close(fd);
		return -1;
	}

	tmp[len - 1] = '\0';

	if (tmp[strlen(tmp) - 1] == '\n')
		tmp[strlen(tmp) - 1] = '\0';

	if (map->type == TYPE_INTEGER) {
		int *p = (int *)(buf + map->offset);
		*p = atoi(tmp);
	} else if (map->type == TYPE_STRING) {
		char **p = (char **)(buf + map->offset);
		strlcpy(*p, tmp, SYSFS_MAXLEN);
	} else if (map->type == TYPE_FLOAT) {
		float *p = (float*)(buf + map->offset);
		*p = atof(tmp);
	}

	close(fd);
	return 0;
}

int NativeSensorManager::getSensorListInner()
{
	int number = 0;
	int err = -1;
	const char *dirname = SYSFS_CLASS;
	char devname[PATH_MAX];
	char *filename;
	char *nodename;
	DIR *dir;
	struct dirent *de;
	struct SensorContext *list;
	unsigned int i;

	dir = opendir(dirname);
	if(dir == NULL) {
		return 0;
	}
	strlcpy(devname, dirname, PATH_MAX);
	filename = devname + strlen(devname);

	while ((de = readdir(dir))) {
		if(de->d_name[0] == '.' &&
			(de->d_name[1] == '\0' ||
				(de->d_name[1] == '.' && de->d_name[2] == '\0')))
			continue;

		list = &context[number];

		strlcpy(filename, de->d_name, PATH_MAX - strlen(SYSFS_CLASS));
		nodename = filename + strlen(de->d_name);
		*nodename++ = '/';

		for (i = 0; i < ARRAY_SIZE(node_map); i++) {
			strlcpy(nodename, node_map[i].node, PATH_MAX - strlen(SYSFS_CLASS) - strlen(de->d_name));
			err = getNode((char*)(list->sensor), devname, &node_map[i]);
			if (err)
				break;
		}

		if (i < ARRAY_SIZE(node_map))
			continue;

		if (!((1ULL << list->sensor->type) & SUPPORTED_SENSORS_TYPE))
			continue;

		/* Setup other information */
		list->sensor->handle = SENSORS_HANDLE(number);
		list->data_path = NULL;

		strlcpy(nodename, "", SYSFS_MAXLEN);
		list->enable_path = strdup(devname);

		number++;
	}
	closedir(dir);
	return number;
}

int NativeSensorManager::activate(int handle, int enable)
{
	SensorContext *list;
	int i;
	int number = getSensorCount();
	int err = 0;
	struct listnode *node;
	struct SensorContext *ctx;
	struct SensorRefMap *item;

	list = getInfoByHandle(handle);
	if (list == NULL) {
		ALOGE("Invalid handle(%d)", handle);
		return -EINVAL;
	}

	/* Search for the background sensor for the sensor specified by handle. */
	list_for_each(node, &list->dep_list) {
		item = node_to_item(node, struct SensorRefMap, list);
		if (enable) {
			/* Enable the background sensor and register a listener on it. */
			err = item->ctx->driver->enable(item->ctx->sensor->handle, 1);
			if (!err) {
				registerListener(item->ctx, list);
			}
		} else {
			/* The background sensor has other listeners, we need
			 * to unregister the current sensor from it and sync the
			 * poll delay settings.
			 */
			if (!list_empty(&item->ctx->listener)) {
				unregisterListener(item->ctx, list);
				/* We're activiating the hardware sensor itself */
				if ((item->ctx == list) && (item->ctx->enable))
					item->ctx->enable = 0;
				syncDelay(item->ctx->sensor->handle);
			}

			/* Disable the background sensor if it doesn't have any listeners. */
			if (list_empty(&item->ctx->listener)) {
				item->ctx->driver->enable(item->ctx->sensor->handle, 0);
			}
		}
	}

	list->enable = enable;

	return err;
}

int NativeSensorManager::syncDelay(int handle)
{
	const SensorRefMap *item;
	SensorContext *ctx;
	const SensorContext *list;
	struct listnode *node;
	int64_t min_ns;

	list = getInfoByHandle(handle);
	if (list == NULL) {
		ALOGE("Invalid handle(%d)", handle);
		return -EINVAL;
	}

	if (list_empty(&list->listener)) {
		min_ns = list->delay_ns;
	} else {
		node = list_head(&list->listener);
		item = node_to_item(node, struct SensorRefMap, list);
		min_ns = item->ctx->delay_ns;

		list_for_each(node, &list->listener) {
			item = node_to_item(node, struct SensorRefMap, list);
			ctx = item->ctx;
			/* To handle some special case that the polling delay is 0. This
			 * may happen if the background sensor is not enabled but the virtual
		         * sensor is enabled case.
			 */
			if (ctx->delay_ns == 0) {
				ALOGW("Listener delay is 0. Fix it to minDelay");
				ctx->delay_ns = ctx->sensor->minDelay;
			}

			if (min_ns > ctx->delay_ns)
				min_ns = ctx->delay_ns;
		}
	}

	if ((list->delay_ns != 0) && (list->delay_ns < min_ns) &&
			(list->enable))
		min_ns = list->delay_ns;

	return list->driver->setDelay(list->sensor->handle, min_ns);
}

int NativeSensorManager::setDelay(int handle, int64_t ns)
{
	SensorContext *list;
	int i;
	int number = getSensorCount();
	int64_t delay = ns;
	struct SensorRefMap *item;
	struct listnode *node;


	list = getInfoByHandle(handle);
	if (list == NULL) {
		ALOGE("Invalid handle(%d)", handle);
		return -EINVAL;
	}

	list->delay_ns = delay;

	// min_delay sysfs entry is in microseconds
	if (ns < list->sensor->minDelay * 1000) {
		list->delay_ns = list->sensor->minDelay * 1000;
	}

	if (list->delay_ns == 0)
		list->delay_ns = 1000000; //  clamped to 1ms

	list_for_each(node, &list->dep_list) {
		item = node_to_item(node, struct SensorRefMap, list);
		syncDelay(item->ctx->sensor->handle);
	}

	return 0;
}

int NativeSensorManager::readEvents(int handle, sensors_event_t* data, int count)
{
	const SensorContext *list;
	int i, j;
	int number = getSensorCount();
	int nb;
	struct listnode *node;
	struct SensorRefMap *item;

	list = getInfoByHandle(handle);
	if (list == NULL) {
		ALOGE("Invalid handle(%d)", handle);
		return -EINVAL;
	}
	do {
		nb = list->driver->readEvents(data, count);
	} while ((nb == -EAGAIN) || (nb == -EINTR));

	for (j = 0; j < nb; j++) {
		list_for_each(node, &list->listener) {
			item = node_to_item(node, struct SensorRefMap, list);
			if (item->ctx->enable) {
				item->ctx->driver->injectEvents(&data[j], 1);
			}
		}
	}

	if (list->enable)
		return nb;

	/* No need to report the events if the sensor is not enabled */
	return 0;
}

int NativeSensorManager::hasPendingEvents(int handle)
{
	const SensorContext *list;

	list = getInfoByHandle(handle);
	if (list == NULL) {
		ALOGE("Invalid handle(%d)", handle);
		return -EINVAL;
	}

	return list->driver->hasPendingEvents();
}

int NativeSensorManager::calibrate(int handle, struct cal_cmd_t *para)
{
	const SensorContext *list;
	struct cal_result_t cal_result;
	sensors_XML& sensor_XML(sensors_XML :: getInstance());
	int err;

	list = getInfoByHandle(handle);
	if(list == NULL) {
		ALOGE("Invalid handle(%d)", handle);
		return -EINVAL;
	}
	sensor_XML.sensors_rm_file();
	memset(&cal_result, 0, sizeof(cal_result));
	err = list->driver->calibrate(handle, para, &cal_result);
	if (err < 0) {
		ALOGE("calibrate %s sensor error\n", list->sensor->name);
		return err;
	}
	if (!para->save) {
		return err;
	}
	err = sensor_XML.write_sensors_params(list->sensor, &cal_result);
	if (err < 0) {
		ALOGE("write calibrate %s sensor error\n", list->sensor->name);
		return err;
	}
	return err;
}

int NativeSensorManager::initCalibrate(const SensorContext *list)
{
	struct cal_result_t cal_result;
	sensors_XML& sensor_XML(sensors_XML :: getInstance());
	int err = 0;

	if(list == NULL) {
		ALOGE("Invalid sensor\n");
		return -EINVAL;
	}
	memset(&cal_result, 0, sizeof(cal_result));
	err = sensor_XML.read_sensors_params(list->sensor, &cal_result);
	if (err < 0) {
		ALOGE("read calibrate params error\n");
		return err;
	}

	err = list->driver->initCalibrate(list->sensor->handle, &cal_result);
	if (err < 0) {
		ALOGE("init sensor %s calibrate params error\n", list->sensor->name);
	}
	return err;
}

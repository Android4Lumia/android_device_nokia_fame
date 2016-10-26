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

#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <utils/Log.h>
#include <CalibrationModule.h>
#include "sensors.h"
#include "CalibrationManager.h"

ANDROID_SINGLETON_STATIC_INSTANCE(CalibrationManager);

int CalibrationManager::check_algo(const sensor_cal_algo_t *list)
{
	if (list->tag != SENSOR_CAL_ALGO_TAG)
		return -1;
	if ((list->type < SENSOR_TYPE_ACCELEROMETER) ||
			(list->type > SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR))
		return -1;
	if (list->compatible == NULL)
		return -1;
	if (list->module == NULL)
		return -1;
	if (list->methods == NULL)
		return -1;
	if (list->methods->convert == NULL)
		return -1;
	return 0;
}

/* Return 0 on success */
void CalibrationManager::loadCalLibs()
{
	int fd;
	size_t len;
	char buf[MAX_CAL_CFG_LEN];
	char* cal_libs[MAX_CAL_LIBS];
	struct sensor_cal_module_t* modules[MAX_CAL_LIBS];
	int i = 0;
	int count;
	int tmp;

	algo_count = 0;
	algo_list = NULL;

	cal_libs[0] = strdup(DEFAULT_CAL_LIB);
	cal_libs[1] = NULL;
	count = 1;

	fd = open(CAL_LIB_CFG_PATH, O_RDONLY);
	if (fd < 0) {
		ALOGE("Open %s failed.(%s)\nDrop to default calibration library.",
				CAL_LIB_CFG_PATH, strerror(errno));
	} else {
		len = read(fd, buf, MAX_CAL_CFG_LEN);
		if (len > 0) {
			char *save_ptr, *str, *token;

			buf[len] = '\0';
			for(str = buf; ;str = NULL) {
				token = strtok_r(str, "\n", &save_ptr);
				if (token == NULL)
					break;
				cal_libs[i++] = strdup(token);
			}
			cal_libs[i] = NULL;
			count = i;
		}
	}

	/* Load the libraries */
	for (i = 0; i < count; i++) {
		void* dso;
		char path[PATH_MAX];

		ALOGI("Found calibration library:%s\n", cal_libs[i]);
		strlcpy(path, CAL_LIB_PATH, sizeof(path));
		strlcat(path, cal_libs[i], sizeof(path));
		if (access(path, F_OK) != 0) {
			ALOGE("module %s doesn't exist(%s)", cal_libs[i], strerror(errno));
			modules[i] = NULL;
			free(cal_libs[i]);
			continue;
		}

		free(cal_libs[i]);

		dso = dlopen(path, RTLD_NOW);
		if (dso == NULL) {
			char const *err_str = dlerror();
			ALOGE("load module %s failed(%s)", path, err_str?err_str:"unknown");
			modules[i] = NULL;
			continue;
		}

		modules[i] = (sensor_cal_module_t*)dlsym(dso, SENSOR_CAL_MODULE_INFO_AS_STR);
		if (modules[i] == NULL) {
			ALOGE("Can't find symbol %s\n", SENSOR_CAL_MODULE_INFO_AS_STR);
			continue;
		}

		modules[i]->dso = dso;

		if (modules[i]->methods->init(modules[i])) {
			ALOGE("init %s failed\n", modules[i]->id);
			modules[i] = NULL;
			continue;
		}
		algo_count += modules[i]->number;
	}

	if (algo_count != 0) {
		tmp = 0;

		algo_list = new const sensor_cal_algo_t *[algo_count];
		/* Get the algo list */
		for (i = 0; i < count; i++) {
			const sensor_cal_algo_t *list;
			/* Success */
			if ((modules[i] != NULL) && (modules[i]->methods != NULL) &&
					(modules[i]->methods->get_algo_list != NULL) &&
					(modules[i]->methods->get_algo_list(&list) == 0)) {
				for (uint32_t j = 0; j < modules[i]->number; j++)
					algo_list[tmp + j] = &list[j];

				tmp += modules[i]->number;
			}
		}
	}

	dump();
}

CalibrationManager::CalibrationManager()
	:algo_list(NULL), algo_count(0)
{
	loadCalLibs();
}

CalibrationManager::~CalibrationManager()
{
	void *dso = NULL;

	/* The following resource clean up assumes the algorithms in algo_list
	 * are listed in library sequence.
	 * e.g. algo_list[0~3] are point to the lib_foo, algo_list[4~5] are point
	 * to lib_bar.
	 */
	if ((algo_list != NULL) && algo_count) {
		dso = algo_list[0]->module->dso;
		for (uint32_t i = 0; i < algo_count; i++) {
			if ((algo_list[i]->module) && (algo_list[i]->module->dso)) {
				if (dso != algo_list[i]->module->dso) {
					dlclose(dso);
					dso = algo_list[i]->module->dso;
				}
			}
		}
		dlclose(dso);
		delete[] algo_list;
	}
}

const sensor_cal_algo_t* CalibrationManager::getCalAlgo(const sensor_t *s/* = NULL*/)
{
	uint32_t i = 0;
	int j = 0;
	const sensor_cal_algo_t **list = algo_list;
	const sensor_cal_algo_t *tmp = NULL;

	if (s == NULL) {
		ALOGW("No available algo found!");
		return NULL;
	}

	for (i = 0; i < algo_count; i++) {
		if ((list[i]->type != s->type) || check_algo(list[i]))
			continue;
		j = 0;
		while (list[i]->compatible[j] != NULL) {
			if (strcmp(list[i]->compatible[j], s->name) == 0)
				break;
			if (strcmp(list[i]->compatible[j], type_to_name(s->type)) == 0)
				tmp = list[i];
			j++;
		}

		/* Exactly compatible */
		if (list[i]->compatible[j] != NULL)
			break;
	}

	if (i != algo_count) {
		ALOGI("found exactly compatible algo for type %d", s->type);
		return list[i];
	}

	if (tmp != NULL)
		ALOGI("found compatible algo for type %d", s->type);

	return tmp;
}

void CalibrationManager::dump()
{
	int j;
	uint32_t i;

	ALOGI("algo_count:%d\n", algo_count);
	for (i = 0; i < algo_count; i++) {
		ALOGI("\ntag:%d\tversion:%d\ttype:%d\n", algo_list[i]->tag, algo_list[i]->version, algo_list[i]->type);
		j = 0;
		while (algo_list[i]->compatible[j] != NULL) {
			ALOGI("compatible:%s\n", algo_list[i]->compatible[j++]);
		}
	}
}

/*
   Copyright (c) 2013, The Linux Foundation. All rights reserved.

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
*/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <string.h>

#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
#include "util.h"

#define DPP_PARTITION "/dev/block/mmcblk0p1"
#define DPP_MOUNTPOINT "/dpp"
#define DPP_FS "vfat"
#define DPP_FLAGS MS_RDONLY|MS_NOATIME|MS_NODEV|MS_NODIRATIME|MS_NOEXEC|MS_NOSUID
#define DPP_DATA "shortname=lower,uid=1000,gid=1000,dmask=227,fmask=337,context=u:object_r:firmware_file:s0"
#define PRODUCT_DAT "/dpp/Nokia/product.dat"

void ds_properties();

void ds_properties()
{
    property_set("persist.radio.multisim.config", "dsds");
    property_set("persist.radio.dont_use_dsd", "true");
}

void vendor_load_properties()
{
    int ret = 0, rdonly = 0;
    FILE *fp;
    char device[PROP_VALUE_MAX];
    char fingerprint[PROP_VALUE_MAX];
    char modelnumber[32];

    if (access(DPP_MOUNTPOINT, F_OK) == -1) {
        ERROR("DPP mount point not found, creating it\n");
        ret = make_dir(DPP_MOUNTPOINT, 0400);
        if (ret) {
            ERROR("cannot create DPP mount point, trying to remount rootfs as RW\n");
            if (mount("", "/", "", MS_REMOUNT, NULL) == 0) {
                ERROR("remount done\n");
                rdonly = 1;
                ret = make_dir(DPP_MOUNTPOINT, 0400);
            }
        }
    }

    if (!ret) {
        ERROR("mounting DPP\n");
    if (mount(DPP_PARTITION, DPP_MOUNTPOINT, DPP_FS, DPP_FLAGS, DPP_DATA) == 0) {
            ERROR("DPP mounted\n");
            if (access(PRODUCT_DAT, R_OK) == 0) {
                if ((fp = fopen(PRODUCT_DAT, "r")) != NULL) {
                    if (fgets(modelnumber, sizeof(modelnumber), fp) != NULL) {
                        if (strcmp(modelnumber, "TYPE:RM-913\n") == 0) {
                            property_set("ro.product.device", "fame_cmcc");
                            property_set("ro.product.name",   "fame_cmcc");
                            property_set("ro.product.model",  "Lumia 520 (RM-913)");
                        }
                        if (strcmp(modelnumber, "TYPE:RM-914\n") == 0) {
                            property_set("ro.product.device", "fame");
                            property_set("ro.product.name",   "fame");
                            property_set("ro.product.model",  "Lumia 520 (RM-914)");
                        }
                        if (strcmp(modelnumber, "TYPE:RM-915\n") == 0) {
                            property_set("ro.product.device", "fame_lta");
                            property_set("ro.product.name",   "fame_lta");
                            property_set("ro.product.model",  "Lumia 520 (RM-915)");
                        }
                        if (strcmp(modelnumber, "TYPE:RM-917\n") == 0) {
                            property_set("ro.product.device", "fame_tmo");
                            property_set("ro.product.name",   "fame_tmo");
                            property_set("ro.product.model",  "Lumia 521 (RM-917)");
                        }
                        if (strcmp(modelnumber, "TYPE:RM-997\n") == 0) {
                            property_set("ro.product.device", "glee_cmcc");
                            property_set("ro.product.name",   "glee_cmcc");
                            property_set("ro.product.model",  "Lumia 526 (RM-997)");
                        }
                        if (strcmp(modelnumber, "TYPE:RM-998\n") == 0) {
                            property_set("ro.product.device", "glee");
                            property_set("ro.product.name",   "glee");
                            property_set("ro.product.model",  "Lumia 525 (RM-998)");
                        }
                    }
                    fclose(fp);
                }
            }
            else {
                ERROR("cannot access product.dat\n");
            }
        }
        else {
            ERROR("mounting DPP failed\n");
        }
    }

    if (rdonly == 1) {
        mount("", "/", "", MS_REMOUNT|MS_RDONLY, NULL);
    }

    property_set("ro.build.description", property_get("ro.build.fingerprint").c_str());

    ERROR("setting build properties for %s device\n", property_get("ro.product.device").c_str());
}

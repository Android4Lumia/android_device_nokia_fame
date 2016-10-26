/*
 * Copyright (C) 2014 The CyanogenMod Project
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

#define CAMERA_PARAMETERS_EXTRA_C \
const char CameraParameters::KEY_SONY_ISO[] = "sony-iso"; \
const char CameraParameters::KEY_SONY_METERING_MODE[] = "sony-metering-mode"; \
const char CameraParameters::KEY_SONY_VS[] = "sony-vs"; \
const char CameraParameters::KEY_SONY_AE_MODE[] = "sony-ae-mode"; \
const char CameraParameters::KEY_SONY_IS[] = "sony-is"; \
const char CameraParameters::KEY_GPS_LATITUDE_REF[] = "gps-latitude-ref"; \
const char CameraParameters::KEY_GPS_LONGITUDE_REF[] = "gps-longitude-ref"; \
const char CameraParameters::KEY_GPS_ALTITUDE_REF[] = "gps-altitude-ref";


#define CAMERA_PARAMETERS_EXTRA_H \
    static const char KEY_SONY_ISO[]; \
    static const char KEY_SONY_METERING_MODE[]; \
    static const char KEY_SONY_VS[]; \
    static const char KEY_SONY_AE_MODE[]; \
    static const char KEY_SONY_IS[]; \
    static const char KEY_GPS_LATITUDE_REF[]; \
    static const char KEY_GPS_LONGITUDE_REF[]; \
    static const char KEY_GPS_ALTITUDE_REF[];

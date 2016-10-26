AK8975/AK8963/AK09911 Sensor Daemon for Android.
=========
Compass Control Program for Android Open Source Project

## NOTICE
 1. Asahi Kasei Microdevices Corporation ("AKM") is pleased to make available to you the source codes of the Electronic Compass Control Program ("Software") for download from this website at no charge.   By downloading the Software, you expressly assume all risk and liability associated with downloading and using the Software.

 2. AKM PROVIDES THE SOFTWARE AND THIS DOWNLOADING SERVICE "AS IS" WITHOUT WARRANTY OF ANY KIND AND ALL SUCH WARRANTIES, EXPRESS OR IMPLIED, ARE HEREBY DISCLAIMED, INCLUDING, WITHOUT LIMITATION, WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE ACCURACY, AVAILABILITY, OR NON-INFRINGEMENT.  YOU ACKNOWLEDGE THAT AKM IS NOT OBLIGATED TO PROVIDE AND DOES NOT PROVIDE ANY SUPPORT OR ASSISTANCE WITH RESPECT TO THE SOFTWARE.

 3. This downloading service shall not affect nor extend any warranty or disclaimer which AKM makes in each of AKM's products in which the Software works with.

 4. This NOTICE shall be governed by and interpreted under the laws of Japan.  All disputes arising out of or under this NOTICE shall be subject to under the exclusive and agreed jurisdiction of the Tokyo District Court as the court of first instance.

## About
This software is developed by Asahi Kasei Microdevices Corporation ("AKM") to provide compatible daemon program, it is known as akmd2 in many production Android phones.

This software consists from two parts, one is Sensor Control Program (i.e. akmd2) and Sensor HAL (sensors.*.so).  Sensor Control Program gets magnetic sensor data form the AKM's compass IC, then estimate offset and calculate azimuth, pitch and roll angle. This behavior is quite similar to the original one. We aimed to develop a compatible daemon software. 

This software may help people who wants to study how to control AKM's magnetic sensor on Android.

## License
The license for all code in this source code is specified in the NOTICE file.  Please, refer to this file for further details.

## Build
To build this project, uncomment your desired device from the top of Android.mk file.

	vi $(YOUR_ENV)/AK8975_FS/Android.mk

For example, if you wish to have a software for AK8975, please uncomment the following line.

	export AKMD_DEVICE_TYPE=8975

Type 'mm -B', then you will get two binary files, **akmdfs** and **sensors.default.so**.


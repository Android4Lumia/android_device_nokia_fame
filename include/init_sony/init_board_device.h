/*
 * Copyright (C) 2016 The CyanogenMod Project
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

#ifndef __INIT_BOARD_DEVICE_H__
#define __INIT_BOARD_DEVICE_H__

#include "init_board_common.h"
#include "init_prototypes.h"

// Constants: devices controls
#define DEV_BLOCK_FOTA_NUM 23

// Class: init_board_device
class init_board_device : public init_board_common
{
public:
    // Board: introduction for keycheck
    virtual void introduce_keycheck()
    {
        // Short vibration
        vibrate(50);

        // LED boot selection colors
        led_color(255, 0, 255);
    }

    // Board: introduction for Android
    virtual void introduce_android()
    {
        // Power off LED
        led_color(0, 0, 0);
    }

    // Board: introduction for Recovery
    virtual void introduce_recovery()
    {
        // Align buffer parameter
        write_int("/sys/module/msm_fb/parameters/align_buffer", 0);

        // Short vibration
        vibrate(100);

        // LED Recovery colors
        led_color(0, 255, 255);
    }

    // Board: finish init execution
    virtual void finish_init()
    {
        // Power off vibrator
        vibrate(0);
    }

    // Board: set led colors
    void led_color(uint8_t r, uint8_t g, uint8_t b)
    {
        write_string("/sys/class/led/fih_led/control", r ? "01 0 1" : "01 0 0");
        write_string("/sys/class/led/fih_led/control", g ? "01 1 1" : "01 1 0");
        write_string("/sys/class/led/fih_led/control", b ? "01 2 1" : "01 2 0");
    }

    // Board: set hardware vibrator
    void vibrate(uint8_t strength)
    {
        write_int("/sys/class/timed_output/vibrator/enable", strength);
    }
};

#endif //__INIT_BOARD_DEVICE_H__

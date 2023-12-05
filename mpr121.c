/*
 * Copyright (c) Nekov0
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "mpr121.h"

void mpr121_init(i2c_inst_t *i2c_port, uint8_t i2c_addr,
                 mpr121_sensor_t *sensor) {
    sensor->i2c_port = i2c_port;
    sensor->i2c_addr = i2c_addr;
    
    // Soft reset
    mpr121_write(0x80, 0x63, sensor);

    //Touch pad baseline filter
    //Rising: Baseline quick rising

    mpr121_write(0x2B, 0x01, sensor);
    mpr121_write(0x2C, 0x01, sensor);
    mpr121_write(0x2D, 0x01, sensor);
    mpr121_write(0x2E, 0x01, sensor);

    //Slow falling

    mpr121_write(0x2F, 0x01,sensor);
    mpr121_write(0x30, 0x01, sensor);
    mpr121_write(0x31, 0x06, sensor);
    mpr121_write(0x32, 0x0C, sensor);

    // Verslow

    mpr121_write(0x33, 0x01, sensor);
    mpr121_write(0x34, 0x08, sensor);
    mpr121_write(0x35, 0x30, sensor);

    for (int i = 0; i < 12; i++) {
        mpr121_write(0x41 + i*2, MPR121_TOUCH_THRESHOLD_REG, sensor);
        mpr121_write(0x42 + i*2, MPR121_RELEASE_THRESHOLD_REG, sensor);
    }

    //debounce

    mpr121_write(0x5B, 0x00, sensor);

    //AFE and filter configuration

    mpr121_write(0x5C, 0b00010000, sensor);
    mpr121_write(0x5D, 0b00010000, sensor);
    mpr121_write(0x5E, 0x80, sensor);

    //auto

    mpr121_write(0x7B, 0b00001011, sensor);
    const uint8_t usl = (3.3 - 0.1) / 3.3 * 256;
    mpr121_write(0x7D, usl, sensor);
    mpr121_write(0x7E, usl*0.65, sensor);
    mpr121_write(0x7F, usl*0.9, sensor);

    mpr121_write(0x5E, 0x8C, sensor);
}
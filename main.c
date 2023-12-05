/*
 * Copyright (c) 2021-2023 Antonio González
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* MPR121 hello_mpr121 example.

When a sensor (electrode) is touched, the on-board LED lights on and the
sensor number is printed.
*/

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpr121.h"
#include "hardware/adc.h"
#include "vl53l0x.h"
#include "config.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "rgb.h"

// I2C definitions: port and pin numbers
#define I2C_PORT i2c0
#define I2C_SDA 20
#define I2C_SCL 21

// I2C definitions: address and frequency
#define MPR121_ADDR 0x5D
#define MPR121_I2C_FREQ 100000

// Touch and release thresholds
#define MPR121_TOUCH_THRESHOLD 5
#define MPR121_RELEASE_THRESHOLD 4

static struct
{
    uint8_t stop_variable; // read by init and used when starting measurement
    uint16_t range;
    uint32_t timing_budget_us;
} instances[16];

static uint16_t distances[5];

static const uint8_t TOF_LIST[] = {1, 2, 0, 4, 5};

static inline uint8_t air_bits(int dist, int offset)
{
    if ((dist < offset) || (dist == 4095))
    {
        return 0;
    }

    int pitch = chu_cfg->tof.pitch * 10;
    int index = (dist - offset) / pitch;
    if (index >= 6)
    {
        return 0;
    }

    return 1 << index;
}

uint8_t air_bitmap()
{
    int offset = chu_cfg->tof.offset * 10;
    int pitch = chu_cfg->tof.pitch * 10;
    uint8_t bitmap = 0;
    bitmap |= air_bits(distances[1], offset) |
              air_bits(distances[1], offset + pitch);
    return bitmap;
}

unsigned air_value(uint8_t index)
{
    if (index >= sizeof(TOF_LIST))
    {
        return 0;
    }
    int offset = chu_cfg->tof.offset * 10;
    int pitch = chu_cfg->tof.pitch * 10;
    uint8_t bitmap = air_bits(distances[index], offset) |
                     air_bits(distances[index], offset + pitch);

    for (int i = 0; i < 6; i++)
    {
        if (bitmap & (1 << i))
        {
            return i + 1; // lowest detected position
        }
    }

    return 0;
}

void air_update()
{
    distances[1] = readRangeContinuousMillimeters(1) * 10;
}

uint16_t air_raw(uint8_t index)
{
    if (index >= 5)
    {
        return 0;
    }
    return distances[index];
}

int main()
{
    stdio_init_all();
    // Setup the on-board LED

    // const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    // gpio_init(LED_PIN);
    // gpio_set_dir(LED_PIN, GPIO_OUT);

    // Initialise I2C
    i2c_init(I2C_PORT, MPR121_I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // uint8_t enable01 = 0x01;

    // i2c_write_blocking(i2c0, 0x70, &enable01, 1, false);
    // Initialise and configure the touch sensor
    struct mpr121_sensor mpr121;
    mpr121_init(I2C_PORT, MPR121_ADDR, &mpr121);
    mpr121_set_thresholds(MPR121_TOUCH_THRESHOLD,
                          MPR121_RELEASE_THRESHOLD, &mpr121);
    mpr121_init(I2C_PORT, 0x5A, &mpr121);
    // Initialise data variables
    uint16_t touched;
    mpr121_set_debounce(MPR121_TOUCH_THRESHOLD, MPR121_RELEASE_THRESHOLD, &mpr121);
    static uint16_t allE[12], oldE[12];
    while (1)
    {

        for (int i = 0; i < 12; i++)
        {
            oldE[i] = allE[i];
            mpr121_baseline_value(i, &allE[i], &mpr121);
            //printf("%d ", allE[i]);
        }
        // Bits 11-0 in the value returned by this function are flags
        // set when the electrodes are touched
        mpr121_touched(&touched, &mpr121);
        // If a touch is detected, light on the LED and print a pattern
        // representing the electrodes (0 to 11) where 1 means touched.
        // gpio_put(LED_PIN, 1);
        for (uint8_t electrode = 0; electrode < 12; electrode++)
        {
            bool tmp = ((touched >> electrode) & 1) ? true : false;
            // if(!tmp) {
            //     if( (allE[electrode] == oldE [electrode]) && (allE[electrode] < 800)) tmp = true;
            // } 
            if(tmp) printf("%c ", '1');
            else printf("%c ", '.');
        }

        printf("\n");
        sleep_ms(50);
    }

    // i2c_init(I2C_PORT, 733*1000);
    // gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA);
    // gpio_pull_up(I2C_SCL);

    // vl53l0x_init(I2C_PORT, 0);

    // uint8_t enable01 = 0x01;

    // i2c_write_blocking(i2c0, 0x70, &enable01, 1, false);

    // vl53l0x_init_tof(1);
    // vl53l0x_start_continuous(1);

    // while(1) {
    //     air_update();
    //     printf("%d\n", air_raw(1));
    //     sleep_ms(100);
    // }
    // return 0;
}

// #define IS_RGBW true
// #define NUM_PIXELS 150

// #ifdef PICO_DEFAULT_WS2812_PIN
// #define WS2812_PIN PICO_DEFAULT_WS2812_PIN
// #else
// // default to pin 2 if the board doesn't have a default WS2812 pin defined
// #define WS2812_PIN 2
// #endif

// static inline void put_pixel(uint32_t pixel_grb) {
//     pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
// }

// static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
//     return
//             ((uint32_t) (r) << 8) |
//             ((uint32_t) (g) << 16) |
//             (uint32_t) (b);
// }

// void pattern_snakes(uint len, uint t) {
//     for (uint i = 0; i < len; ++i) {
//         uint x = (i + (t >> 1)) % 64;
//         if (x < 10)
//             put_pixel(urgb_u32(0xff, 0, 0));
//         else if (x >= 15 && x < 25)
//             put_pixel(urgb_u32(0, 0xff, 0));
//         else if (x >= 30 && x < 40)
//             put_pixel(urgb_u32(0, 0, 0xff));
//         else
//             put_pixel(0);
//     }
// }

// void pattern_random(uint len, uint t) {
//     if (t % 8)
//         return;
//     for (int i = 0; i < len; ++i)
//         put_pixel(rand());
// }

// void pattern_sparkle(uint len, uint t) {
//     if (t % 8)
//         return;
//     for (int i = 0; i < len; ++i)
//         put_pixel(rand() % 16 ? 0 : 0xffffffff);
// }

// void pattern_greys(uint len, uint t) {
//     int max = 100; // let's not draw too much current!
//     t %= max;
//     for (int i = 0; i < len; ++i) {
//         put_pixel(t * 0x10101);
//         if (++t >= max) t = 0;
//     }
// }

// typedef void (*pattern)(uint len, uint t);
// const struct {
//     pattern pat;
//     const char *name;
// } pattern_table[] = {
//         {pattern_snakes,  "Snakes!"},
//         {pattern_random,  "Random data"},
//         {pattern_sparkle, "Sparkles"},
//         {pattern_greys,   "Greys"},
// };

// int main() {
//     //set_sys_clock_48();
//     stdio_init_all();
//     printf("WS2812 Smoke Test, using pin %d", WS2812_PIN);

//     // todo get free sm
//     PIO pio = pio0;
//     int sm = 0;
//     uint offset = pio_add_program(pio, &ws2812_program);

//     ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

//     int t = 0;
//     while (1) {
//         int pat = rand() % count_of(pattern_table);
//           int dir = (rand() >> 30) & 1 ? 1 : -1;
//         puts(pattern_table[pat].name);
//         puts(dir == 1 ? "(forward)" : "(backward)");
//         for (int i = 0; i < 1000; ++i) {
//             pattern_table[pat].pat(NUM_PIXELS, t);
//             sleep_ms(10);
//             t += dir;
//         }
//     }
// }
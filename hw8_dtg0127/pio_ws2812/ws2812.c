/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/pwm.h"

#define ServoPin 16 // the built in LED on the Pico

void servo_pwm_setup(void);
void setServoAngle(float angle);

uint16_t wrap;
uint slice_num;

/**
 * NOTE:
 *  Take into consideration if your WS2812 is a RGB or RGBW variant.
 *
 *  If it is RGBW, you need to set IS_RGBW to true and provide 4 bytes per 
 *  pixel (Red, Green, Blue, White) and use urgbw_u32().
 *
 *  If it is RGB, set IS_RGBW to false and provide 3 bytes per pixel (Red,
 *  Green, Blue) and use urgb_u32().
 *
 *  When RGBW is used with urgb_u32(), the White channel will be ignored (off).
 *
 */
#define IS_RGBW false
#define NUM_PIXELS 4
#define DURATION_MS 5000 // 5 seconds
#define FRAME_TIME_MS 20 // update every 20ms (50 FPS)

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 2
#endif

// Check the pin is compatible with the platform
#if WS2812_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

// link three 8bit colors together
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor;

wsColor HSBtoRGB(float hue, float sat, float brightness);

float wrapHue(float hue);

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 16) |
            ((uint32_t) (g) << 8) |
            (uint32_t) (b);
}

static inline uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            ((uint32_t) (w) << 24) |
            (uint32_t) (b);
}

void pattern_snakes(PIO pio, uint sm, uint len, uint t) {
    for (uint i = 0; i < len; ++i) {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            put_pixel(pio, sm, urgb_u32(0xff, 0, 0));
        else if (x >= 15 && x < 25)
            put_pixel(pio, sm, urgb_u32(0, 0xff, 0));
        else if (x >= 30 && x < 40)
            put_pixel(pio, sm, urgb_u32(0, 0, 0xff));
        else
            put_pixel(pio, sm, 0);
    }
}

void pattern_random(PIO pio, uint sm, uint len, uint t) {
    if (t % 8)
        return;
    for (uint i = 0; i < len; ++i)
        put_pixel(pio, sm, rand());
}

void pattern_sparkle(PIO pio, uint sm, uint len, uint t) {
    if (t % 8)
        return;
    for (uint i = 0; i < len; ++i)
        put_pixel(pio, sm, rand() % 16 ? 0 : 0xffffffff);
}

void pattern_greys(PIO pio, uint sm, uint len, uint t) {
    uint max = 100; // let's not draw too much current!
    t %= max;
    for (uint i = 0; i < len; ++i) {
        put_pixel(pio, sm, t * 0x10101);
        if (++t >= max) t = 0;
    }
}

typedef void (*pattern)(PIO pio, uint sm, uint len, uint t);
const struct {
    pattern pat;
    const char *name;
} pattern_table[] = {
        {pattern_snakes,  "Snakes!"},
        {pattern_random,  "Random data"},
        {pattern_sparkle, "Sparkles"},
        {pattern_greys,   "Greys"},
};

int main() {
    //set_sys_clock_48();
    stdio_init_all();
    stdio_init_all();
    servo_pwm_setup();

    printf("WS2812 Smoke Test, using pin %d\n", WS2812_PIN);

    // todo get free sm
    PIO pio;
    uint sm;
    uint offset;

    // This will find a free pio and state machine for our program and load it for us
    // We use pio_claim_free_sm_and_add_program_for_gpio_range (for_gpio_range variant)
    // so we will get a PIO instance suitable for addressing gpios >= 32 if needed and supported by the hardware
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
    hard_assert(success);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int t = 0;

    while (1) {
        int i;
        float angle;

        int total_steps = DURATION_MS / FRAME_TIME_MS;
        for (int step = 0; step < total_steps; step++) {
            float base_hue = (360.0f * step) / total_steps; // progress from 0° to 360°

            for (int i = 0; i < NUM_PIXELS; i++) {
                float pixel_hue = wrapHue(base_hue + (i * (360.0f / NUM_PIXELS))); // offset for each pixel
                wsColor c = HSBtoRGB(pixel_hue, 1.0f, 1.0f); // full saturation, 50% brightness
                put_pixel(pio, sm, urgb_u32(c.r, c.g, c.b));
                
                if (pixel_hue<=180){
                    angle = pixel_hue;
                }
                else if (pixel_hue>180){
                    angle = pixel_hue-180;
                }

                setServoAngle(angle);
        }

        sleep_ms(FRAME_TIME_MS); // small delay between frames
    }

        // for (int j=0;j<7;j++){
        //     int hue_val = getColorHue(rainbow[j]);
        //     wsColor c_hsb = HSBtoRGB(hue_val,1,0.05);

        //     for(i=0;i<NUM_PIXELS;i++){
        //         put_pixel(pio, sm, urgb_u32(c_hsb.r, c_hsb.g, c_hsb.b));
        //     }

        //     sleep_ms(500);
        // }
        // sleep_ms(1); // wait at least the reset time
    }

    // This will free resources and unload our program
    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);
}


// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}

float wrapHue(float hue) {
    while (hue >= 360.0f) hue -= 360.0f;
    while (hue < 0.0f) hue += 360.0f;
    return hue;
}

void servo_pwm_setup(void){
    gpio_init(ServoPin);
    gpio_set_function(ServoPin, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(ServoPin); // Get PWM slice number
    float div = 50.0; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    wrap = 60000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
}

void setServoAngle(float angle){
    float duty_cycle = 0.025 + ((0.125-0.025)/(180-0))*(angle-0);  
    pwm_set_gpio_level(ServoPin, wrap * duty_cycle); // set the duty cycle to 50%
    pwm_set_enabled(slice_num, true);
}

/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define FLAG_VALUE 123
#define LEDPin 15
#define ADCPin 26

void setup_LED(void);
void adc_initialize(void);

void core1_entry() {

    multicore_fifo_push_blocking(FLAG_VALUE);

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 1!\n");
    else
        printf("Its all gone well on core 1!\r\n");

    while (1)
        tight_loop_contents();
}

int main() {

    stdio_init_all();
    setup_LED();
    adc_initialize();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("Hello, multicore!\n");

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // Wait for it to start up

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 0!\n");
    else {
        multicore_fifo_push_blocking(FLAG_VALUE);
        printf("It's all gone well on core 0!\r\n");
    }

    /// \end::setup_multicore[]

    while (1){
        //tight_loop_contents();
        int num;
        printf("What would you like to do:\n\t0: Read voltage\n\t1:Turn on LED\n\t2: Turn off LED\n");
        scanf("%i\n",&num);
        printf("You have entered %i", num);
        // uint16_t result = adc_read();
        // float volts = 3.3 / 4095 * result;
        // printf("%.2f\r\n",volts);
    }
}

void setup_LED(void){
    gpio_init(LEDPin); // initialize the LED pin
    gpio_set_dir(LEDPin,true); // set the LED pin to output

    gpio_put(LEDPin,true);
    sleep_ms(500);
    gpio_put(LEDPin,false);
}

void adc_initialize(void){
    adc_init(); // init the adc module
    adc_gpio_init(ADCPin); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0
}

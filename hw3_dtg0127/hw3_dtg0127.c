#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define BUTTON_PIN_NUM 2 // button pin
#define LED_PIN_NUM 13 // led pin

void pin_initialize(void){

    gpio_init(BUTTON_PIN_NUM); // initialize the button pin
    gpio_init(LED_PIN_NUM); // initialize the led pin

    gpio_set_dir(BUTTON_PIN_NUM, GPIO_IN); // set the button pin to input
    gpio_set_dir(LED_PIN_NUM, GPIO_OUT); // set the button pin to input
    
    gpio_put(13,1); // initialize the led off
}

void adc_initialize(void){
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0
}

int main() {
    stdio_init_all();
    pin_initialize(); // initialize the button and led pin
    adc_initialize(); // initialize the adc

    while (!stdio_usb_connected()) { // wait until the USB port has been opened
        sleep_ms(100);
    }
    printf("Start!\n");

    gpio_put(13,0); // turn on the LED

    while (gpio_get(2)){ // wait until the button is pressed
        sleep_ms(100);
    }

    gpio_put(13,1); // turn off the LED
    
    while (1){
        printf("Enter a number of analog samples to take, between 1 and 100: \r\n"); // ask the user input
        
        int num_samples;
        scanf("%d", &num_samples); // read the number entered by the user
        printf("The number of analog samples requested: %i\r\n", num_samples);
        sleep_ms(50);

        for (int i=0; i < num_samples; i++){
            uint16_t result = adc_read();
            float volts = 3.3 / 4095 * result;
            printf("%i. %.1f V\r\n", i,volts);
            sleep_ms(10); 
        }

        /*if (gpio_get(2) == 0){
            gpio_put(13,0);
            uint16_t result = adc_read();
            printf("%i\r\n",result);
            sleep_ms(50);   
        }
        else if (gpio_get(2) == 1) {
            gpio_put(13,1);
        }

        char message[100];
        scanf("%s", message);
        printf("message: %s\r\n",message);
        sleep_ms(50);*/
    }
}

#include <stdio.h>
#include "pico/stdlib.h"

#define BUTTON_PIN_NUM 2 // button pin
#define LED_PIN_NUM 13 // led pin

void pin_initialize(void){

    gpio_init(BUTTON_PIN_NUM); // initialize the button pin
    gpio_init(LED_PIN_NUM); // initialize the led pin

    gpio_set_dir(BUTTON_PIN_NUM, GPIO_IN); // set the button pin to input
    gpio_set_dir(LED_PIN_NUM, GPIO_OUT); // set the button pin to input

    gpio_put(13, 0);
}

int main() {
    stdio_init_all();

    pin_initialize();
    
    while (1){
        if (gpio_get(2) == 0){
            gpio_put(13,0);
        }
        else if (gpio_get(2) == 1) {
            gpio_put(13,1);
        }
    }
}

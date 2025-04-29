#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define ServoPin 16 // the built in LED on the Pico

void servo_pwm_setup(void);
void setServoAngle(float angle);

uint16_t wrap;
uint slice_num;

int main()
{
    stdio_init_all();
    servo_pwm_setup();
    float angles[6];

    angles[0] = 0.0;
    angles[1] = 180.0;
    angles[2] = 0.0;
    angles[3] = 180.0;
    angles[4] = 0.0;
    angles[5] = 180.0;
    
    while (true) {
        for (int i=0;i<=1;i++){
            setServoAngle(angles[i]);
            sleep_ms(1000);
        }
    }

    return 0;
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

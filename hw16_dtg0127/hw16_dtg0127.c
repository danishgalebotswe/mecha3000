#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PWM_B_PH 20
#define PWM_B_EN 21

void pwm_pin_setup(void);
void setMotor(int dutyCycle);

uint16_t wrap = 60000;
int duty = 0;
uint slice_num;

int main()
{
    stdio_init_all();

    pwm_pin_setup();

    printf("Use '+' to increase and '-' to decrease duty cycle (-100 to 100)\n");

    while (true) {
        int c = getchar_timeout_us(0);
        if (c == '+') {
            if (duty < 100) duty++;
            setMotor(duty);
            printf("Duty: %d\n", duty);
        } else if (c == '-') {
            if (duty > -100) duty--;
            setMotor(duty);
            printf("Duty: %d\n", duty);
        }
        sleep_ms(10);
    }
}


void pwm_pin_setup(void) {
    // gpio_init(PWM_B_EN);
    gpio_init(PWM_B_PH);

    // gpio_set_dir(PWM_A_EN, true);
    gpio_set_dir(PWM_B_PH, true);
    gpio_put(PWM_B_PH, 0);

    gpio_set_function(PWM_B_EN, GPIO_FUNC_PWM);
    // gpio_set_function(PWM_B_PH, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to PH (it's slice 0)
    slice_num = pwm_gpio_to_slice_num(PWM_B_EN);

    
    pwm_set_wrap(slice_num, wrap);
    float div = 50.0; // must be between 1-255
    pwm_set_clkdiv(slice_num, div);
    pwm_set_enabled(slice_num, true);
}

void setMotor(int dutyCycle) {

    // Clamp value and calculate PWM level
    if (dutyCycle > 100) dutyCycle = 100;
    if (dutyCycle < -100) dutyCycle = -100;

    uint16_t level = (uint16_t)((abs(dutyCycle) / 100.0) * wrap);

    if (dutyCycle > 0) {
        // pwm_set_chan_level(slice_num, PWM_CHAN_A,);        // PH = 0
        // pwm_set_chan_level(slice_num, PWM_CHAN_B, level);    // EN = PWM

        gpio_put(PWM_B_PH, 0);  // Forward
        pwm_set_chan_level(slice_num, pwm_gpio_to_channel(PWM_B_EN), level);

    } else if (dutyCycle < 0) {
        gpio_put(PWM_B_PH, 1);  // Reverse
        pwm_set_chan_level(slice_num, pwm_gpio_to_channel(PWM_B_EN), 100-level);

        // pwm_set_chan_level(slice_num, PWM_CHAN_A, level);    // PH = PWM
        // pwm_set_chan_level(slice_num, PWM_CHAN_B, 0);        // EN = 0
    } else {
        pwm_set_chan_level(slice_num, pwm_gpio_to_channel(PWM_B_EN), 0);        // Both = 0
        // pwm_set_chan_level(slice_num, PWM_CHAN_B, 0);
    }
}





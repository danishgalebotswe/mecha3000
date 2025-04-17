#include <stdio.h>
#include "pico/stdlib.h"

#define PICO_FREQ 150000000
#define LOOP_NUM 1000


int main()
{
    stdio_init_all();

    while (true) {

        while (!stdio_usb_connected()) { // wait until the USB port has been opened
            sleep_ms(100);
        }
        printf("Start!\n\r\n");

        volatile float f1, f2;
        volatile absolute_time_t t1, t2;
        volatile uint64_t t_op;
        volatile float cpu_cycles;


        printf("Enter two floats to use: \r\n");
        scanf("%f %f", &f1, &f2);
        volatile float f_add, f_sub, f_mult, f_div;

        t1 = get_absolute_time();
        for (int i = 1; i <= 1000; i++) {
            f_add = f1+f2;
        }
        t2 = get_absolute_time();
        t_op = to_us_since_boot(t2-t1);
        cpu_cycles = (t_op * 1e-6 * PICO_FREQ) / LOOP_NUM;
        printf("Addition CPU Clock Cycles: %f\r\n", cpu_cycles);
        
        t1 = get_absolute_time();
        for (int i = 1; i <= 1000; i++) {
            f_sub = f1-f2;
        }
        t2 = get_absolute_time();
        t_op = to_us_since_boot(t2) - to_us_since_boot(t1);
        cpu_cycles = (t_op * 1e-6 * PICO_FREQ)/ LOOP_NUM;
        printf("Subtraction CPU Clock Cycles: %f\r\n", cpu_cycles);
        
        t1 = get_absolute_time();
        for (int i = 1; i <= 1000; i++) {
            f_mult = f1*f2;
        }
        t2 = get_absolute_time();
        t_op = to_us_since_boot(t2-t1);
        cpu_cycles = (t_op * 1e-6 * PICO_FREQ) / LOOP_NUM;
        printf("Multiplication CPU Clock Cycles: %f\r\n", cpu_cycles);

        t1 = get_absolute_time();
        for (int i = 1; i <= 1000; i++) {
            f_div = f1/f2;
        }
        t2 = get_absolute_time();
        t_op = to_us_since_boot(t2-t1);
        cpu_cycles = (t_op * 1e-6 * PICO_FREQ) / LOOP_NUM;
        printf("Division CPU Clock Cycles: %f\r\n\r\n", cpu_cycles);
        
        printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);

        //printf("t = %llu\n", t);
    }
}

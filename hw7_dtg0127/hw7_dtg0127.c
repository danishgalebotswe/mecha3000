#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
#define LED_PIN 25

void drawMessage(int x, int y, char *m);
void drawLetter(int x, int y, char c);


int main()
{
    stdio_init_all();

    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();


    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA);
    // gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c


    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN,0);
    
    while (true) {
        gpio_put(LED_PIN,1);
        char message[50];
        sprintf(message,"hello");
        drawMessage(3,12,message);
        ssd1306_update();
        // printf("Hello, world!\n");
        sleep_ms(1000);
        gpio_put(LED_PIN,0);
    }
}

void drawMessage(int x, int y, char *m){
    int i = 0;
    while (m[i] != 0){
        drawLetter(x+i*5,y,m[i]);
        i++;
    }
}

void drawLetter(int x, int y, char c){
    int row, col;
    row = c - 0x20;
    col = 0;
    for (col=0; col<5;col++){
        char byte = ASCII[row][col];

        for (int i=0;i<8;i++){
            char on_or_off = (byte>>i)&0b1;

            ssd1306_drawPixel(x+col,y+i,on_or_off);

        }
    }
}

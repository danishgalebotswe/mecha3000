#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

// define address for the LED and Button
#define ADDR 0x20 // 0100, 000 for A2-0 wired low, 0 for write 
#define OLAT_ADDR 0x0A // 0x0A address for Latch SFR
#define IODIR_ADDR 0x00 // 0x00 address for I/O direction register
#define GPIO_ADDR 0x09 // 0x09 GPIO Port address


void i2c_pin_init(void); // function to initialize all the pins for i2c on the Pico and the external chip
void setLED(unsigned char address, unsigned char reg, unsigned char value);
unsigned int readPIN(void); 


int main()
{
    stdio_init_all();

    i2c_pin_init();

    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    while (true) {
        // printf("%x\r\n",data[0]<<7);
        // printf("Hello, world!\n");
        unsigned int buttonPressed = readPIN();

        if (buttonPressed == 1){
            setLED(ADDR,OLAT_ADDR,1);
        }
        if (buttonPressed == 0){
            setLED(ADDR,OLAT_ADDR,0);
        }

        // sleep_ms(1000);
    }
}


void i2c_pin_init(void){
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(I2C_SDA); // using physical pull up resistors
    //gpio_pull_up(I2C_SCL);

    uint8_t buf[2];

    buf[0] = 0x00; // IODIR address
    buf[1] = 0b01111111; // Set GP7 to output & GP0 to input as well

    //i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false);

    // check if LED is on
    // buf[0] = 0x0A; // OLAT address
    // buf[1] = 0b10000000; // Set GP7 to high, switch on the LED

    // i2c_write_blocking(I2C_PORT, ADDR, buf,2,false);

    setLED(ADDR,OLAT_ADDR,1); // switch on the LED

    sleep_ms(2000);

    setLED(ADDR,OLAT_ADDR,0);

    //buf[1] = 0x00;

    //i2c_write_blocking(I2C_PORT, ADDR, buf,2,false);

}

void setLED(unsigned char address, unsigned char reg, unsigned char value){
    uint8_t buf[2];

    buf[0] = reg;

    if (value==1){
        buf[1] = 0b10000000; // Set GP7 to high, switch on the LED
    }

    if (value==0){
        buf[1] = 0x00; // Set GP7 to low, switch off the LED
    }

    i2c_write_blocking(I2C_PORT, address, buf, 2, false);
}

unsigned int readPIN(void){
    uint8_t data[1];
    uint8_t reg = GPIO_ADDR; //temp variable to hold the address

    // reading the all the GPIO pins to see which ones are high or low
    i2c_write_blocking(I2C_PORT,ADDR,&reg,1,true);
    i2c_read_blocking(I2C_PORT,ADDR,data,1,false);

    if ((data[0] & 0b1) == 0){
        return 1;
    }
    else {
        return 0;
    }
}



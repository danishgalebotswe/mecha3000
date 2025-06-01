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
#define OLED_I2C_PORT i2c1
#define OLED_I2C_SDA 14
#define OLED_I2C_SCL 15

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75
#define IMU_ADDR     0x68

#define LED_PIN      25

void imu_init(void);
uint8_t imu_read(uint8_t msg);
void oled_init(void);
void drawMessage(int x, int y, char *m);
void drawLetter(int x, int y, char c);

int main()
{
    stdio_init_all();

    // while (!stdio_usb_connected()) {
    //     sleep_ms(100);
    // }
    printf("Initializing IMU!\n");

    imu_init();
    oled_init();

    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();
    

    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    while (true) {

        // read the high and low bytes
        uint8_t xh = imu_read(ACCEL_XOUT_H);
        uint8_t xl = imu_read(ACCEL_XOUT_L);
        uint8_t yh = imu_read(ACCEL_YOUT_H);
        uint8_t yl = imu_read(ACCEL_YOUT_L);

        // combine to 16 bit numbers
        int16_t x_raw = (int16_t)((xh<<8) | xl);
        int16_t y_raw = (int16_t)((yh<<8) | yl);

        // convert to acceleration in g's
        float x_g = x_raw / 16384.0f;
        float y_g = y_raw / 16384.0f;

        printf("X-ACCEL = %.3f, Y-ACCEL = %.3f\n", x_g, y_g);

        char message[50];
        sprintf(message,"X-ACCEL = %.3f\n", x_g);
        drawMessage(3,12,message);
        ssd1306_update();

        sprintf(message,"Y-ACCEL = %.3f\n", y_g);
        drawMessage(3,21,message);
        ssd1306_update();

        sleep_ms(1000);
    }
}

void imu_init(void){
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA);
    // gpio_pull_up(I2C_SCL);


    // set up the LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); 

    // turn on the chip
    uint8_t buf[2];
    buf[0] = PWR_MGMT_1;
    buf[1] = 0x00;
    i2c_write_blocking(I2C_PORT, IMU_ADDR, buf, 2, false);

    // enable the accelerometer
    buf[1] = ACCEL_CONFIG;
    i2c_write_blocking(I2C_PORT, IMU_ADDR, buf, 2,false);

    // check the i2c
    uint8_t whoami = imu_read(WHO_AM_I);
    printf("0x%02x\r\n",whoami);


    gpio_put(LED_PIN, 1);
    printf("IMU initialized!\n");
}

uint8_t imu_read(uint8_t msg) {
    uint8_t reg[2];
    reg[0] = msg;
    reg[1] = 0x00;
    uint8_t val;
    i2c_write_blocking(I2C_PORT,IMU_ADDR, reg, 1, true); // send the reg address
    i2c_read_blocking(I2C_PORT, IMU_ADDR, &val, 1, false); // read the result
    return val;
}

void oled_init(void) {
    i2c_init(OLED_I2C_PORT, 400*1000);
    gpio_set_function(OLED_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(OLED_I2C_SCL, GPIO_FUNC_I2C);
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

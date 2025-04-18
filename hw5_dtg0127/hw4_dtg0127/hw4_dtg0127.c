#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#define SPI_PORT spi0
#define SCK_PIN_NUM 6
#define SDI_PIN_NUM 7
#define CS_PIN_NUM 9

#define SCK_PIN_RAM 18
#define SDI_PIN_RAM 19
#define CS_PIN_RAM 17
#define SDO_PIN_RAM 16

#define PI 3.142
#define FREQ 1

void spi_initialize(void);
static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);
void writeDAC(int channel, float volts);
void spi_ram_init(void);
void spi_ram_write(uint16_t addr, float v);
float spi_ram_read(uint16_t addr);

int main()
{
    stdio_init_all();

    // while (!stdio_usb_connected()) { // wait until the USB port has been opened
    //     sleep_ms(100);
    // }
    // printf("Start!\n\r\n");

    spi_initialize();
    spi_ram_init();

    float t=0;

    // float v_new,v = 1.23;


    // spi_ram_write(0x4, v);

    // v_new = spi_ram_read(0x4);

    // printf("%.2f\r\n", v_new);

    uint16_t address = 0;


    // load 1000 floats into external RAM  representing a single cycle of a sine wave from 0V to 3.3V.

    for (int i=0;i<1000;i++){
        float voltage = 1.65*sin(FREQ*t*2.0*PI) + 1.65;
        // float v = fabs(3.3*(fmod(2*FREQ*t,2)-1));
        t=t+0.002;
        spi_ram_write(address, voltage);
        address += 4;
    }

    // check the first 20 values

    address = 0;
    float v_check;

    // for (int i=0;i<20;i++){
    //     // float v = fabs(3.3*(fmod(2*FREQ*t,2)-1));
    //     v_check = spi_ram_read(address);
    //     address += 4;
    //     printf("%.2f\r\n", v_check);
    //     sleep_ms(250);
    // }


    while (1){

        for (int i=0;i<1000;i++){
            v_check = spi_ram_read(address);
            writeDAC(0,v_check);
            address += 4;
            sleep_ms(2);
        }
        printf("Data written!\r\n");
        address = 0;
        sleep_ms(0.01);
    }
}

void spi_initialize(void){
    spi_init(SPI_PORT,1000*1000); // set the SPI 0 at 12 kHz and connect to GPIOs
    gpio_set_function(SCK_PIN_NUM, GPIO_FUNC_SPI); // set the SCK to GP6 (pin 9)
    gpio_set_function(SDI_PIN_NUM, GPIO_FUNC_SPI); // set the SDI to GP7 (pin 10) 

    gpio_init(CS_PIN_NUM); // set CS to GP9 (pin 12)
    gpio_set_dir(CS_PIN_NUM, GPIO_OUT); // set GP9 to be output pin for chip select
    gpio_put(CS_PIN_NUM,1); // initialize GP9 to high

    gpio_set_function(SCK_PIN_RAM, GPIO_FUNC_SPI); // set the SCK to GP18 (pin 24)
    gpio_set_function(SDI_PIN_RAM, GPIO_FUNC_SPI); // set the SDI to GP19 (pin 25) 
    gpio_set_function(SDO_PIN_RAM, GPIO_FUNC_SPI); // set the SD16 to GP19 (pin 25) 

    gpio_init(CS_PIN_RAM); // set CS to GP17 (pin 22)
    gpio_set_dir(CS_PIN_RAM, GPIO_OUT); // set GP17 to be output pin for chip select
    gpio_put(CS_PIN_RAM,1); // initialize GP17 to high


}

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void writeDAC(int channel, float volts){
    uint8_t data[2];
    int len = 2;

    if (volts < 0){
        volts = 0;
    }

    if (volts > 3.3){
        volts = 3.3;
    }

    uint16_t v_bin = (uint16_t)((volts / 3.3) * 1023.0);
    
    data[0] = 0;
    data[1] = 0;
    data[0] = data[0] | (channel<<7);
    data[0] = data[0] | (0b111<<4);
    data[0] = data[0] | (v_bin>>6);
    data[1] = data[1] | (v_bin << 2);

    //data[0] = 0b01111000;
    //data[1] = 0b00000000;

    cs_select(CS_PIN_NUM);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(CS_PIN_NUM);
}

void spi_ram_init(void){
    uint8_t buf[2];
    uint8_t buf2[2];
    uint8_t buf3[2];

    buf[0] = 0b1; // write STATUS register
    buf[1] = 0b1000000; // set sequential mode

    buf2[0] = 0b101;
    buf2[1] = 0b1000000;

    cs_select(CS_PIN_RAM);
    spi_write_blocking(SPI_PORT, buf, 2);
    cs_deselect(CS_PIN_RAM);

    // cs_select(CS_PIN_RAM);
    // spi_write_read_blocking(SPI_PORT, buf2, buf3, 2);
    // cs_deselect(CS_PIN_RAM);

    // printf("%x\n",buf3);

    printf("RAM Ready!\r\n");
    
}

void spi_ram_write(uint16_t addr, float v){
    uint8_t buf[7];

    union FloatInt {
        float f;
        uint32_t i;
    };

    union FloatInt num;
    num.f = v;

    buf[0] = 0b10; // sequential write instruction

    buf[1] = addr >> 8; // bit 15-8 of address
    buf[2] = addr & 0xFF; // bit 7-0 of address

    buf[3] = num.i >> 24;
    buf[4] = num.i >> 16;
    buf[5] = num.i >> 8;
    buf[6] = num.i;

    cs_select(CS_PIN_RAM);
    spi_write_blocking(SPI_PORT, buf, 7);
    cs_deselect(CS_PIN_RAM);

}

float spi_ram_read(uint16_t addr){
    uint8_t write[7], read[7];

    write[0] = 0b11; // instruction for sequential read sequence
    write[1] = addr >> 8; // bit 15-8 of address
    write[2] = addr & 0xFF; // bit 7-0 of address

    write[3] = 0b1; // send garbage
    write[4] = 0b1;
    write[5] = 0b1;
    write[6] = 0b1;

    cs_select(CS_PIN_RAM);
    spi_write_read_blocking(SPI_PORT, write, read, 7);
    // spi_write_blocking(SPI_PORT, write, 3);
    // spi_read_blocking(SPI_PORT, 0, read, 4);
    cs_deselect(CS_PIN_RAM);

    union FloatInt {
        float f;
        uint32_t i;
    };

    union FloatInt num;
    
    uint32_t raw = 0;
    raw = raw | (uint32_t)read[3]<<24;
    raw = raw | (uint32_t)read[4]<<16;
    raw = raw | (uint32_t)read[5]<<8;
    raw = raw | (uint32_t)read[6];

    num.i = raw;

    return num.f;
}

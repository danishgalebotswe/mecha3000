#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

#define SPI_PORT spi0
#define SCK_PIN_NUM 6
#define SDI_PIN_NUM 7
#define CS_PIN_NUM 9

void spi_initialize(void);
static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);
void writeDAC();

int main()
{
    stdio_init_all();
    spi_initialize();

    while (1){
        writeDAC();
        printf("Data written!\r\n");
        sleep_ms(1000);
    }
}

void spi_initialize(void){
    spi_init(SPI_PORT,12000); // set the SPI 0 at 12 kHz and connect to GPIOs
    gpio_set_function(SCK_PIN_NUM, GPIO_FUNC_SPI); // set the SCK to GP6 (pin 9)
    gpio_set_function(SDI_PIN_NUM, GPIO_FUNC_SPI); // set the SDI to GP7 (pin 10) 

    gpio_init(CS_PIN_NUM); // set CS to GP9 (pin 12)
    gpio_set_dir(CS_PIN_NUM, GPIO_OUT); // set GP9 to be output pin for chip select
    gpio_put(CS_PIN_NUM,1); // initialize GP9 to high
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

void writeDAC(){
    uint8_t data[2];
    int len = 2;
    data[0] = 0b01110011;
    data[1] = 0b00000000;
    cs_select(CS_PIN_NUM);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(CS_PIN_NUM);
}

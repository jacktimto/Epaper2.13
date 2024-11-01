#ifndef _MY_SPI_H_
#define _MY_SPI_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define MOSI 23
#define SCK 19
#define CS 22
#define DC 21
#define RST 18
#define BUSY 5

// Display resolution
#define WIDTH       122
#define HEIGHT      250

#define Read_BUSY_Status   gpio_get_level(BUSY)

enum data_cmd{
    cmd,
    data,
};

void gpio_init(void);
void spi_init(void);
void spi_write(int type, uint8_t tx_data);



#endif
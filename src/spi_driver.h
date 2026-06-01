#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H
#include <stdbool.h>
#include <stddef.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <stdint.h>

#define SPI_MAX_DEVICES 3
#define SPI_MAX_TRANSFER_SIZE 64
#define SPI_DRIVER_HOST SPI2_HOST

void spi_driver_init(gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk);
bool spi_driver_add_device(gpio_num_t cs_pin, uint8_t mode, uint32_t clock_hz);
bool spi_driver_transfer(gpio_num_t cs_pin, const uint8_t *tx, uint8_t *rx, size_t len);
bool spi_driver_read_register(gpio_num_t cs_pin, uint8_t reg, uint8_t *value);
bool spi_driver_write_register(gpio_num_t cs_pin, uint8_t reg, uint8_t value);

#endif // SPI_DRIVER_H
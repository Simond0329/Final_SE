#include "spi_driver.h"
#include <string.h>
#include "esp_log.h"
static const char *TAG = "SPI";
 
typedef struct { gpio_num_t cs_pin; spi_device_handle_t handle; bool active; } spi_device_entry_t;
static spi_device_entry_t s_devices[SPI_MAX_DEVICES];
static bool               s_initialized = false;
 
static bool find_device(gpio_num_t cs_pin, spi_device_handle_t *out_handle) {
    for (int i = 0; i < SPI_MAX_DEVICES; i++)
        if (s_devices[i].active && s_devices[i].cs_pin == cs_pin)
            { *out_handle = s_devices[i].handle; return true; }
    return false;
}
 
void spi_driver_init(gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk) {
    if (s_initialized) return;
    memset(s_devices, 0, sizeof(s_devices));
    spi_bus_config_t bus = {
        .mosi_io_num = mosi, .miso_io_num = miso, .sclk_io_num = sclk,
        .quadwp_io_num = -1, .quadhd_io_num = -1,
        .max_transfer_sz = SPI_MAX_TRANSFER_SIZE,
    };
    spi_bus_initialize(SPI_DRIVER_HOST, &bus, SPI_DMA_CH_AUTO);
    s_initialized = true;
    ESP_LOGI(TAG, "Init MOSI=%d MISO=%d SCLK=%d", mosi, miso, sclk);
    
}
 
bool spi_driver_add_device(gpio_num_t cs_pin, uint8_t mode, uint32_t clock_hz) {
    if (!s_initialized) return false;
    spi_device_handle_t existing;
    if (find_device(cs_pin, &existing)) return true;
    int slot = -1;
    for (int i = 0; i < SPI_MAX_DEVICES; i++) if (!s_devices[i].active) { slot = i; break; }
    if (slot == -1) return false;
    spi_device_interface_config_t dev = {
        .command_bits = 0, .address_bits = 0, .mode = mode,
        .clock_speed_hz = clock_hz, .spics_io_num = cs_pin, .queue_size = 1,
    };
    spi_device_handle_t handle;
    if (spi_bus_add_device(SPI_DRIVER_HOST, &dev, &handle) != ESP_OK) return false;
    s_devices[slot] = (spi_device_entry_t){ .cs_pin=cs_pin, .handle=handle, .active=true };
    ESP_LOGI(TAG, "Device CS=GPIO%d modo=%d %luHz slot[%d]", cs_pin, mode, (unsigned long)clock_hz, slot);
    return true;
}
 
bool spi_driver_transfer(gpio_num_t cs_pin, const uint8_t *tx, uint8_t *rx, size_t len) {
    if (!s_initialized || len == 0 || len > SPI_MAX_TRANSFER_SIZE) return false;
    spi_device_handle_t handle;
    if (!find_device(cs_pin, &handle)) return false;
    static uint8_t s_tx[SPI_MAX_TRANSFER_SIZE];
    static uint8_t s_rx[SPI_MAX_TRANSFER_SIZE];
    memcpy(s_tx, tx ? tx : (memset(s_tx, 0, len), s_tx), len);
    memset(s_rx, 0, len);
    spi_transaction_t t = { .length = len*8, .tx_buffer = s_tx, .rx_buffer = s_rx };
    esp_err_t err = spi_device_transmit(handle, &t);
    if (err != ESP_OK) return false;
    if (rx) memcpy(rx, s_rx, len);
    return true;
}
 
bool spi_driver_write_register(gpio_num_t cs_pin, uint8_t reg, uint8_t value) {
    if (!s_initialized) return false;
    spi_device_handle_t handle;
    if (!find_device(cs_pin, &handle)) return false;
    static uint8_t s_tx[2]; static uint8_t s_rx[2];
    s_tx[0] = reg & 0x7F; s_tx[1] = value;  // bit7=0 → escritura
    spi_transaction_t t = { .length = 16, .tx_buffer = s_tx, .rx_buffer = s_rx };
    return (spi_device_transmit(handle, &t) == ESP_OK);
}
 
bool spi_driver_read_register(gpio_num_t cs_pin, uint8_t reg, uint8_t *value) {
    if (!s_initialized || value == NULL) return false;
    spi_device_handle_t handle;
    if (!find_device(cs_pin, &handle)) return false;
    static uint8_t s_tx[2]; static uint8_t s_rx[2];
    s_tx[0] = reg | 0x80; s_tx[1] = 0x00;  // bit7=1 → lectura, dummy byte
    spi_transaction_t t = { .length = 16, .tx_buffer = s_tx, .rx_buffer = s_rx };
    if (spi_device_transmit(handle, &t) != ESP_OK) return false;
    *value = s_rx[1];  // rx[0] inválido, dato llega en rx[1]
    return true;
}

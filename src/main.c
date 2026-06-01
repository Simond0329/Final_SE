#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi_driver.h"
#include "uart_driver.h"
#include "adc_driver.h"

void spi_bus_init() {
    spi_driver_init(GPIO_NUM_23, GPIO_NUM_19, GPIO_NUM_18);
    spi_driver_add_device(GPIO_NUM_5, 3, 1000000); // modo 3, 1MHz
    uint8_t tx[2] = {0x0C, 0x00}; 
    uint8_t rx[2];
    spi_driver_transfer(GPIO_NUM_5, tx, rx, 2);
}

void mcp4132_write_register(uint8_t reg, uint8_t value) {
    spi_driver_write_register(GPIO_NUM_5, reg, value);
}

uint8_t mcp4132_read_register(uint8_t reg) {
    uint8_t value;
    spi_driver_read_register(GPIO_NUM_5, reg, &value);
    return value;
}

void mcp4132_set_wiper(uint8_t value) {
    value = value & 0x7F; // Asegurar que el valor esté en el rango de 0 a 127
    mcp4132_write_register(0x00, value);
}

void mcp4132_set_cutoff_frequency(float cutoff_freq) {
    
    float R = 1 / (2 * 3.14159 * cutoff_freq * 0.0000000001); // R en ohmios
    uint8_t wiper_value = (uint8_t)((R / 10000) * 127);
    mcp4132_set_wiper(wiper_value);
}


void app_main() {
    spi_bus_init();
    uart_driver_init();
    adc_driver_init(ADC_UNIT_1, NULL);
    float mv = adc_driver_read_raw(NULL, ADC_CHANNEL_0) * (3.3 / 4095) * 1000; 
    while (1) {
        if (mv > 1400) {
            mcp4132_set_wiper(95); 
            uart_driver_send_string("valor whiper: 95\n");
        } else if (mv < 900){
            mcp4132_set_wiper(42); // 100 Hz
            uart_driver_send_string("valor whiper: 42\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar 1 segundo
    }

    }

#include "uart_driver.h"
#include <string.h>
#include <stdio.h>
 
#define UART_BAUD_RATE 115200   // ejercicio: 115200 8N1
 
void uart_driver_init(void) {
    uart_config_t cfg = {
        .baud_rate  = UART_BAUD_RATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_param_config(UART_PORT, &cfg);
    uart_set_pin(UART_PORT,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_PORT, UART_BUF_SIZE, UART_BUF_SIZE, 0, NULL, 0);
}
 
void uart_driver_send_string(const char *msg) {
    uart_write_bytes(UART_PORT, msg, strlen(msg));
}
 
int uart_driver_read_line(char *buffer, int max_len) {
    int len = uart_read_bytes(UART_PORT,
                              (uint8_t *)buffer,
                              max_len - 1,
                              pdMS_TO_TICKS(10));
    if (len > 0) buffer[len] = '\0';
    return len;
}
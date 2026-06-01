#ifndef UART_DRIVER_H
#define UART_DRIVER_H
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "driver/uart.h"
 
#define UART_PORT      UART_NUM_0   // USB-Serial integrado
#define UART_BUF_SIZE  1024
 
void uart_driver_init(void);
void uart_driver_send_string(const char *msg);
int  uart_driver_read_line(char *buffer, int max_len);
#endif

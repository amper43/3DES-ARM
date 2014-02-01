#ifndef _UART_IO_H
#define _UART_IO_H


#include "stm32f4xx_usart.h"
#include <stdint.h>


void send_to_uart3(uint8_t data);
void send_to_uart4(uint8_t data);
void send_str_uart(uint8_t* string,int length, uint8_t numb,void (*foo)(uint8_t));
void read_str_uart3(uint8_t* buf);

#endif

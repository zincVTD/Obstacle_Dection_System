#ifndef __UART_H__
#define __UART_H__

#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART

// USART1 configurations
#define UART_PORT         USART1
#define UART_GPIO_PORT    GPIOA
#define UART_TX_PIN       GPIO_Pin_9
#define UART_RX_PIN       GPIO_Pin_10
#define UART_BAUDRATE     115200

void UART_Init(void);
void UART_SendChar(char c);
void UART_SendString(const char *str);
char UART_ReceiveChar(void);
void UART_ReceiveString(char *buffer, uint16_t maxLength);
void UART_ConvertIntToString(uint32_t num, char *str);

#endif

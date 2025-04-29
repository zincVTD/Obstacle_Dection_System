#include "uart.h"

/**
  * @brief  Initializes the UART peripheral.
  * @param  None
  * @retval None
  */
void UART_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = UART_TX_PIN;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(UART_GPIO_PORT, &gpio);

	gpio.GPIO_Pin = UART_RX_PIN;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(UART_GPIO_PORT, &gpio);

	USART_InitTypeDef uart;
	uart.USART_BaudRate = UART_BAUDRATE;
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(UART_PORT, &uart);

	USART_Cmd(UART_PORT, ENABLE);
}

/**
  * @brief  Sends a single character over UART.
  * @param  c: Character to be sent.
  * @retval None
  */
void UART_SendChar(char c) {
	while (USART_GetFlagStatus(UART_PORT, USART_FLAG_TXE) == RESET);
	USART_SendData(UART_PORT, c);
}

/**
  * @brief  Sends a null-terminated string over UART.
  * @param  str: Pointer to the string to send.
  * @retval None
  */
void UART_SendString(const char *str) {
	while (*str) {
			UART_SendChar(*str++);
	}
}

/**
  * @brief  Receives a single character from UART.
  * @param  None
  * @retval Received character.
  */
char UART_ReceiveChar(void) {
	while (USART_GetFlagStatus(UART_PORT, USART_FLAG_RXNE) == RESET);
	return (char)USART_ReceiveData(UART_PORT);
}

/**
  * @brief  Receives a string from UART until newline or maximum length.
  * @param  buffer: Pointer to the buffer to store the received string.
  * @param  maxLength: Maximum length of the buffer.
  * @retval None
  */
void UART_ReceiveString(char *buffer, uint16_t maxLength) {
	uint16_t i = 0;
	char c;
	do {
			c = UART_ReceiveChar();
			UART_SendChar(c);  // echo back
			if (c == '\r' || c == '\n') break;

			if (i < maxLength - 1) {
					buffer[i++] = c;
			}
	} while (1);
	buffer[i] = '\0';
}

/**
  * @brief  Converts an integer to a string.
  * @param  num: Integer number to convert.
  * @param  str: Pointer to the buffer to store the resulting string.
  * @retval None
  */
void UART_ConvertIntToString(uint32_t num, char *str){
	int i = 0;
	if (num == 0){
		str[i++] = '0';
		str[i] = '\0';
		return;
	}
	while (num > 0){
		str[i++] = (num % 10) + '0';
		num /= 10;
	}
	str[i] = '\0';

	// d?o chu?i l?i
	for (int j = 0; j < i/2; j++) {
		char tmp = str[j];
		str[j] = str[i - j - 1];
		str[i - j - 1] = tmp;
	}
}

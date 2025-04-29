#ifndef __SSD1306_H
#define __SSD1306_H

#include "stm32f10x_i2c.h"              // Keil::Device:StdPeriph Drivers:I2C
#include "osKernel.h"

// I2C Configuration (ch?nh l?i n?u b?n dùng I2C khác)
#define SSD1306_I2C        	I2C1
#define SSD1306_ADDRESS    	0x78 // (0x3C << 1)

#define SSD1306_WIDTH      	128
#define SSD1306_HEIGHT     	64
#define SSD1306_MAX_LINE   	7

#define I2C_TIMEOUT					10000

/**
  * @brief  Struct storing position and data of the label.
  */
typedef struct {
    char label[4];
    uint8_t x;
    uint8_t y;
} ADAS_DataLabel;

extern ADAS_DataLabel adas_labels[];

void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_SetCursor(uint8_t line, uint8_t column);
void SSD1306_PrintChar(char c);
void SSD1306_PrintString(char *str);
void SSD1306_SetBrightness(uint8_t brightness);

#endif

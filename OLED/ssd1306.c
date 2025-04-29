#include "ssd1306.h"

ADAS_DataLabel adas_labels[] = {
    {"---", 14, 5},   // top-left
    {"---", 98, 5},   // top-right
    {"---", 14, 28},  // mid-left
    {"---", 98, 28},  // mid-right
    {"---", 14, 51},  // bot-left
    {"---", 98, 51},  // bot-right
    {"---", 53, 1},   // top center 1
    {"---", 71, 1},   // top center 2
    {"---", 53, 42},  // bot center 1
    {"---", 71, 42}   // bot center 2
};

// Font 5x7
static const uint8_t font5x7[][5] = {
		{0x00, 0x00, 0x00, 0x00, 0x00}, // space
		{0x00, 0x00, 0x2f, 0x00, 0x00}, // !
		{0x00, 0x07, 0x00, 0x07, 0x00}, // "
		{0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
		{0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
		{0x23, 0x13, 0x08, 0x64, 0x62}, // %
		{0x36, 0x49, 0x55, 0x22, 0x50}, // &
		{0x00, 0x05, 0x03, 0x00, 0x00}, // '
		{0x00, 0x1c, 0x22, 0x41, 0x00}, // (
		{0x00, 0x41, 0x22, 0x1c, 0x00}, // )
		{0x14, 0x08, 0x3E, 0x08, 0x14}, // *
		{0x08, 0x08, 0x3E, 0x08, 0x08}, // +
		{0x00, 0x00, 0xA0, 0x60, 0x00}, // ,
		{0x08, 0x08, 0x08, 0x08, 0x08}, // -
		{0x00, 0x60, 0x60, 0x00, 0x00}, // .
		{0x20, 0x10, 0x08, 0x04, 0x02}, // /
		{0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
		{0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
		{0x42, 0x61, 0x51, 0x49, 0x46}, // 2
		{0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
		{0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
		{0x27, 0x45, 0x45, 0x45, 0x39}, // 5
		{0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
		{0x01, 0x71, 0x09, 0x05, 0x03}, // 7
		{0x36, 0x49, 0x49, 0x49, 0x36}, // 8
		{0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
		{0x00, 0x36, 0x36, 0x00, 0x00}, // :
		{0x00, 0x56, 0x36, 0x00, 0x00}, // ;
		{0x08, 0x14, 0x22, 0x41, 0x00}, // <
		{0x14, 0x14, 0x14, 0x14, 0x14}, // =
		{0x00, 0x41, 0x22, 0x14, 0x08}, // >
		{0x02, 0x01, 0x51, 0x09, 0x06}, // ?
		{0x32, 0x49, 0x59, 0x51, 0x3E}, // @
		{0x7C, 0x12, 0x11, 0x12, 0x7C}, // A
		{0x7F, 0x49, 0x49, 0x49, 0x36}, // B
		{0x3E, 0x41, 0x41, 0x41, 0x22}, // C
		{0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
		{0x7F, 0x49, 0x49, 0x49, 0x41}, // E
		{0x7F, 0x09, 0x09, 0x09, 0x01}, // F
		{0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
		{0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
		{0x00, 0x41, 0x7F, 0x41, 0x00}, // I
		{0x20, 0x40, 0x41, 0x3F, 0x01}, // J
		{0x7F, 0x08, 0x14, 0x22, 0x41}, // K
		{0x7F, 0x40, 0x40, 0x40, 0x40}, // L
		{0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
		{0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
		{0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
		{0x7F, 0x09, 0x09, 0x09, 0x06}, // P
		{0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
		{0x7F, 0x09, 0x19, 0x29, 0x46}, // R
		{0x46, 0x49, 0x49, 0x49, 0x31}, // S
		{0x01, 0x01, 0x7F, 0x01, 0x01}, // T
		{0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
		{0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
		{0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
		{0x63, 0x14, 0x08, 0x14, 0x63}, // X
		{0x07, 0x08, 0x70, 0x08, 0x07}, // Y
		{0x61, 0x51, 0x49, 0x45, 0x43}, // Z
		{0x00, 0x7F, 0x41, 0x41, 0x00}, // [
		{0x55, 0xAA, 0x55, 0xAA, 0x55}, // Backslash (Checker pattern)
		{0x00, 0x41, 0x41, 0x7F, 0x00}, // ]
		{0x04, 0x02, 0x01, 0x02, 0x04}, // ^
		{0x40, 0x40, 0x40, 0x40, 0x40}, // _
		{0x00, 0x03, 0x05, 0x00, 0x00}, // `
		{0x20, 0x54, 0x54, 0x54, 0x78}, // a
		{0x7F, 0x48, 0x44, 0x44, 0x38}, // b
		{0x38, 0x44, 0x44, 0x44, 0x20}, // c
		{0x38, 0x44, 0x44, 0x48, 0x7F}, // d
		{0x38, 0x54, 0x54, 0x54, 0x18}, // e
		{0x08, 0x7E, 0x09, 0x01, 0x02}, // f
		{0x18, 0xA4, 0xA4, 0xA4, 0x7C}, // g
		{0x7F, 0x08, 0x04, 0x04, 0x78}, // h
		{0x00, 0x44, 0x7D, 0x40, 0x00}, // i
		{0x40, 0x80, 0x84, 0x7D, 0x00}, // j
		{0x7F, 0x10, 0x28, 0x44, 0x00}, // k
		{0x00, 0x41, 0x7F, 0x40, 0x00}, // l
		{0x7C, 0x04, 0x18, 0x04, 0x78}, // m
		{0x7C, 0x08, 0x04, 0x04, 0x78}, // n
		{0x38, 0x44, 0x44, 0x44, 0x38}, // o
		{0xFC, 0x24, 0x24, 0x24, 0x18}, // p
		{0x18, 0x24, 0x24, 0x18, 0xFC}, // q
		{0x7C, 0x08, 0x04, 0x04, 0x08}, // r
		{0x48, 0x54, 0x54, 0x54, 0x20}, // s
		{0x04, 0x3F, 0x44, 0x40, 0x20}, // t
		{0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
		{0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
		{0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
		{0x44, 0x28, 0x10, 0x28, 0x44}, // x
		{0x1C, 0xA0, 0xA0, 0xA0, 0x7C}, // y
		{0x44, 0x64, 0x54, 0x4C, 0x44}, // z
		{0x00, 0x10, 0x7C, 0x82, 0x00}, // {
		{0x00, 0x00, 0xFF, 0x00, 0x00}, // |
		{0x00, 0x82, 0x7C, 0x10, 0x00}, // }
		{0x00, 0x06, 0x09, 0x09, 0x06}	// ~ (Degrees)
};

static uint8_t current_line = 0;
static uint8_t current_column = 0;

static void I2C_ResetBus(void) {
	I2C_GenerateSTOP(SSD1306_I2C, ENABLE);
	I2C_SoftwareResetCmd(SSD1306_I2C, ENABLE);
	for (volatile int i = 0; i < 1000; i++);  // short delay
	I2C_SoftwareResetCmd(SSD1306_I2C, DISABLE);
	I2C_Cmd(SSD1306_I2C, ENABLE);
}

static uint8_t I2C_WaitForEvent(uint32_t event) {
	uint32_t timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(SSD1306_I2C, event)) {
		if (--timeout == 0) {
			I2C_ResetBus();  // Reset if timeout
			return 0;
		}
		osThreadYield();
	}
	return 1;
}

static void I2C_WriteByte(uint8_t control, uint8_t data) {
	uint32_t timeout = I2C_TIMEOUT;

	while (I2C_GetFlagStatus(SSD1306_I2C, I2C_FLAG_BUSY)) {
		if (--timeout == 0) {
			I2C_ResetBus();  // reset if timeout
			return;
		}
		osThreadYield();
	}

	I2C_GenerateSTART(SSD1306_I2C, ENABLE);
	if (!I2C_WaitForEvent(I2C_EVENT_MASTER_MODE_SELECT)) return;

	I2C_Send7bitAddress(SSD1306_I2C, SSD1306_ADDRESS, I2C_Direction_Transmitter);
	if (!I2C_WaitForEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) return;

	I2C_SendData(SSD1306_I2C, control);
	if (!I2C_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) return;

	I2C_SendData(SSD1306_I2C, data);
	if (!I2C_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) return;

	I2C_GenerateSTOP(SSD1306_I2C, ENABLE);
}

static void SSD1306_WriteCmd(uint8_t cmd) {
	I2C_WriteByte(0x00, cmd);
}

static void SSD1306_WriteData(uint8_t data) {
	I2C_WriteByte(0x40, data);
}

/**
  * @brief  Sets the cursor position on the OLED screen.
  * @param  line: Line number (page address).
  * @param  column: Column number (pixel address).
  * @retval None
  */
void SSD1306_SetCursor(uint8_t line, uint8_t column) {
	current_line = line;
	current_column = column;

	SSD1306_WriteCmd(0x21); // Set column address
	SSD1306_WriteCmd(column);
	SSD1306_WriteCmd(SSD1306_WIDTH - 1);

	SSD1306_WriteCmd(0x22); // Set page address
	SSD1306_WriteCmd(line);
	SSD1306_WriteCmd(SSD1306_MAX_LINE);
}

/**
  * @brief  Clears the entire OLED display.
  * @param  None
  * @retval None
  */
void SSD1306_Clear(void) {
	SSD1306_SetCursor(0, 0);
	for (uint16_t i = 0; i < SSD1306_WIDTH * 8; i++) {
		SSD1306_WriteData(0x00);
	}
	SSD1306_SetCursor(0, 0);
}

/**
  * @brief  Prints a single character to the OLED display.
  * @param  c: Character to print.
  * @retval None
  */
void SSD1306_PrintChar(char c) {
	if (c < 0x20 || c > 0x7F) c = ' ';
	const uint8_t *bitmap = font5x7[c - 0x20];
	for (int i = 0; i < 5; i++) {
		SSD1306_WriteData(bitmap[i]);
	}
	SSD1306_WriteData(0x00); // space between characters
}

/**
  * @brief  Prints a string vertically starting from a position.
  * @param  x: X-coordinate position.
  * @param  y: Y-coordinate position.
  * @param  text: Pointer to the string to print.
  * @retval None
  */
void ADAS_PrintVertical(uint8_t x, uint8_t y, const char *text) {
	while (*text) {
		SSD1306_SetCursor(x, y);
		SSD1306_PrintChar(*text);
		y += 8; // Each character is 8 pixel high
		text++;
	}
}

/**
  * @brief  Prints a string to the OLED display.
  * @param  str: Pointer to the string to print.
  * @retval None
  */
void SSD1306_PrintString(char *str) {
	while (*str) {
		if (*str == '\n') {
			current_line = (current_line + 1) % 8;
			SSD1306_SetCursor(current_line, 0);
		} else {
			SSD1306_PrintChar(*str);
			current_column += 6;
			if (current_column + 5 >= SSD1306_WIDTH) {
				current_line = (current_line + 1) % 8;
				SSD1306_SetCursor(current_line, 0);
			}
		}
		str++;
	}
}

/**
  * @brief  Sets the brightness level of the OLED display.
  * @param  brightness: Brightness value (0-255).
  * @retval None
  */
void SSD1306_SetBrightness(uint8_t brightness) {
    SSD1306_WriteCmd(0x81);
    SSD1306_WriteCmd(brightness);
}

/**
  * @brief  Initializes the SSD1306 OLED display via I2C.
  * @param  None
  * @retval None
  */
void SSD1306_Init(void) {
    // I2C1 init (PB6: SCL, PB7: SDA)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);

    I2C_InitTypeDef i2c;
    i2c.I2C_ClockSpeed = 400000;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = 0x00;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(SSD1306_I2C, &i2c);
    I2C_Cmd(SSD1306_I2C, ENABLE);

    // SSD1306 init sequence
    SSD1306_WriteCmd(0xAE);
    SSD1306_WriteCmd(0xD5); SSD1306_WriteCmd(0x80);
    SSD1306_WriteCmd(0xA8); SSD1306_WriteCmd(0x3F);
    SSD1306_WriteCmd(0xD3); SSD1306_WriteCmd(0x00);
    SSD1306_WriteCmd(0x40);
    SSD1306_WriteCmd(0x8D); SSD1306_WriteCmd(0x14);
    SSD1306_WriteCmd(0x20); SSD1306_WriteCmd(0x00);
    SSD1306_WriteCmd(0xA1);
    SSD1306_WriteCmd(0xC8);
    SSD1306_WriteCmd(0xDA); SSD1306_WriteCmd(0x12);
    SSD1306_WriteCmd(0x81); SSD1306_WriteCmd(0xCF);
    SSD1306_WriteCmd(0xD9); SSD1306_WriteCmd(0xF1);
    SSD1306_WriteCmd(0xDB); SSD1306_WriteCmd(0x40);
    SSD1306_WriteCmd(0xA4);
    SSD1306_WriteCmd(0xA6);
    SSD1306_WriteCmd(0xAF); // Display ON

    SSD1306_Clear();
}

/**
  * @brief  Draws a single pixel on the OLED screen.
  * @param  x: X-coordinate.
  * @param  y: Y-coordinate.
  * @param  color: 1 to set pixel, 0 to clear pixel.
  * @retval None
  */
void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
        return;

    SSD1306_WriteCmd(0xB0 + (y / 8));         // Set page address (y / 8)
    SSD1306_WriteCmd(0x00 + (x & 0x0F));      // Set lower column address
    SSD1306_WriteCmd(0x10 + ((x >> 4) & 0x0F)); // Set higher column address

    uint8_t data = (1 << (y % 8));
    if (color)
        SSD1306_WriteData(data);  // ghi pixel
    else
        SSD1306_WriteData(0x00);  // xóa pixel
}

/**
  * @brief  Draws a line between two points on the OLED screen.
  * @param  x0: Starting x-coordinate.
  * @param  y0: Starting y-coordinate.
  * @param  x1: Ending x-coordinate.
  * @param  y1: Ending y-coordinate.
  * @retval None
  */
void SSD1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    int dx, dy, sx, sy, err, e2;

    if (x0 > x1) {
        dx = x0 - x1;
        sx = -1;
    }
		else {
        dx = x1 - x0;
        sx = 1;
    }

    if (y0 > y1) {
        dy = y0 - y1;
        sy = -1;
    }
		else {
        dy = y1 - y0;
        sy = 1;
    }

    dx = dx;
    dy = -dy;
    err = dx + dy;

    while (1) {
        SSD1306_DrawPixel(x0, y0, 1);
        if (x0 == x1 && y0 == y1) break;

        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

/**
  * @brief  Draws a rectangle on the OLED screen.
  * @param  x: X-coordinate of top-left corner.
  * @param  y: Y-coordinate of top-left corner.
  * @param  w: Width of the rectangle.
  * @param  h: Height of the rectangle.
  * @retval None
  */
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    SSD1306_DrawLine(x, y, x + w - 1, y);           // top
    SSD1306_DrawLine(x, y + h - 1, x + w - 1, y + h - 1); // bottom
    SSD1306_DrawLine(x, y, x, y + h - 1);           // left
    SSD1306_DrawLine(x + w - 1, y, x + w - 1, y + h - 1); // right
}

/**
  * @brief  Sets a text label in the ADAS display layout.
  * @param  index: Index of the label (0-9).
  * @param  text: Pointer to the label text.
  * @retval None
  */
void ADAS_SetLabel(uint8_t index, const char *text) {
    if (index >= sizeof(adas_labels)/sizeof(adas_labels[0]))
        return;

    int i = 0;
    while (i < 4 && text[i]) {
        adas_labels[index].label[i] = text[i];
        i++;
    }
    adas_labels[index].label[i] = '\0';
}

/**
  * @brief  Draws the predefined ADAS layout on the OLED screen.
  * @param  None
  * @retval None
  */
void drawADASLayout(void) {
    SSD1306_Clear();

    // V? khung
    SSD1306_DrawRect(46, 24, 36, 17);
    SSD1306_DrawLine(64, 23, 64, 0);
    SSD1306_DrawLine(81, 41, 81, 63);
    SSD1306_DrawLine(82, 40, 127, 48);
    SSD1306_DrawLine(64, 63, 64, 41);
    SSD1306_DrawLine(81, 0, 81, 23);
    SSD1306_DrawLine(82, 24, 127, 16);
    SSD1306_DrawLine(46, 23, 46, 0);
    SSD1306_DrawLine(46, 63, 46, 41);
    SSD1306_DrawLine(0, 16, 45, 24);
    SSD1306_DrawLine(0, 48, 45, 40);

    SSD1306_SetCursor(53, 29);
    SSD1306_PrintString("ADAS");

    for (uint8_t i = 0; i < sizeof(adas_labels)/sizeof(adas_labels[0]); i++) {
        SSD1306_SetCursor(adas_labels[i].x, adas_labels[i].y);
        SSD1306_PrintString(adas_labels[i].label);
    }
}

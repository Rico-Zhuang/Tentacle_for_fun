/*
 * LCD_GFX.h
 *
 * Created: 9/20/2021 6:54:37 PM
 *  Author: You
 */ 

#include <avr/io.h>
#include "ASCII_LUT.h"

#ifndef LCD_GFX_H_
#define LCD_GFX_H_

// colors
#define	BLACK     0x0000
#define WHITE     0xFFFF
#define	BLUE      0x001F
#define	RED       0xF800
#define	GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define BROWN     0x79E0
#define EYELLOW   0xFE00
#define LIGHTBLUE 0x8D7B
#define PINK      0xFC88
#define IVORY     0xF6DB

uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue);
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color);
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor);
void LCD_drawHLine(uint8_t x, uint8_t y, uint8_t w, uint16_t color);
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color);
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c);
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color);
void LCD_setScreen(uint16_t color);
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg);
void LCD_drawCatMouth(uint8_t x, uint8_t y, uint16_t color);
void LCD_fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color);
void LCD_drawArc_fast(uint8_t cx, uint8_t cy, uint8_t r,
                      uint16_t start_angle, uint16_t end_angle,
                      uint8_t thickness, uint16_t color);

void LCD_drawThickLine(short x0, short y0, short x1, short y1, uint8_t thickness, uint16_t color);
void LCD_drawArc(uint8_t cx, uint8_t cy, uint8_t r,
                      float start_angle, float end_angle,
                      uint8_t thickness, uint16_t color);
void LCD_fillOval(int cx, int cy, int rx, int ry, uint16_t color);
#endif /* LCD_GFX_H_ */
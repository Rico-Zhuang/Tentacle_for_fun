/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */ 

#include "LCD_GFX.h"
#include "ST7735.h"
#include <math.h>

/******************************************************************************
* Local Functions
******************************************************************************/



/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
	uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
		for(i=0;i<5;i++){
			uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
			for(j=0;j<8;j++){
				if ((pixels>>j)&(1==1)){
					LCD_drawPixel(x+i,y+j,fColor);
				}
				else {
					LCD_drawPixel(x+i,y+j,bColor);
				}
			}
		}
	}
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/

static inline void drawHSpan(int16_t x0, int16_t x1, int16_t y, uint16_t color)
{
    if (x0 > x1) { int16_t t = x0; x0 = x1; x1 = t; }
    LCD_setAddr((uint8_t)x0, (uint8_t)y, (uint8_t)x1, (uint8_t)y);
    for (int16_t x = x0; x <= x1; x++) {
        SPI_ControllerTx_16bit(color);
    }
}

void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color)
{
    int16_t cx = x0, cy = y0;
    int16_t x  = radius;
    int16_t y  = 0;
    int16_t d  = 1 - (int16_t)radius;

    // center scanline
    drawHSpan(cx - x, cx + x, cy, color);

    while (y < x) {
        y++;
        if (d < 0) {
            d += (y << 1) + 1;           // move vertically
        } else {
            x--;
            d += ((y - x) << 1) + 1;     // move diagonally
        }

        // four symmetric filled spans
        drawHSpan(cx - x, cx + x, cy + y, color);
        drawHSpan(cx - x, cx + x, cy - y, color);
        drawHSpan(cx - y, cx + y, cy + x, color);
        drawHSpan(cx - y, cx + y, cy - x, color);
    }
}


/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
{
	// Fill this out
    float gradient = (float) (y1 - y0) / (x1 - x0);
    int delta_y = 0;
    if (x0<x1) // x0<x1 increment
    {
        for(uint8_t x=x0; x<=x1; x++)
        {
            delta_y = (int) round(gradient * (x - x0)); // round delta y
            LCD_drawPixel(x, y0 + delta_y, c);
        }
    }
    else if(x0>x1)// x0>x1 decrement
    {
        for(uint8_t x=x0; x>=x1; x--)
        {
            delta_y = (int) round(gradient * (x - x0)); // round delta y
            LCD_drawPixel(x, y0 + delta_y, c);
        }
    }
    else // x0=x1
    {  
        if (y0<=y1) // x0<x1 increment
    {
        for(uint8_t y=y0; y<=y1; y++)
        {
            LCD_drawPixel(x0, y, c);
        }
    }
    else if(y0>y1)// x0>x1 decrement
    {
        for(uint8_t y=y0; y>=y1; y--)
        {

            LCD_drawPixel(x0, y, c);
        }
    }
    }
}



/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
    LCD_setAddr(x0, y0, x1, y1);
    uint32_t count = (uint32_t)(x1-x0) * (uint32_t)(y1-y0);
    while (count--) {
        SPI_ControllerTx_16bit(color);
    }
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color)
{
    // Set a single drawing window = full screen
    LCD_setAddr(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    // Stream the same color for every pixel
    uint32_t count = (uint32_t)LCD_WIDTH * (uint32_t)LCD_HEIGHT;
    while (count--) {
        SPI_ControllerTx_16bit(color);
    }
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
{
	// Fill this out
    // one character, 6x8. x: 6 pixel, y: 8 pixel
    for (uint8_t i=0; str[i]!='\0'; i++)
    {
        LCD_drawChar(x + i * 6, y, str[i], fg, bg);
    }
}

// mouth size 16×10
// void LCD_drawCatMouth(uint8_t x, uint8_t y, uint16_t color) {
// uint16_t mouth[10] = {
//     0b1111111100000000,
//     0b1111111100000000,
//     0b1111111100000000,
//     0b1111111100000000,
//     0b1111111100000000,
//     0b1111111100000000,
//     0b1111111100000000,
//     0b1111111100000000,
//     0b1111111100000000,
//     0b1111111100000000
// };
//     LCD_setAddr(x - 8, y - 5, x + 8, y + 5);

//     for (uint8_t i = 0; i < 10; i++) {
//         uint16_t bits = mouth[i];
//         for (uint16_t mask = (1u << 15); mask != 0; mask >>= 1) {
//             LCD_drawPixel(x+i,y+j,fColor);
//         }
//     }
// }
void LCD_drawCatMouth(uint8_t x, uint8_t y, uint16_t color) {
uint16_t mouth[10] = {
    0b0000000000000000,
    0b0011110000111100,
    0b0111111001111110,
    0b1111011111101111,
    0b1110001111000111,
    0b1100000110000011,
    0b1100000110000011,
    0b1100001111000011,
    0b1100000000000011,
    0b1110000000000111
};

    for (uint8_t row = 0; row < 10; row++) {
        uint16_t bits = mouth[row];

        for (uint8_t col = 0; col < 16; col++) {
            // mask bit: leftmost is bit 15
            uint16_t mask = 1u << (15 - col);

            // compute screen pixel position
            uint8_t px = x - 8 + col;
            uint8_t py = y - 5 + row;

            if (bits & mask) {
                LCD_drawPixel(px, py, color);
            } else {
                LCD_drawPixel(px, py, BLACK);
            }
        }
    }
}

void LCD_fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint16_t color)
{
    for (int16_t y = -r; y <= r; y++) {
        int16_t x_span = (int16_t)(sqrt((float)(r * r - y * y)));
        LCD_drawLine(x0 - x_span, y0 + y, x0 + x_span, y0 + y, color);
    }
}

void LCD_drawThickLine(short x0, short y0, short x1, short y1, uint8_t thickness, uint16_t color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    float length = sqrt(dx * dx + dy * dy);

  
    float ux = dx / length;
    float uy = dy / length;

   
    float px = -uy;
    float py = ux;
    
    for (int t = -thickness / 2; t <= thickness / 2; t++)
    {
        short offsetX = (short)(px * t);
        short offsetY = (short)(py * t);
        LCD_drawLine(x0 + offsetX, y0 + offsetY, x1 + offsetX, y1 + offsetY, color);
    }
}



void LCD_drawArc_fast(uint8_t cx, uint8_t cy, uint8_t r,
                      uint16_t start_angle, uint16_t end_angle,
                      uint8_t thickness, uint16_t color)
{
    int x = 0;
    int y = r;
    int d = 1 - r;

    while (y >= x) {

        // 8 symmetric circle points
        int pts[8][2] = {
            { cx + x, cy + y },
            { cx + y, cy + x },
            { cx - x, cy + y },
            { cx - y, cy + x },
            { cx - x, cy - y },
            { cx - y, cy - x },
            { cx + x, cy - y },
            { cx + y, cy - x }
        };

        // For thickness: draw multiple pixels outward
        for (int t = 0; t < thickness; t++) {

            for (int i = 0; i < 8; i++) {
                int px = pts[i][0];
                int py = pts[i][1];

                // Shift outward or inward based on thickness
                if (i < 4) px += t; else px -= t;

                // Compute angle of this point
                long dx = px - cx;
                long dy = py - cy;
                long ang = (atan2(dy, dx) * 180.0 / M_PI);
                if (ang < 0) ang += 360;

                // Draw only if the angle is within the arc range
                if (ang >= start_angle && ang <= end_angle) {
                    LCD_drawPixel(px, py, color);
                }
            }
        }

        // Bresenham update
        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}


void LCD_drawArc(uint8_t cx, uint8_t cy, uint8_t r,
                      float start_angle, float end_angle,
                      uint8_t thickness, uint16_t color)
{
    float step = 0.5f;  

    for (float angle = start_angle; angle <= end_angle; angle += step)
    {
        float rad = angle * M_PI / 180.0f;

        for (int t = 0; t < thickness; t++)
        {
            float rr = r - (thickness / 2.0f) + t;
            int x = (int)(cx + rr * cos(rad));
            int y = (int)(cy + rr * sin(rad));
            LCD_drawPixel(x, y, color);
        }
    }
}

void LCD_fillOval(int cx, int cy, int rx, int ry, uint16_t color)
{
    long rx2 = (long)rx * rx;
    long ry2 = (long)ry * ry;
    long two_rx2 = 2 * rx2;
    long two_ry2 = 2 * ry2;

    long x = 0;
    long y = ry;

    long px = 0;
    long py = two_rx2 * y;

    long d1 = ry2 - rx2 * ry + (rx2 / 4);

    while (px < py) {
        LCD_drawLine(cx - x, cy + y, cx + x, cy + y, color);
        LCD_drawLine(cx - x, cy - y, cx + x, cy - y, color);

        x++;
        px += two_ry2;

        if (d1 < 0) {
            d1 += ry2 + px;
        } else {
            y--;
            py -= two_rx2;
            d1 += ry2 + px - py;
        }
    }

    long d2 = ry2 * (x + 0.5)*(x + 0.5) + rx2 * (y - 1)*(y - 1) - rx2 * ry2;

    while (y >= 0) {
        LCD_drawLine(cx - x, cy + y, cx + x, cy + y, color);
        LCD_drawLine(cx - x, cy - y, cx + x, cy - y, color);

        y--;
        py -= two_rx2;

        if (d2 > 0) {
            d2 += rx2 - py;
        } else {
            x++;
            px += two_ry2;
            d2 += rx2 - py + px;
        }
    }
}


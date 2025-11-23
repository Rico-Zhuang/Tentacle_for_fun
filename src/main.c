/*
 * ATmega328PB Servo Control
 * Output: PB1 (OC1A) - Corresponds to Arduino Pin 9
 * Clock: Assumes 16MHz External Crystal or Internal Oscillator
 */

// #define F_CPU 16000000UL // Define clock speed as 16MHz
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "lib/ST7735.h"
#include "lib/LCD_GFX.h"
#include "lib/motor.h"



/* system time include file. */
#include "time.h"

#include <avr/io.h>
#include <util/delay.h>
#include "lib/ST7735.h"
#include "lib/LCD_GFX.h"

void angry_frame(){
    uint8_t cx = LCD_WIDTH / 2;
    uint8_t cy = LCD_HEIGHT / 2;

    //face
    LCD_fillCircle(cx, cy, 55, RED);

    //eye
    LCD_fillCircle(cx-25, cy-10, 16, WHITE);
    LCD_fillCircle(cx+25, cy-10, 16, WHITE);
    LCD_fillCircle(cx-19, cy-10, 10, BLACK);
    LCD_fillCircle(cx+31, cy-10, 10, BLACK);
    
    //left eyebrow
    LCD_drawThickLine(cx-32, cy-44, cx-12, cy-34, 4, BROWN);

    //right eyebrow
    LCD_drawThickLine(cx+14, cy-34, cx+32, cy-44, 4, BROWN);

    //mouth
    LCD_drawArc(cx, cy+50, 30, -120, -40, 3, BROWN); 
    while (1)
    {
       LCD_drawThickLine(cx-32, cy-44, cx-12, cy-34, 4, RED);
       LCD_drawThickLine(cx+14, cy-34, cx+32, cy-44, 4, RED);
       LCD_fillCircle(cx-31, cy-10, 10, WHITE);
       LCD_fillCircle(cx+19, cy-10, 10, WHITE);
       _delay_ms(1);
       LCD_drawArc(cx-20, cy-60, 30, 90, 125, 3, BROWN); 
       LCD_drawArc(cx-25, cy-50, 20, 30, 90, 3, BROWN);
       LCD_drawArc(cx+23, cy-50, 20, 90, 125, 3, BROWN); 
       LCD_drawArc(cx+23, cy-70, 40, 60, 90, 3, BROWN); 
       LCD_fillCircle(cx-19, cy-10, 10, BLACK);
       LCD_fillCircle(cx+31, cy-10, 10, BLACK);
       _delay_ms(500);
       LCD_drawArc(cx-20, cy-60, 30, 90, 125, 3, RED); 
       LCD_drawArc(cx-25, cy-50, 20, 30, 90, 3, RED);
       LCD_drawArc(cx+23, cy-50, 20, 90, 125, 3, RED); 
       LCD_drawArc(cx+23, cy-70, 40, 60, 90, 3, RED); 
       LCD_fillCircle(cx-19, cy-10, 10, WHITE);
       LCD_fillCircle(cx+31, cy-10, 10, WHITE);
       _delay_ms(1);
       LCD_drawThickLine(cx-32, cy-44, cx-12, cy-34, 4, BROWN);
       LCD_drawThickLine(cx+14, cy-34, cx+32, cy-44, 4, BROWN);
       LCD_fillCircle(cx-31, cy-10, 10, BLACK);
       LCD_fillCircle(cx+19, cy-10, 10, BLACK);
       _delay_ms(500);
    }
}

void happy_frame(){
    uint8_t cx = LCD_WIDTH / 2;
    uint8_t cy = LCD_HEIGHT / 2;

    //face
    LCD_fillCircle(cx, cy, 55, EYELLOW);

    //eye
    LCD_drawArc_fast(cx-25, cy-10, 15, 180, 360, 4, BROWN); 
    LCD_drawArc_fast(cx+25, cy-10, 15, 180, 360, 4, BROWN); 

    //mouth
    LCD_drawThickLine(cx-38, cy+16, cx+38, cy+16, 2, BROWN);
    LCD_drawArc(cx, cy+3, 40, 20, 160, 2, BROWN); 
    LCD_drawThickLine(cx-8, cy+18, cx-8, cy+42, 2, BROWN);//left two
    LCD_drawThickLine(cx+8, cy+18, cx+8, cy+42, 2, BROWN);//right two
    LCD_drawThickLine(cx-23, cy+18, cx-23, cy+34, 2, BROWN);//left one
    LCD_drawThickLine(cx+23, cy+18, cx+23, cy+34, 2, BROWN);//right one
    
    //eyebrow
    LCD_drawArc(cx-35, cy-40, 10, 180, 320, 3, BROWN); //white
    LCD_drawArc(cx-35, cy-40, 10, 320, 360, 3, BROWN); //yellow
    LCD_drawArc(cx+35, cy-40, 10, 180, 220, 3, BROWN); //yellow
    LCD_drawArc(cx+35, cy-40, 10, 220, 360, 3, BROWN); //white
    
    
    while(1){
        LCD_drawBlock(cx-44, cy-30, cx+44, cy+5, EYELLOW);
        _delay_ms(1);
        LCD_drawArc_fast(cx-25, cy-15, 15, 180, 360, 4, BROWN); 
        LCD_drawArc_fast(cx+25, cy-15, 15, 180, 360, 4, BROWN); 
        _delay_ms(500);
        LCD_drawBlock(cx-44, cy-30, cx+44, cy+5, EYELLOW);
        _delay_ms(1);
        LCD_drawArc_fast(cx-25, cy-10, 15, 180, 360, 4, BROWN); 
        LCD_drawArc_fast(cx+25, cy-10, 15, 180, 360, 4, BROWN); 
        _delay_ms(500);
    }
}



int main(void)
{
    lcd_init();
    LCD_setScreen(WHITE);
    //angry_frame();
    happy_frame();
  
}
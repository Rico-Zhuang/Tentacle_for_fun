/*
 * ATmega328PB Servo Control
 * Output: PB1 (OC1A) - Corresponds to Arduino Pin 9
 * Clock: Assumes 16MHz External Crystal or Internal Oscillator
 */

#define F_CPU 16000000UL // Define clock speed as 16MHz
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

int main(void) {
    lcd_init();
    LCD_setScreen(BLACK);
    LCD_drawBlock(10, 10, 20, 30, WHITE);
    LCD_drawBlock(140, 108, 150, 118, YELLOW);
    LCD_drawBlock(140, 20, 150, 30, RED);
    LCD_drawCircle(40, 64, 20, WHITE);
    LCD_drawCircle(120, 64, 20, WHITE);


    servo_init();
    while (1) {
        // Move to 180 degrees
        servo_write(0);
        _delay_ms(1000);

        // Move to 0 degrees
        servo_write(180);
        _delay_ms(1000);
    }
    
    return 0;
}
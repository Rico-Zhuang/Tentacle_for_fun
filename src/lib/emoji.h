#include "lib/ST7735.h"
#include "lib/LCD_GFX.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#ifndef EMOJI_C
#define EMOJI_H

void angry_frame();
void happy_frame();
void sad_frame();
void smile_frame();

#endif
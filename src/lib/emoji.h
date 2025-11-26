#include "lib/ST7735.h"
#include "lib/LCD_GFX.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#ifndef EMOJI_H
#define EMOJI_H

//uint8_t EMOJI_STATE=0;
uint8_t smile_state;
uint8_t angry_state;
uint8_t sad_state;
uint8_t happy_state;
uint8_t cx;
uint8_t cy;
void angry_frame();
void happy_frame();
void sad_frame();
void smile_frame();

#endif
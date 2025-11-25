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

#include "time.h"
#include "lib/ST7735.h"
#include "lib/LCD_GFX.h"
#include "lib/PCA9685.h"
#include "lib/tentacle.h"

int main(void)
{
    PCA9685_Init(50.0);
    uint8_t delay = 1;
    Reset_tentacle(0);
    while(1){
        Sweep_0_1(delay);
        Sweep_1_2(delay);
        Sweep_2_0(delay);
    }
}
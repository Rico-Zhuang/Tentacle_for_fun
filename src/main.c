#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "./lib/adc.h"
#include "./lib/PCA9685.h"
#include "./lib/tentacle.h"
#include "./lib/LCD_GFX.h"
#include "./lib/emoji.h"
#include "./lib/ST7735.h"

ISR(TIMER1_COMPA_vect) {
    // 1. 读取传感器
    int raw_x = ADC_Read(0);
    int raw_y = ADC_Read(1);
    
    // 2. 控制逻辑 (数学计算)
    int center_x = 512, center_y = 512;
    int deadzone = 30;
    
    double map_x = raw_x - center_x;
    double map_y = raw_y - center_y;
    double magnitude = sqrt(map_x * map_x + map_y * map_y);
    
    if (magnitude < deadzone) {
        Tentacle_Move(0, 0);
    } else {
        double angle_rad = atan2(map_y, map_x);
        double angle_deg = angle_rad * 180.0 / PI;
        if (angle_deg < 0) angle_deg += 360.0;
        
        double bending_factor = magnitude / 512.0;
        if (bending_factor > 1.0) bending_factor = 1.0;
        
        Tentacle_Move(angle_deg, bending_factor);
    }
}


int main(void) {
    ADC_Init();
    Timer1_Init();
    lcd_init();
    LCD_setScreen(WHITE);
    smile_frame();
    PCA9685_Init(50.0); // 50Hz for servos
    sei(); // Enable global interrupts
    
    while (1) {
        // 主循环可以执行其他任务
    }

    return 0;
}
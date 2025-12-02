#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "./lib/adc.h"
#include "./lib/PCA9685.h"
#include "./lib/tentacle.h"
#include "./lib/LCD_GFX.h"
#include "./lib/emoji.h"
#include "./lib/ST7735.h"
#include "./lib/uart.h"
#include "./lib/ultrasonic.h"

double magnitude;
uint8_t cx = LCD_WIDTH / 2;
uint8_t cy = LCD_HEIGHT / 2;

void set_emoji_state(uint8_t happy, uint8_t smile, uint8_t angry, uint8_t sad)
{
    happy_state = happy;
    smile_state = smile;
    angry_state = angry;
    sad_state   = sad;
}

ISR(TIMER1_COMPA_vect) {
    // 1. 读取传感器
    int raw_x = ADC_Read(0);
    int raw_y = ADC_Read(1);
    
    // 2. 控制逻辑 (数学计算)
    int center_x = 512, center_y = 512;
    int deadzone = 30;
    
    double map_x = raw_x - center_x;
    double map_y = raw_y - center_y;
    magnitude = sqrt(map_x * map_x + map_y * map_y);
    
    if (magnitude < deadzone) {
        //Tentacle_Move(0, 0);
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
    //char buf[50];
    UART_init(BAUD_PRESCALER);

    ultrasonic_init();

    ADC_Init();
    Timer1_Init();

    lcd_init();
    LCD_setScreen(WHITE);
    smile_frame();

    PCA9685_Init(50.0); // 50Hz for servos
    sei(); // Enable global interrupts

    SetServoAngle(0, 0);
    SetServoAngle(1, 0);
    SetServoAngle(2, 0);
    smile_state=0;
    angry_state=0;
    happy_state=0;  
    sad_state=0;
    
     while (1) {
        SpeedLevel lvl = ultrasonic_get_speed_nonblocking();

        switch (lvl) {
            case SPEED_FAST:
                angry_frame();
                set_emoji_state(0,0,1,0);
                // LCD_drawArc(cx-35, cy-40, 10, 180, 320, 3, WHITE);
                // LCD_drawArc(cx+35, cy-40, 10, 220, 360, 3, WHITE);
                break;

            case SPEED_SLOW:
                happy_frame();
                set_emoji_state(1,0,0,0);
                // LCD_drawArc(cx-35, cy-40, 10, 180, 320, 3, WHITE);
                // LCD_drawArc(cx+35, cy-40, 10, 220, 360, 3, WHITE);
                break;

            case SPEED_NONE:
                smile_frame();
                set_emoji_state(0,1,0,0);
                // LCD_drawArc(cx-35, cy-40, 10, 180, 320, 3, WHITE);
                // LCD_drawArc(cx+35, cy-40, 10, 220, 360, 3, WHITE);
                break;

            case SPEED_BACK:
                sad_frame();
                set_emoji_state(0,0,0,1);
                break;
        }
    }
    return 0;
}
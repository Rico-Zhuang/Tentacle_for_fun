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
//uint8_t angry_mode = 0;

void set_emoji_state(uint8_t happy, uint8_t smile, uint8_t angry, uint8_t sad)
{
    happy_state = happy;
    smile_state = smile;
    angry_state = angry;
    sad_state   = sad;
}


int main(void) {
    
    //angry_mode = 0;
    UART_init(BAUD_PRESCALER);
    
    ultrasonic_init();
    

    // Initialize ADC
    ADC_Init();
    Timer1_Init();

    // Initialize LCD
    lcd_init();
    LCD_setScreen(WHITE);
    smile_frame();

    // Initialize PCA9685 for servo control
    PCA9685_Init(50.0); // 50Hz for servos
    sei(); // Enable global interrupts

    // SetServoAngle(0, 0);
    // SetServoAngle(1, 0);
    // SetServoAngle(2, 0);

     while (1) {
        ultrasonic_update();
        SpeedLevel lvl = ultrasonic_get_speed_status();
        
        switch (lvl) {
            case SPEED_FAST:
                angry_frame();
                set_emoji_state(0,0,1,0);
                //angry_mode = 1;
                //Motion_Strike(30.0);
                break;
            case SPEED_SLOW:
                happy_frame();
                set_emoji_state(1,0,0,0);
                break;

            case SPEED_NONE:
                smile_frame();
                set_emoji_state(0,1,0,0);
                //angry_mode = 0;
                break;

            case SPEED_BACK:
                sad_frame();
                set_emoji_state(0,0,0,1);
                break;
        }

    
    }
    return 0;
}
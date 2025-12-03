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

#define BENDING_AMP 1.0

double magnitude;
uint8_t cx = LCD_WIDTH / 2;
uint8_t cy = LCD_HEIGHT / 2;
uint8_t angry_mode = 0;

int raw_x = 0;
int raw_y = 0;
int raw_esp1 = 0;
int raw_esp2 = 0;
double bending_factor = 0.0;

void set_emoji_state(uint8_t happy, uint8_t smile, uint8_t angry, uint8_t sad)
{
    happy_state = happy;
    smile_state = smile;
    angry_state = angry;
    sad_state   = sad;
}

// ISR(TIMER1_COMPA_vect) {
//    
//     int raw_x = ADC_Read(0);
//     int raw_y = ADC_Read(1);
    
//     
//     int center_x = 512, center_y = 512;
//     int deadzone = 30;
    
//     double map_x = raw_x - center_x;
//     double map_y = raw_y - center_y;
//     magnitude = sqrt(map_x * map_x + map_y * map_y);
    
//     if (magnitude < deadzone) {
//         //Tentacle_Move(0, 0);
//     } else {
//         double angle_rad = atan2(map_y, map_x);
//         double angle_deg = angle_rad * 180.0 / PI;
//         if (angle_deg < 0) angle_deg += 360.0;
        
//         double bending_factor = magnitude / 512.0;
//         if (bending_factor > 1.0) bending_factor = 1.0;
        
//         Tentacle_Move(angle_deg, bending_factor);
//     }
// }

ISR(TIMER1_COMPA_vect) {
    raw_x = ADC_Read(0);
    raw_y = ADC_Read(1);
    raw_esp1 = ADC_Read(2);
    raw_esp2 = ADC_Read(3);
    
    int center_1 = 512;
    
    int center_2 = 325; // 650 / 2
    double scale_factor = 512.0 / 325.0; 

    int deadzone = 30; // 死区

    //Mapping and Combining Signals
    
    double map_x1 = raw_x - center_1;
    double map_y1 = raw_y - center_1;

    double map_x2 = (raw_esp1 - center_2 - 15) * scale_factor;
    double map_y2 = (raw_esp2 - center_2 - 15) * scale_factor;

    double final_map_x = map_x1 + map_x2;
    double final_map_y = map_y1 + map_y2;

    double magnitude = sqrt(final_map_x * final_map_x + final_map_y * final_map_y);
    
    if (magnitude < deadzone) {
         // Tentacle_Move(0, 0); 
    } else {
        double angle_rad = atan2(final_map_y, final_map_x);
        double angle_deg = angle_rad * 180.0 / PI;
        if (angle_deg < 0) angle_deg += 360.0;
       
        bending_factor = BENDING_AMP * magnitude / 512.0;
        
        if (bending_factor > 1.0) bending_factor = 1.0;
        
        Tentacle_Move(angle_deg, bending_factor);
    }
}



int main(void) {
    //char buf[50];
    angry_mode = 0;
    UART_init(BAUD_PRESCALER);
    char buffer[30];

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

     while (1) {

        SpeedLevel lvl = ultrasonic_get_speed_nonblocking();



        switch (lvl) {
            case SPEED_FAST:
                angry_frame();
                set_emoji_state(0,0,1,0);
                angry_mode = 1;
                break;
            case SPEED_SLOW:
                happy_frame();
                set_emoji_state(1,0,0,0);
                break;

            case SPEED_NONE:
                smile_frame();
                set_emoji_state(0,1,0,0);
                angry_mode = 0;
                break;

            case SPEED_BACK:
                sad_frame();
                set_emoji_state(0,0,0,1);
                break;
        }
    }
    return 0;
}
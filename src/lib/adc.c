/*
 * Target: ATmega328PB
 * Description: Interrupt-driven Joystick Control (Clean Version, No UART)
 * Frequency: 16MHz
 */

//#define F_CPU 16000000UL 

#include <avr/io.h>
#include <avr/interrupt.h> 
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include "./lib/PCA9685.h"
#include "./lib/tentacle.h"
#include "./lib/adc.h"
#include "./lib/uart.h"

#define PI 3.14159265
#define BENDING_AMP 1.0

int raw_x = 0;
int raw_y = 0;
int raw_esp1 = 0;
int raw_esp2 = 0;

// --- ADC 初始化 ---
void ADC_Init() {
    #ifdef PRR0
    PRR0 &= ~(1 << PRADC);
    #elif defined(PRR)
    PRR &= ~(1 << PRADC);
    #endif
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// --- 读取 ADC ---
uint16_t ADC_Read(uint8_t channel) {
    ADMUX &= 0xF0;
    ADMUX |= (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    uint16_t timeout = 5000; 
    while ((ADCSRA & (1 << ADSC)) && (--timeout));
    if (timeout == 0) return 512;
    return ADC;
}

// --- Timer1 初始化 (50Hz 控制回路) ---
void Timer1_Init(void) {
    // F_CPU = 16M, Prescaler = 64 => 250kHz (4us/tick)
    // 20ms / 4us = 5000 ticks
    // OCR1A = 4999
    
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC Mode, Prescaler 64
    OCR1A = 4999;
    TIMSK1 |= (1 << OCIE1A); // Enable Compare Match A Interrupt
}


ISR(TIMER1_COMPA_vect) {
    raw_x = ADC_Read(0);
    raw_y = ADC_Read(1);
    raw_esp1 = ADC_Read(2);
    raw_esp2 = ADC_Read(3);
    
    int center_1 = 512;
    
    int center_2 = 325; // 650 / 2
    double scale_factor = 512.0 / 325.0; 

    int deadzone = 50; // 死区

    //Mapping and Combining Signals
    
    double map_x1 = raw_x - center_1;
    double map_y1 = raw_y - center_1;

    double map_x2 = (raw_esp1 - center_2 - 15) * scale_factor;
    double map_y2 = (raw_esp2 - center_2 - 15) * scale_factor;

    double final_map_x = map_x1 + map_x2;
    double final_map_y = map_y1 + map_y2;

    double magnitude = sqrt(final_map_x * final_map_x + final_map_y * final_map_y);
    
    if (magnitude < deadzone) {
         //Tentacle_Move(30, 0.8); 

    } else {
        double angle_rad = atan2(final_map_y, final_map_x);
        double angle_deg = angle_rad * 180.0 / PI;
        if (angle_deg < 0) angle_deg += 360.0;
       
        bending_factor = BENDING_AMP * magnitude / 512.0;
        
        if (bending_factor > 1.0) bending_factor = 1.0;
        
        Tentacle_Move(angle_deg, bending_factor);
    }
}



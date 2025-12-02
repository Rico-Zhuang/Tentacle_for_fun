/*
 * Target: ATmega328PB
 * Description: Interrupt-driven Joystick Control (Clean Version, No UART)
 * Frequency: 16MHz
 */

#define F_CPU 16000000UL 

#include <avr/io.h>
#include <avr/interrupt.h> 
#include <util/delay.h>
#include <math.h>
#include "./lib/PCA9685.h"
#include "./lib/tentacle.h"
#include "./lib/adc.h"

#define PI 3.14159265

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


#include <avr/io.h>
#include <util/delay.h>
#include "motor.h"

//目前只有一个servo
void servo_init() {
    MOTOR_1_DDR |= (1 << MOTOR_1_PIN);

    // 2. Configure Timer1 for Fast PWM (Mode 14)
    // In Mode 14, ICR1 determines the frequency (TOP), OCR1A determines duty cycle.
    
    // TCCR1A: Clear OC1A on Compare Match (Non-inverting mode) | WGM11
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    
    // TCCR1B: WGM13 | WGM12 | Prescaler 8 (CS11)
    // Prescaler 8 gives us 0.5us per tick at 16MHz.
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    // 3. Set PWM Frequency to 50Hz
    // Period = 20ms = 20,000us.
    // Ticks = 20,000us / 0.5us = 40,000 ticks.
    // Since counter assumes 0-index, we set TOP to 39999.
    ICR1 = 39999;
}

void servo_write(uint8_t angle) {
    if (angle > 180) angle = 180;
    
    // Map angle (0-180) to pulse width (SERVO_MIN - SERVO_MAX)
    // Formula: output = (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
    uint16_t pulse_width = ((uint32_t)angle * (SERVO_MAX - SERVO_MIN) / 180) + SERVO_MIN;
    
    OCR1A = pulse_width;
}
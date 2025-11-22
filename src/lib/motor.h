#include <avr/io.h>
#include <util/delay.h>

#ifndef MOTOR_H_
#define MOTOR_H_

#define MOTOR_1_DDR DDRB
#define MOTOR_1_PIN PB1

// Servo Pulse Width Definitions (in Timer Ticks)
// Calculation: (Target Time / Prescaler Time)
// With 16MHz clock and Prescaler 8: 1 Tick = 0.5us
// Min (0 deg)   = 1000us / 0.5us = 2000 ticks  (Adjust as needed, e.g., 1088 for Arduino standard)
// Max (180 deg) = 2000us / 0.5us = 4000 ticks  (Adjust as needed, e.g., 4800 for Arduino standard)
#define SERVO_MIN 1088 // ~544us (Arduino Standard Min)
#define SERVO_MAX 4800 // ~2400us (Arduino Standard Max)

void servo_init();
void servo_write(uint8_t angle);

#endif /* MOTOR_H_ */
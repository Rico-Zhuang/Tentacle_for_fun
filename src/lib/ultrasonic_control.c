#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h" 

#define TRIG_PIN   PD2
#define ECHO_PIN   PD3

#define BAUD 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD * 16UL))) - 1)

// #define SERVO_MIN 1088 
// #define SERVO_MAX 4800 

// void servo_init() {
//     // 1. Set PB1 (OC1A) as Output
//     DDRB |= (1 << PB1);

//     // 2. Configure Timer1 for Fast PWM (Mode 14)
//     TCCR1A = (1 << COM1A1) | (1 << WGM11);
    
//     // TCCR1B: WGM13 | WGM12 | Prescaler 8 (CS11)
//     TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

//     // 3. Set PWM Frequency to 50Hz
//     ICR1 = 39999;
// }

// void servo_write(uint8_t angle) {
//     if (angle > 180) angle = 180;
    
//     uint16_t pulse_width = ((uint32_t)angle * (SERVO_MAX - SERVO_MIN) / 180) + SERVO_MIN;
    
//     OCR1A = pulse_width;
// }


void trig_pulse_10us(void)
{
    DDRD |= (1 << TRIG_PIN);
    PORTD &= ~(1 << TRIG_PIN);

    PORTD |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTD &= ~(1 << TRIG_PIN);
}

int main(void)
{
    DDRD &= ~(1 << ECHO_PIN);     // Echo input
    UART_init(BAUD_PRESCALER);    // initialize UART
    UART_putstring("Ultrasonic Distance Measurement\r\n");

    char buf[64];

    servo_init(); // Timer1 现在只负责 PWM，不会被干扰

    // 确保 Timer3 处于初始状态
    TCCR3A = 0;
    TCCR3B = 0;

    while (1)
    {
        
        while (PIND & (1 << ECHO_PIN));

        trig_pulse_10us();   
       
        while (!(PIND & (1 << ECHO_PIN)));
        
        // --- 修改开始：使用 Timer3 进行计时，不碰 Timer1 ---
        TCNT3 = 0;              // 清零 Timer3
        TCCR3B = (1 << CS30);   // 开启 Timer3 (Prescaler 1，与你原代码逻辑一致)
        
        
        while (PIND & (1 << ECHO_PIN));
        TCCR3B = 0;             // 停止 Timer3

        uint16_t ticks = TCNT3; // 读取 Timer3 的值
        // --- 修改结束 ---

        double time_us = ticks * 0.0625;      
        double distance_cm = time_us / 58.0;  

        servo_write((uint8_t)(distance_cm *3 > 180 ? 180 : distance_cm *3));

        sprintf(buf, "Echo: %.1f us  Distance: %.2f cm\r\n", time_us, distance_cm);
        UART_putstring(buf);

        _delay_ms(60);  
    }
}
#define F_CPU 16000000UL // 必须定义在 <util/delay.h> 之前

//#include <xc.h>
#include <stdint.h>
#include <math.h>
#include <util/delay.h> // 必须包含此头文件才能使用延时函数
#include <avr/io.h>

#include "PCA9685.h"

// --- I2C (TWI0) 底层函数 ---
void I2C_Init(void) {
    // 设置波特率: SCL = F_CPU / (16 + 2*TWBR*Prescaler)
    // F_CPU=16M, SCL=100k
    TWBR0 = 72; 
    TWSR0 = 0x00; 
    TWCR0 = (1 << TWEN); 
}

void I2C_Start(void) {
    TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR0 & (1 << TWINT))); 
}

void I2C_Stop(void) {
    TWCR0 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void I2C_Write(uint8_t data) {
    TWDR0 = data;
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR0 & (1 << TWINT))); 
}

// --- PCA9685 驱动函数 ---
void PCA9685_WriteByte(uint8_t reg, uint8_t value) {
    I2C_Start();
    I2C_Write(PCA9685_ADDR << 1); 
    I2C_Write(reg);
    I2C_Write(value);
    I2C_Stop();
}

void PCA9685_Init(float freq) {
    I2C_Init();
    
    float prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= freq;
    prescaleval -= 1;
    uint8_t prescale = (uint8_t)(prescaleval + 0.5);

    PCA9685_WriteByte(MODE1, 0x10); 
    PCA9685_WriteByte(PRESCALE, prescale); 
    
    PCA9685_WriteByte(MODE1, 0x00); 
    _delay_ms(5); // 使用单下划线
    
    PCA9685_WriteByte(MODE1, 0xA0); 
}

void PCA9685_SetPWM(uint8_t channel, uint16_t on, uint16_t off) {
    I2C_Start();
    I2C_Write(PCA9685_ADDR << 1);
    I2C_Write(LED0_ON_L + 4 * channel); 
    I2C_Write(on & 0xFF);
    I2C_Write(on >> 8);
    I2C_Write(off & 0xFF);
    I2C_Write(off >> 8);
    I2C_Stop();
}

void SetServoAngle(uint8_t channel, int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    uint16_t pulse_len = SERVOMIN + (uint16_t)(((float)(SERVOMAX - SERVOMIN) / 180.0) * angle);
    
    PCA9685_SetPWM(channel, 0, pulse_len);
}


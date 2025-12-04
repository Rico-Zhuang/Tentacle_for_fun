/*
 * File:   main.c
 * Target: ATmega328PB
 * Compiler: XC8 (AVR)
 */
// ATmega328PB <-> PCA9685:

// PC5 (SCL0) -> PCA9685 SCL

// PC4 (SDA0) -> PCA9685 SDA

// GND -> PCA9685 GND

// VCC (5V) -> PCA9685 VCC (逻辑供电)

// 电源:

// PCA9685 的 V+ 端子接 独立 5V-6V 电源（用于驱动舵机）。

// 确保外部电源的 GND 与单片机的 GND 共地。

// 舵机:

// SG92R 1 -> PCA9685 通道 0

// SG92R 2 -> PCA9685 通道 1

#ifndef PCA9685_H_
#define PCA9685_H_

//#define F_CPU 16000000UL // 必须定义在 <util/delay.h> 之前

//#include <xc.h>
#include <stdint.h>
#include <math.h>
#include <util/delay.h> // 必须包含此头文件才能使用延时函数

// --- 配置位设置 ---
// 为了避免编译报错，建议使用 MPLAB X 的 "Configuration Bits" 窗口生成代码。
// 下面是通常适用的配置，如果你没有配置过 Fuse，可以直接使用默认值（通常内部8MHz/8 = 1MHz），
// 但为了让舵机正常工作，你需要确保 F_CPU 和实际时钟一致。
// 如果你使用外部 16MHz 晶振，请在 IDE 中生成配置位，或者尝试取消下面几行的注释：

/*
#pragma config SUT_CKSEL = EXTXTAL_16KCK_14CK_65MS // 外部晶振
#pragma config CKDIV8 = OFF    // 关闭 8分频 (系统全速运行)
#pragma config WDTON = OFF     // 关闭看门狗
#pragma config EESAVE = OFF    // EEPROM保留
*/

// --- PCA9685 定义 ---
#define PCA9685_ADDR    0x40 
#define MODE1           0x00
#define PRESCALE        0xFE
#define LED0_ON_L       0x06

#define SERVOMIN  102  
#define SERVOMAX  512

// --- I2C (TWI0) 底层函数 ---  
void I2C_Init(void) ;
void I2C_Start(void) ;
void I2C_Stop(void) ;
void I2C_Write(uint8_t data) ;


// --- PCA9685 驱动函数 ---
void PCA9685_WriteByte(uint8_t reg, uint8_t value);
void PCA9685_Init(float freq);
void PCA9685_SetPWM(uint8_t channel, uint16_t on, uint16_t off);
void SetServoAngle(uint8_t channel, int angle);

#endif /* PCA9685_H_ */
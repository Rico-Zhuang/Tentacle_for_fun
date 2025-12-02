#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>

// -------------------------
// Speed Levels
// -------------------------
typedef enum {
    SPEED_NONE = 0,
    SPEED_SLOW,
    SPEED_FAST,
    SPEED_BACK,
    DEFAULT
} SpeedLevel;

// -------------------------
// Public API
// -------------------------

// 初始化超声波 + Timer3 + INT1
void ultrasonic_init(void);

// 获取当前距离（cm）
float ultrasonic_get_distance(void);

// 非阻塞获取速度等级（基于距离变化）
SpeedLevel ultrasonic_get_speed_nonblocking(void);

// UART 输出 float（ultrasonic.c 内部使用）
void UART_putstring(char *str);
void UART_print_float(float x);

#endif

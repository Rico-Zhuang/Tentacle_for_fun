#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>

typedef enum {
    SPEED_NONE = 0,
    SPEED_SLOW,
    SPEED_FAST,
    SPEED_BACK,

} SpeedLevel;

void ultrasonic_init(void);

float ultrasonic_get_distance(void);

SpeedLevel ultrasonic_get_speed_nonblocking(void);

// void UART_putstring(char *str);
// void UART_print_float(float x);

#endif

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>

typedef enum {
    SPEED_NONE = 0,
    SPEED_SLOW,
    SPEED_FAST,
    SPEED_BACK,

} SpeedLevel;

float get_system_time_ms(void);

void ultrasonic_init(void);

void ultrasonic_update(void);

float ultrasonic_get_distance(void);

SpeedLevel ultrasonic_get_speed_status(void);

// void UART_putstring(char *str);
// void UART_print_float(float x);
extern uint8_t angry_mode;

#endif

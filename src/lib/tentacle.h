#include "PCA9685.h"
#include <assert.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX_ANGLE 120
#define RELAX_ANG 70
#define SAFE_LIMIT 240

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t z;
} uint8_vec3;


void Set_servos(uint8_t servoAngle0, uint8_t servoAngle1, uint8_t servoAngle2) {
    int total = (uint32_t)servoAngle0 + servoAngle1 + servoAngle2;
    if(total>SAFE_LIMIT){
        float scale = (float)SAFE_LIMIT / (float)total;
        assert(scale < 1.0);
        servoAngle0 *= scale;
        servoAngle1 *= scale;
        servoAngle2 *= scale;
    }
    SetServoAngle(0, servoAngle0);
    SetServoAngle(1, servoAngle1);
    SetServoAngle(2, servoAngle2);
}

void In_direction_0_max() {
    Set_servos(MAX_ANGLE, 0, 0);
}

void In_direction_1_max() {
    Set_servos(0, MAX_ANGLE, 0);
}

void In_direction_2_max() {
    Set_servos(0, 0, MAX_ANGLE);
}

void Sweep_0_1(uint8_t delay) {
    for(uint8_t progress=0; progress<=100; progress++){
        uint8_t s0 = (MAX_ANGLE * (100 - progress)) / 100;
        uint8_t s1 = (MAX_ANGLE * progress) / 100;
        Set_servos(s0, s1, 0);
        _delay_ms(delay);
    }
}

void Sweep_1_0(uint8_t delay) {
    for(uint8_t progress=0; progress<=100; progress++){
        uint8_t s0 = (MAX_ANGLE * (100 - progress)) / 100;
        uint8_t s1 = (MAX_ANGLE * progress) / 100;
        Set_servos(s1, s0, 0);
        _delay_ms(delay);
    }
}

void Sweep_1_2(uint8_t delay) {
    for(uint8_t progress=0; progress<=100; progress++){
        uint8_t s1 = (MAX_ANGLE * (100 - progress)) / 100;
        uint8_t s2 = (MAX_ANGLE * progress) / 100;
        Set_servos(0, s1, s2);
        _delay_ms(delay);
    }
}

void Sweep_2_0(uint8_t delay) {
    for(uint8_t progress=0; progress<=100; progress++){
        uint8_t s2 = (MAX_ANGLE * (100 - progress)) / 100;
        uint8_t s0 = (MAX_ANGLE * progress) / 100;
        Set_servos(s0, 0, s2);
        _delay_ms(delay);
    }
}

void Reset_tentacle(uint8_t angle) {
    assert(angle <= RELAX_ANG);
    Set_servos(angle, angle, angle);
}   
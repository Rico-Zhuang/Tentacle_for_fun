

#ifndef TENTACLE_H_
#define TENTACLE_H_
#define F_CPU 16000000UL 

#include <avr/io.h>
#include <util/delay.h>
#include "PCA9685.h"


#define CENTER_POS 70 // motor center position
#define MAX_BEND 80   // arm max bend angle from center



//int clip(int val, int min_val, int max_val);
void Tentacle_Move(float direction_angle, float bending_amount);

void Tentacle_Center(void);

void Motion_Breathing(float direction);
void Motion_Rotate(float bend_val, int speed_delay);
void Motion_ConicalScan(float bend_val, int speed_delay);
void Motion_SpiralOut();
void Motion_Strike(float target_angle);

#endif /* TENTACLE_H_ */
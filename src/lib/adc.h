#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "./lib/constants.h"
#include <math.h>
#include "PCA9685.h"
#include "tentacle.h"
#define PI 3.14159265

void ADC_Init();
uint16_t ADC_Read(uint8_t channel);
void Timer1_Init(void);



#endif /* ADC_H_ */

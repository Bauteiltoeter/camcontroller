#ifndef __ADC_H
#define __ADC_H

#include <stdint.h>	

void ADC_Init(void);

uint16_t ADC_Read( uint8_t channel);

int16_t axis_offset(uint8_t channel);
#endif

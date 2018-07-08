#ifndef __SENSOR_H_
#define __SENSOR_H_

#include "comm.h"

#define HAL_ADC_REF_1V25                0x00
#define ADC_12_BIT                      0x30
#define ADC_EMP_SENS                    0x0E
#define ADC_TO_CELSIUS(ADC_VALUE)       ((ADC_VALUE>>4)-334)   //ÎÂ¶ÈĞ£Õı

void InitSensor(void);
float GetTemperature(void);


#endif


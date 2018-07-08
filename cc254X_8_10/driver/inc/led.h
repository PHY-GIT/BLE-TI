#ifndef __LED_H_
#define __LED_H_

#include "comm.h"


#define LED1  P1_0       //定义P1.0口为LED1控制端
#define LED2  P1_1       //定义P1.1口为LED2控制端
#define LED3  P1_4       //定义P1.4口为LED3控制端


void led_scan(void);
void InitLed(void);
void DelayMS(uint msec);


#endif

#ifndef __LED_H_
#define __LED_H_

#include "comm.h"


#define LED1  P1_0       //����P1.0��ΪLED1���ƶ�
#define LED2  P1_1       //����P1.1��ΪLED2���ƶ�
#define LED3  P1_4       //����P1.4��ΪLED3���ƶ�


void led_scan(void);
void InitLed(void);
void DelayMS(uint msec);


#endif

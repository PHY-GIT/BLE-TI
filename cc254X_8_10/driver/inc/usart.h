#ifndef __USART_H_
#define __USART_H_

#include "comm.h"

#define UART0_RX    1       //���ձ�־
#define UART0_TX    2       //���ͱ�־  
#define SIZE        51


extern char RxBuf;
extern char UartState;
extern char RxData[SIZE]; //�洢�����ַ���




void Uart0_Init(void);
void UartSendString(char *Data, uint len);
void UartDispose(void);


#endif

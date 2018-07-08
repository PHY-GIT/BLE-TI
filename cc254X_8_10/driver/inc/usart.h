#ifndef __USART_H_
#define __USART_H_

#include "comm.h"

#define UART0_RX    1       //接收标志
#define UART0_TX    2       //发送标志  
#define SIZE        51


extern char RxBuf;
extern char UartState;
extern char RxData[SIZE]; //存储发送字符串




void Uart0_Init(void);
void UartSendString(char *Data, uint len);
void UartDispose(void);


#endif

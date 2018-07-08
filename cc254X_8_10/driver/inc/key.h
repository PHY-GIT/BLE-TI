#ifndef __KEY_H_
#define __KEY_H_

#include "comm.h"


#define KEY   P0_1        // 定义P0.1口为S1控制端  

void InitKey(void);
bool key_scan(void);

#endif

/*
* #include "otp_var.h"
*/
#ifndef __OTP_VAR_H__
#define __OTP_VAR_H__

#include "comm.h"
#include "user.h"


#ifdef  OTP_VAR_GLOBALS
#define OTP_VAR_EXT
#else
#define OTP_VAR_EXT extern
#endif


#define IAR_ROOT __root
#define IAR_CONST const __code
#define IAR_XDATA_A __root __no_init __xdata
#define IAR_PDATA_A __root __no_init __pdata
#define IAR_IDATA_A __root __no_init __idata
#define IAR_DATA_A  __root __no_init __data
#define IAR_BIT_A   __root __no_init __bit


/* 用户自定义变量区 **********************************************************/

//OTP_VAR_EXT IAR_XDATA_A u8 uasasasrt_cmd;
//OTP_VAR_EXT IAR_XDATA_A s8 rxbuf;;

//OTP_VAR_EXT IAR_XDATA_A char RxData[51]; //存储发送字符串



/* end **********************************************************************/

#endif


#ifndef __COMM_H_
#define __COMM_H_


#include <ioCC2540.h>       //主控头文件

/*****************************************************************************
 * Module    : COMMON
 * File      : comm.h
 * Function  : 常用的类型定义
 *****************************************************************************/

#define BIT(n) (1<<(n))

typedef _Bool          bool, BOOL;

typedef unsigned char uchar;    //u8
typedef unsigned int  uint;     //u16
typedef unsigned long ulong;    //u32

//typedef unsigned char  u8, U8, uint8, UINT8, BYTE;
//typedef signed char    s8, S8, int8, INT8;

//typedef unsigned short u16, U16, uint16, UINT16, WORD;
//typedef signed short   s16, S16, int16, INT16;

//typedef unsigned long  u32, U32, uint32, UINT32, DWORD;
//typedef signed long    s32, S32, int32, INT32;

//typedef unsigned long  u64, U64, uint64, UINT64;
//typedef signed long    s64, S64, int64, INT64;


//typedef unsigned short string;





#include "otp_var.h"

#endif
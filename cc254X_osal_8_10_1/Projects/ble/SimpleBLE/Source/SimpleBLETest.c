/**************************************************************************************************
  Filename:       SimpleBLETest.c
  Revised:        $Date: 2010-08-06 08:56:11 -0700 (Fri, 06 Aug 2010) $
  Revision:       $Revision: 23333 $

  Description:    This file contains the Simple BLE Peripheral sample application
                  for use with the CC2540 Bluetooth Low Energy Protocol Stack.

  Copyright 2010 - 2013 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"

#include "OnBoard.h"
#include "hal_adc.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_lcd.h"
#include "LL.h"
#include "simpleBLETest.h"
#include "npi.h"
#include "stdio.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 SimpleBLETest_TaskID;   // Task ID for internal task/event processing

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
static void NpiSerialCallback( uint8 port, uint8 events );
static void DataAesEncryptAndDecrypTest();

/*********************************************************************
 * @fn      SimpleBLETest_Init
 *
 * @brief   Initialization function for the Simple BLE Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SimpleBLETest_Init( uint8 task_id )
{ //保存任务id到全局变量   
  SimpleBLETest_TaskID = task_id;       

  // 串口初始化 波特率默认是115200, 形参是回调函数
  NPI_InitTransport(NpiSerialCallback);

  //显示字符串在某一行
  HalLcdWriteString( "SimpleBLETest 27", HAL_LCD_LINE_1 );
  HalLcdWriteString( "AmoMcu.com", HAL_LCD_LINE_2 );  

  //加密与解密测试
  DataAesEncryptAndDecrypTest();
    
  // Setup a delayed profile startup  
  /*
  设置一个任务， 这么做的目的是按照多任务处理的方法来做
  SimpleBLETest_ProcessEvent 就是处理 SBP_START_DEVICE_EVT
  */
  osal_set_event( SimpleBLETest_TaskID, SBP_START_DEVICE_EVT );
}

/*********************************************************************
 * @fn      SimpleBLETest_ProcessEvent
 *
 * @brief   Simple BLE Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
// 这个是我们的应用程序的事件处理函数 
uint16 SimpleBLETest_ProcessEvent( uint8 task_id, uint16 events )
{

  VOID task_id; // OSAL required parameter that isn't used in this function

  // SYS_EVENT_MSG 这是系统事件比如按键事件蓝牙读写事件处理，都会置这个事件
  if ( events & SYS_EVENT_MSG )
  {
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // 这个是我们应用程序自定义的事件，SBP_START_DEVICE_EVT 的值被定义为 0x0001， 
  // 实际上我们可以定义 16个事件， 第一的时候是以位来定义的
  // 这个 SBP_PERIODIC_EVT 就是在SimpleBLETest_Init初始化函数最后一行设置的事件
  if ( events & SBP_START_DEVICE_EVT )
  {
    HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);      // 点亮led1               

    // 返回这个， 告诉osal，这个实践你已经处理了
    return ( events ^ SBP_START_DEVICE_EVT );   
  }

  // Discard unknown events
  return 0;
}

// 串口回调函数， 下面把该回调函数里实现的功能讲解一下
/*
1, 思路:  当串口收到数据后，就会马上调用以下回调函数，在实际测试中发现，此回调
函数调用频繁， 如果你不执行NPI_ReadTransport函数进行读取， 那么这个回调函数就会
频繁地被执行，但是，你通过串口发送一段数据， 你本意是想处理这一完整一段的数据，所以，
我们在下面引入了时间的处理方法， 也即接收的数据够多或者超时，就读取一次数据， 
然后根据当前的状态决定执行，如果没有连接上，就把所有数据当做AT命令处理， 如果连接
上了，就把数据送到对端。  ---------------amomcu   2014.08.17
*/
static void NpiSerialCallback( uint8 port, uint8 events )
{
    (void)port;//加个 (void)，是未了避免编译告警，明确告诉缓冲区不用理会这个变量

    if (events & (HAL_UART_RX_TIMEOUT | HAL_UART_RX_FULL))   //串口有数据
    {
        uint8 numBytes = 0;

        numBytes = NPI_RxBufLen();           //读出串口缓冲区有多少字节
        
        if(numBytes == 0)
        {
            return;
        }
        else
        {
            //申请缓冲区buffer
            uint8 *buffer = osal_mem_alloc(numBytes);
            if(buffer)
            {
                //读取读取串口缓冲区数据，释放串口数据   
                NPI_ReadTransport(buffer,numBytes);   

                //把收到的数据发送到串口-实现回环 
                NPI_WriteTransport(buffer, numBytes);  

                //释放申请的缓冲区
                osal_mem_free(buffer);
            }
        }
    }
}

/*********************************************************************
 * @fn      bdAddr2Str
 *
 * @brief   Convert Bluetooth address to string
 *
 * @return  none
 */
char *bdAddr2Str( uint8 *pAddr )
{
#define B_ADDR_STR_LEN                        34

  uint8       i;
  char        hex[] = "0123456789ABCDEF";
  static char str[B_ADDR_STR_LEN];
  char        *pStr = str;
  
  *pStr++ = '0';
  *pStr++ = 'x';
  
  // Start from end of addr
  pAddr += B_ADDR_LEN;
  
  for ( i = B_ADDR_LEN; i > 0; i-- )
  {
    *pStr++ = hex[*--pAddr >> 4];
    *pStr++ = hex[*pAddr & 0x0F];
  }
  
  *pStr = 0;
  
  return str;
}

static void DataAesEncryptAndDecrypTest()
{
    // 加密秘钥 16个字节=128bit
    uint8 key[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

    // 需要加密的数据
    uint8 plaintextData[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

    // 加密后数据存放区
    uint8 encryptedData[16];

    // 解密后数据存放区
    uint8 deccryptedData[16];

    // 开始加密
    LL_Encrypt( key,  plaintextData, encryptedData );

    /*
    注意我们解密时需要key，加密后的数据用于进行通信， 这样，如果对方没有key
    是解码不出来的， 我们使用了16字节也即是128bit的key。

    加密的意义在于保护用户数据不被截取
    */

    // 开始解密    
    LL_EXT_Decrypt( key, encryptedData, deccryptedData );

    // 判断机密后的数据是否与原来一样
    if(osal_memcmp(plaintextData, deccryptedData, 16))
    {
        HalLcdWriteString( "Encrypt Decrypt OK", HAL_LCD_LINE_8 );    
    }
    else
    {
        HalLcdWriteString( "Encrypt Decrypt Fail", HAL_LCD_LINE_8 );    
    }
}

/*********************************************************************
*********************************************************************/

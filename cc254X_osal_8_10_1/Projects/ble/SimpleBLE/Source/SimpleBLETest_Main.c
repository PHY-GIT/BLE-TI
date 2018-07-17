/**************************************************************************************************
  Filename:       SimpleBLETest_Main.c
  Revised:        $Date: 2010-07-06 15:39:18 -0700 (Tue, 06 Jul 2010) $
  Revision:       $Revision: 22902 $

  Description:    This file contains the main and callback functions for
                  the Simple BLE Peripheral sample application.

  Copyright 2010 - 2011 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

/**************************************************************************************************
 *                                           Includes
 **************************************************************************************************/
/* Hal Drivers */
#include "hal_types.h"
#include "hal_key.h"
#include "hal_timer.h"
#include "hal_drivers.h"
#include "hal_led.h"

/* OSAL */
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "OSAL_PwrMgr.h"
#include "osal_snv.h"
#include "OnBoard.h"

/**************************************************************************************************
 * FUNCTIONS
 **************************************************************************************************/

/* This callback is triggered when a key is pressed */
void MSA_Main_KeyCallback(uint8 keys, uint8 state);

/**************************************************************************************************
 * @fn          main
 *
 * @brief       Start of application.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
// ÈÎºÎ8051µ¥Æ¬»úc³ÌĞò£¬ ¶¼ÊÇÓÉ main º¯Êı¿ªÊ¼µÄ£¬
// ÎÒÃÇÄÃµ½Ò»·İ´úÂë£¬Ê×ÏÈĞèÒªÕÒµ½mainº¯Êı
int main(void)
{
  /* Initialize hardware */
  HAL_BOARD_INIT();      //³õÊ¼»¯Ê±ÖÓÎÈ¶¨Ê±ÖÓµÈµÈ

  // Initialize board I/O
  //ÀäÆô¶¯£¬¹Ø±ÕÁËledµÆÓëÖĞ¶Ï£¬ Ò»±ß½ÓÏÂÀ´µÄ¸÷ÖÖ³õÊ¼»¯²»ÊÜ¸ÉÈÅ
  InitBoard( OB_COLD ); 

  /* Initialze the HAL driver */
  HalDriverInit();   //¸÷ÖÖÇı¶¯µÄ³õÊ¼»¯¡¢Èç°´¼ü¡¢lcd¡¢adc¡¢usb¡¢uartµÈ

  /* Initialize NV system */
  //snv ÄÚ²¿ÓÃÓÚ±£´æÅä¶ÔÊı¾İ»òÄãµÄÓÃ»§×Ô¶¨ÒåÊı¾İµÄÒ»¶Îflash£¬4kB¿Õ¼ä
  osal_snv_init(); 

  /* Initialize LL */

  /* Initialize the operating system */
  //oasl ²Ù×÷ÏµÍ³³õÊ¼»¯, °üº¬ÄÚ´æ·ÖÅä¡¢ÏûÏ¢¶ÓÁĞ¡¢¶¨Ê±Æ÷¡¢µçÔ´¹ÜÀíºÍÈÎÎñµÈ
  osal_init_system(); 

  /* Enable interrupts */
  HAL_ENABLE_INTERRUPTS();// ¿ªÆôÈ«¾ÖÖĞ¶Ï

  // Final board initialization
  InitBoard( OB_READY );      //ÉèÖÃ±êÖ¾±êÊ¾ÏµÍ³³õÊ¼»¯Íê±Ï 

  #if defined ( POWER_SAVING )
  // Èç¹ûÄãÊ¹ÄÜÁËµÍ¹¦ºÄ£¬ ¾ÍÆô¶¯µÍ¹¦ºÄÄ£Ê½£¬
    osal_pwrmgr_device( PWRMGR_BATTERY );
  #endif
/*
µÍ¹¦ºÄ²¿·Ö
1.ÈçºÎ×ÜÊÇÔÚPM1
  osal_pwrmgr_device( PWRMGR_ALWAYS_ON );
2.ÈçºÎ½øÈëPM2
  osal_pwrmgr_device( PWRMGR_BATTERY );ÔÚ¿ÕÏĞµÄÊ±ºò¾Í»á½øÈëµ½PM2Ä£Ê½
3.ÈçºÎ½øÈëPM3
  ´æÔÚÁ¬½Ó¾Í¶Ï¿ªÁ¬½Ó£¬´æÔÚ¹ã²¥¾ÍÍ£µô¹ã²¥£¬²¢È·ÈÏ×Ô¼º´´½¨µÄËùÓĞ¶¨Ê±ÈÎÎñ¶¼ÒÑ¹Ø±Õ£¬
  ÔòÏµÍ³Ó¦¸Ã¾Í»á½øÈëPM3Ä£Ê½£¬Ö»ÄÜ½øĞĞÍâ²¿ÖĞ¶Ï»½ĞÑ
*/

  /* Start OSAL */
  osal_start_system(); // No Return from here
/* osal ²Ù×÷ÏµÍ³Æô¶¯£¬Êµ¼ÊÉÏÊÇÒ»¸ö´óÑ­»·£¬Ö»ÊÇ¼ì²éÏà¶ÔÓ¦µÄ±êÖ¾Î»£¬
¾ÍÖ¸¶¨Ïà¶ÔÓ¦µÄÈÎÎñ,¿´µ½ÕâÀï£¬Í¬Ñ§ÃÇÓ¦¸ÃÍùÄÄÀï¿´ÄØ¿ ÆäÊµ£¬ÕâÒÑ¾­ÊÇ¾¡Í·ÁË£
ÄÇÃ´ÎÒÃÇµÄÓ¦ÓÃ³ÌĞòÊÇÔÚÄÄÀïĞ´µÄÄØ
ÆäÊµÊÇÔÚÉÏÃæµÄ ÉÏÃæµÄº¯Êı osal_init_system Àï¾Í³õÊ¼»¯ÁË£¬ÏÖÔÚ»Ø¹ıÍ·È¥¿´¿´
osal_init_system Õâ¸öº¯ÊıÄÚ²¿¾ÍÖªµÀÁË
*/    
  return 0;
}

/**************************************************************************************************
                                           CALL-BACKS
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/

/**************************************************************************************************
  Filename:       simpleBLEPeripheral.c
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
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

#include "gatt.h"

#include "hci.h"

#include "gapgattserver.h"
#include "gattservapp.h"
#include "devinfoservice.h"
#include "simpleGATTprofile.h"

#if defined( CC2540_MINIDK )
  #include "simplekeys.h"
#endif

#if defined ( PLUS_BROADCASTER )
  #include "peripheralBroadcaster.h"
#else
  #include "peripheral.h"
#endif

#include "gapbondmgr.h"

#include "simpleBLEPeripheral.h"

#if defined FEATURE_OAD
  #include "oad.h"
  #include "oad_target.h"
#endif

#include "npi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#if (HAL_UART ==TRUE)
#include "npi.h"
#include "stdio.h"
#define PRINTF_DEBUG              1         //���ڵ��Դ�ӡP03
#else
#define PRINTF_DEBUG              0
#endif

#define  USER_RSSI_EN             1          //rssi
#define  SEND_RSSI_EN             0          //���;���
#define  BAT_DET_EN               1          //�ⲿ��ؼ��

#define  USER_SW_EN               1          //�ͻ���������
#define  USER_LED_EN              1          //�ͻ�led

#if USER_SW_EN
//��Э��
#define KEY_SET_SW    0x41   //���ð�������  
#define KEY_SW_STA    0x42   //��ѯ��������
#endif

/* led */
#define LED_PORT   P2
#define LED_BIT    BV(0)
#define LED_SEL    P2SEL
#define LED_DIR    P2DIR

#define LED_H()    {P2_0 =0;}   
#define LED_L()    {P2_0 =1;}


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// How often to perform periodic event
#define SBP_PERIODIC_EVT_PERIOD               1000  //1000ms


// What is the advertising interval when device is discoverable (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// Limited discoverable mode advertises for 30.72s, and then stops
// General discoverable mode advertises indefinitely

#if defined ( CC2540_MINIDK )
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_LIMITED
#else
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL
#endif  // defined ( CC2540_MINIDK )

#if defined (POWER_SAVING)// ע�⣬ ���������˯�߹��ܣ� led�ƾͲ������������ˣ� �������Ӽ����Ҫ��ϴ� �Ż�ʡ��
// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     40//  ���Ӽ�������ݷ������йأ� ���Ӽ��Խ�̣� ��λʱ���ھ��ܷ���Խ�������

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     400//  ���Ӽ�������ݷ������йأ� ���Ӽ��Խ�̣� ��λʱ���ھ��ܷ���Խ�������
#else
// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8//  ���Ӽ�������ݷ������йأ� ���Ӽ��Խ�̣� ��λʱ���ھ��ܷ���Խ�������

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     8//  ���Ӽ�������ݷ������йأ� ���Ӽ��Խ�̣� ��λʱ���ھ��ܷ���Խ�������
#endif


// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         TRUE

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         6

// Company Identifier: Texas Instruments Inc. (13)
#define TI_COMPANY_ID                         0x000D

#define INVALID_CONNHANDLE                    0xFFFF

// Length of bd addr as a string
#define B_ADDR_STR_LEN                        15

#if defined ( PLUS_BROADCASTER )
  #define ADV_IN_CONN_WAIT                    500 // delay 500 ms
#endif

#if 1
//��ʱ����
extern void simpleBLE_Delay_1ms(int times);   
#endif

#if BAT_DET_EN
static uint8 bat_sta=0;  //0����  1:�͵�   
#endif
#if USER_SW_EN
static uint8 key_sw_sta=1;     //0�أ�1��
static uint8 key_up_flg=1;     //500ms
#endif

//�������ã�Ĭ��0dbm
#define LL_EXT_TX_POWER_MINUS_23_DBM                   0 // -23dbm  ���� ��С
#define LL_EXT_TX_POWER_MINUS_6_DBM                    1 // -6dbm   
#define LL_EXT_TX_POWER_0_DBM                          2  // 0dbm   
#define LL_EXT_TX_POWER_4_DBM                          3  // +dbm  ���� ��� 

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 simpleBLEPeripheral_TaskID;   // Task ID for internal task/event processing

static gaprole_States_t gapProfileState = GAPROLE_INIT;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8 scanRspData[] =
{

#if 1  
    // complete name
    0x09,   // length of this data
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    'L',
    'O',
    'S',
    'T',
    '-',
    'B',
    'L',
    'E',
#else
      // complete name
  0x14,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
    0x53,   // 'S'
    0x69,   // 'i'
    0x6d,   // 'm'
    0x70,   // 'p'
    0x6c,   // 'l'
    0x65,   // 'e'
    0x42,   // 'B'
    0x4c,   // 'L'
    0x45,   // 'E'
    0x50,   // 'P'
    0x65,   // 'e'
    0x72,   // 'r'
    0x69,   // 'i'
    0x70,   // 'p'
    0x68,   // 'h'
    0x65,   // 'e'
    0x72,   // 'r'
    0x61,   // 'a'
    0x6c,   // 'l'
#endif
  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),   // 100ms
  HI_UINT16( DEFAULT_DESIRED_MIN_CONN_INTERVAL ),
  LO_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),   // 1s
  HI_UINT16( DEFAULT_DESIRED_MAX_CONN_INTERVAL ),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
static uint8 advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x03,   // length of this data
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
  LO_UINT16( SIMPLEPROFILE_SERV_UUID ),
  HI_UINT16( SIMPLEPROFILE_SERV_UUID ),

  0x09,   // length of this data
  GAP_ADTYPE_MANUFACTURER_SPECIFIC,   //�����̹㲥��
  'L',
  'O',
  'S',
  'T',
  '-',
  'B',
  'L',
  'E',
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
uint8 advertData_Ex[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
  0x0B,   // length of this data
  //�����ܹ��Զ��� 8���ֽ�
  0x00,   // �Զ���
  0x00,   // �Զ���
  0x00,   // �Զ���
  0x00,   // �Զ���
  0x00,   // �Զ���
  0x00,   // �Զ���
  0x00,   // �Զ���
  0x00,   // �Զ���
  
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
  LO_UINT16( SIMPLEPROFILE_SERV_UUID ),
  HI_UINT16( SIMPLEPROFILE_SERV_UUID ),

};


// GAP GATT Attributes
//static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "Simple BLE Peripheral";
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "LOST-BLE";

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void simpleBLEPeripheral_ProcessOSALMsg( osal_event_hdr_t *pMsg );
static void peripheralStateNotificationCB( gaprole_States_t newState );
//static void performPeriodicTask( void );
static void simpleProfileChangeCB( uint8 paramID );

#if defined( CC2540_MINIDK )
static void simpleBLEPeripheral_HandleKeys( uint8 shift, uint8 keys );
#endif

#if (defined HAL_LCD) && (HAL_LCD == TRUE)
static char *bdAddr2Str ( uint8 *pAddr );
#endif // (defined HAL_LCD) && (HAL_LCD == TRUE)



/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t simpleBLEPeripheral_PeripheralCBs =
{
  peripheralStateNotificationCB,  // Profile State Change Callbacks
  peripheralRssiReadCB,//NULL                            // When a valid RSSI is read from controller (not used by application)
};

// GAP Bond Manager Callbacks
static gapBondCBs_t simpleBLEPeripheral_BondMgrCBs =
{
  NULL,                     // Passcode callback (not used by application)
  NULL                      // Pairing / Bonding state Callback (not used by application)
};

// Simple GATT Profile Callbacks
static simpleProfileCBs_t simpleBLEPeripheral_SimpleProfileCBs =
{
  simpleProfileChangeCB    // Charactersitic value change callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
#if (HAL_UART ==TRUE)
static void NpiSerialCallback( uint8 port, uint8 events );
#endif


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleBLEPeripheral_Init
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
void SimpleBLEPeripheral_Init( uint8 task_id )
{
  simpleBLEPeripheral_TaskID = task_id;

  // Setup the GAP
  VOID GAP_SetParamValue( TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL );
  
  // Setup the GAP Peripheral Role Profile
  {
    #if 0//defined( CC2540_MINIDK )
      // For the CC2540DK-MINI keyfob, device doesn't start advertising until button is pressed
      uint8 initial_advertising_enable = FALSE;
    #else
      // For other hardware platforms, device starts advertising upon initialization
      uint8 initial_advertising_enable = TRUE;
    #endif

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16 gapRole_AdvertOffTime = 0;

    uint8 enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16 desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16 desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16 desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    GAPRole_SetParameter( GAPROLE_ADVERT_OFF_TIME, sizeof( uint16 ), &gapRole_AdvertOffTime );

    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );
    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );

    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
  }

  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );


  {
     // ����rssi ������������
     uint16 rssi_read_rate_1ms = 1000; //һ�����1��   
     GAPRole_SetParameter(GAPROLE_RSSI_READ_RATE, sizeof( uint16 ), &rssi_read_rate_1ms);
  }


  // Set advertising interval
  {
    uint16 advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_LIM_DISC_ADV_INT_MAX, advInt );
    GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, advInt );
    GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, advInt );
  }
#if 0
  // Setup the GAP Bond Manager
  {
    uint32 passkey = 0; // passkey "000000"
    uint8 pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8 mitm = TRUE;
    uint8 ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    uint8 bonding = TRUE;
    GAPBondMgr_SetParameter( GAPBOND_DEFAULT_PASSCODE, sizeof ( uint32 ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_BONDING_ENABLED, sizeof ( uint8 ), &bonding );
  }
#endif

  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );            // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES );    // GATT attributes
  DevInfo_AddService();                           // Device Information Service
  SimpleProfile_AddService( GATT_ALL_SERVICES );  // Simple GATT Profile
#if defined FEATURE_OAD
  VOID OADTarget_AddService();                    // OAD Profile
#endif

  // Setup the SimpleProfile Characteristic Values
  {
    uint8 charValue1 = 1;
    uint8 charValue2 = 2;
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR1, sizeof ( uint8 ), &charValue1 );
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR2, sizeof ( uint8 ), &charValue2 );
  }


#if (defined HAL_LCD) && (HAL_LCD == TRUE)
  HalLcdWriteString( "BLE Peripheral", HAL_LCD_LINE_1 );
#endif // FEATURE_OAD

  // Register callback with SimpleGATTprofile
  VOID SimpleProfile_RegisterAppCBs( &simpleBLEPeripheral_SimpleProfileCBs );

  // Enable clock divide on halt
  // This reduces active current while radio is active and CC254x MCU
  // is halted
  HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT );

#if (HAL_UART ==TRUE)    //���ڳ�ʼ��
  // ���ڳ�ʼ�� ������Ĭ����115200, �β��ǻص�����
  NPI_InitTransport(NpiSerialCallback);

  // ���ַ������
  NPI_PrintString("BT_LOST INIT\r\n");  
#endif
// Register for all key events - This app will handle all key events
  RegisterForKeys( simpleBLEPeripheral_TaskID );
  
  // Setup a delayed profile startup
  osal_set_event( simpleBLEPeripheral_TaskID, SBP_START_DEVICE_EVT );

#if 0//   //�޸ķ��书��
	HCI_EXT_SetTxPowerCmd(LL_EXT_TX_POWER_4_DBM);
#endif
#if USER_LED_EN
    LED_SEL &= ~(LED_BIT);
    LED_DIR |= (LED_BIT);
#endif
}



/*********************************************************************
 * @fn      SimpleBLEPeripheral_ProcessEvent
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
uint16 SimpleBLEPeripheral_ProcessEvent( uint8 task_id, uint16 events )
{

  VOID task_id; // OSAL required parameter that isn't used in this function

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = osal_msg_receive( simpleBLEPeripheral_TaskID )) != NULL )
    {
      simpleBLEPeripheral_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      // Release the OSAL message
      VOID osal_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & SBP_START_DEVICE_EVT )
  {
        
    // Start the Device
    VOID GAPRole_StartDevice( &simpleBLEPeripheral_PeripheralCBs );

    // Start Bond Manager
    VOID GAPBondMgr_Register( &simpleBLEPeripheral_BondMgrCBs );

    // Set timer for first periodic event
    osal_start_timerEx( simpleBLEPeripheral_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );

#if USER_SW_EN
     osal_start_timerEx( simpleBLEPeripheral_TaskID, SBP_KEY_UP_EVT, 500 );
#endif

    return ( events ^ SBP_START_DEVICE_EVT );
  }

#if USER_SW_EN
  if ( events & SBP_KEY_UP_EVT )
  {  
        key_up_flg =1; //500ms
        osal_start_timerEx( simpleBLEPeripheral_TaskID, SBP_KEY_UP_EVT, 500 );
        return (events ^ SBP_KEY_UP_EVT);
  }
#endif

  if ( events & SBP_PERIODIC_EVT )
  {
     
#if BAT_DET_EN
     uint8 adc;

     HalAdcSetReference( HAL_ADC_REF_AVDD );
     adc = HalAdcRead( HAL_ADC_CHANNEL_7, HAL_ADC_RESOLUTION_8 );  //1s
     
     task_battery_check(adc);
#endif
    
    // Restart timer
    if ( SBP_PERIODIC_EVT_PERIOD )
    {
    
      osal_start_timerEx( simpleBLEPeripheral_TaskID, SBP_PERIODIC_EVT, SBP_PERIODIC_EVT_PERIOD );
    }

    // Perform periodic application task
    //performPeriodicTask();
    //if(gapProfileState == GAPROLE_CONNECTED)
    //{
    //    pktBuffer[1] =adc;
    //    bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
    //}
    
    return (events ^ SBP_PERIODIC_EVT);
  }

  // Discard unknown events
  return 0;
}

#if BAT_DET_EN
void task_battery_check(uint8 bat_vol)
{
    uint8 pktBuffer[3];
    static uint8 bat_cnt=0;
    static uint16 warning_cnt=0;
    pktBuffer[0] = 0x00;pktBuffer[1] = 0x00;pktBuffer[2] = 0x00;

    if((bat_vol <= 0x33)){   //2.9v
        if(bat_cnt < 5){
            bat_cnt++;
        }else if(bat_cnt == 5){   //5s
            bat_cnt = 10;
            warning_cnt =0;
            if(gapProfileState == GAPROLE_CONNECTED)
            {
                pktBuffer[0] = 0x50;
                pktBuffer[1] = 0x01;
                pktBuffer[2] = 0x51;
                bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
                bat_sta =1;   //�͵�
            }
        }
    }else{
        bat_sta =0;
        bat_cnt =0;
        warning_cnt =0;
    }


    if((++warning_cnt >=300)&&(bat_sta ==1)){   //5����һ��
        pktBuffer[0] = 0x50;
        pktBuffer[1] = 0x01;
        pktBuffer[2] = 0x51;
        if(gapProfileState == GAPROLE_CONNECTED)
        {
            bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
        }
        warning_cnt =0;
    }

    
}
#endif



/*********************************************************************
 * @fn      simpleBLEPeripheral_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void simpleBLEPeripheral_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
  #if defined( CC2540_MINIDK )
    case KEY_CHANGE:
      simpleBLEPeripheral_HandleKeys( ((keyChange_t *)pMsg)->state, ((keyChange_t *)pMsg)->keys );
      break;
  #endif // #if defined( CC2540_MINIDK )

    case KEY_CHANGE:
      simpleBLEPeripheral_HandleKeys( ((keyChange_t *)pMsg)->state, ((keyChange_t *)pMsg)->keys );
      break;

  default:
    // do nothing
    break;
  }
}
uint8 gTxPower = LL_EXT_TX_POWER_0_DBM;

//static uint8 GAPRole_flag=TRUE;  
//static uint8 old_GAPRole_flag=TRUE;  

static void simpleBLEPeripheral_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter


  uint8 pktBuffer[3];

  pktBuffer[0] = 0x00;pktBuffer[1] = 0x00;pktBuffer[2] = 0x00;
    
  if(keys==0){
     LED_L();
  }else{
     LED_H();
  }  
    
#if USER_SW_EN
  if(key_sw_sta ==0)     return;  //û�д򿪿���  
  if(key_up_flg ==0)     return;  //500ms  
#endif

  //bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0])); 

  HalLcdWriteStringValue( "key = 0x", keys, 16, HAL_LCD_LINE_5 );
  if ( keys & HAL_KEY_SW_1 )
  {

  	pktBuffer[0] = 0x40;
  	pktBuffer[1] = 0x01;
  	pktBuffer[2] = 0x41;

	bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
    HalLcdWriteString( "HAL_KEY_SW_1", HAL_LCD_LINE_5 );
  }
  if ( keys & HAL_KEY_SW_2 )
  {

  	pktBuffer[0] = 0x40;
  	pktBuffer[1] = 0x02;
  	pktBuffer[2] = 0x42;


	bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
    HalLcdWriteString( "HAL_KEY_SW_2", HAL_LCD_LINE_5 );
  }
  if ( keys & HAL_KEY_SW_3 )
  {

	pktBuffer[0] = 0x40;
  	pktBuffer[1] = 0x03;
  	pktBuffer[2] = 0x43;


	bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
    HalLcdWriteString( "HAL_KEY_SW_3", HAL_LCD_LINE_5 );
  }
  if ( keys & HAL_KEY_SW_4)
  {
    pktBuffer[0] = 0x40;
  	pktBuffer[1] = 0x04;
  	pktBuffer[2] = 0x44;

	bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
    HalLcdWriteString( "HAL_KEY_SW_4", HAL_LCD_LINE_5 );
  }	  
  if ( keys & HAL_KEY_SW_5 )
  {
    pktBuffer[0] = 0x40;
  	pktBuffer[1] = 0x05;
  	pktBuffer[2] = 0x45;

	bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
    HalLcdWriteString( "HAL_KEY_SW_5", HAL_LCD_LINE_5 );
  } 
  if ( keys & HAL_KEY_SW_6 )
  {
    pktBuffer[0] = 0x40;
  	pktBuffer[1] = 0x06;
  	pktBuffer[2] = 0x46;
#if 0
    if(GAPRole_flag == TRUE)
    {
       // bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
       GAPRole_TerminateConnection();  // ��ֹ����
       simpleBLE_Delay_1ms(100);
       GAPRole_flag =FALSE;
       old_GAPRole_flag =FALSE;
       GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &GAPRole_flag );
    }else
    {
        if(old_GAPRole_flag ==FALSE){
            GAPRole_flag =TRUE;
           // old_GAPRole_flag =GAPRole_flag;
            GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &GAPRole_flag );
        }
       
       // ��ֹ���Ӻ� ��Ҫ��λ�ӻ�
       //HAL_SYSTEM_RESET();
    }

#endif
	bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
    HalLcdWriteString( "HAL_KEY_SW_6", HAL_LCD_LINE_5 );
  }
  if ( keys & HAL_KEY_SW_7 )
  {
    pktBuffer[0] = 0x40;
  	pktBuffer[1] = 0x07;
  	pktBuffer[2] = 0x47;

	bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
    HalLcdWriteString( "HAL_KEY_SW_7", HAL_LCD_LINE_5 );
  }	
  key_up_flg =0;
#if 0
  if ( keys & HAL_KEY_UP )
  { 
#if 0  
      if(gTxPower < LL_EXT_TX_POWER_4_DBM)
      {
          gTxPower++;   //�������һ��
          HCI_EXT_SetTxPowerCmd(gTxPower);
      
          HalLcdWriteStringValue( "TxPower: ", gTxPower, 10, HAL_LCD_LINE_7 );
      }
#endif
    HalLcdWriteString( "HAL_KEY_UP", HAL_LCD_LINE_6 );
  }

  if ( keys & HAL_KEY_LEFT )
  {

    if(GAPRole_flag == TRUE)
    {
       // bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
       GAPRole_TerminateConnection();  // ��ֹ����
       simpleBLE_Delay_1ms(100);
       GAPRole_flag =FALSE;
       old_GAPRole_flag =FALSE;
       GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &GAPRole_flag );
    }else
    {
        if(old_GAPRole_flag ==FALSE){
            GAPRole_flag =TRUE;
           // old_GAPRole_flag =GAPRole_flag;
            GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &GAPRole_flag );
        }
       
       // ��ֹ���Ӻ� ��Ҫ��λ�ӻ�
       //HAL_SYSTEM_RESET();
    }

  
    HalLcdWriteString( "HAL_KEY_LEFT", HAL_LCD_LINE_6 );
  }

  if ( keys & HAL_KEY_RIGHT )
  {
    HalLcdWriteString( "HAL_KEY_RIGHT", HAL_LCD_LINE_6 );
  }
  
  if ( keys & HAL_KEY_CENTER )
  {
    HalLcdWriteString( "HAL_KEY_CENTER", HAL_LCD_LINE_6 );

  }
  
  if ( keys & HAL_KEY_DOWN )
  {
#if 0  
    if(gTxPower > LL_EXT_TX_POWER_MINUS_23_DBM)
    {
        gTxPower--; //���ʽ���һ��
        HCI_EXT_SetTxPowerCmd(gTxPower);
        
        HalLcdWriteStringValue( "TxPower: ", gTxPower, 10, HAL_LCD_LINE_7 );
    } 
#endif    
    HalLcdWriteString( "HAL_KEY_DOWN", HAL_LCD_LINE_6 );
  }
 #endif 
}

/*********************************************************************
 * @fn      peripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateNotificationCB( gaprole_States_t newState )
{

  switch ( newState )
  {
    case GAPROLE_STARTED:    //��ʼ��
      {
        uint8 ownAddress[B_ADDR_LEN];
        uint8 systemId[DEVINFO_SYSTEM_ID_LEN];

        GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

        // use 6 bytes of device address for 8 bytes of system ID value
        systemId[0] = ownAddress[0];
        systemId[1] = ownAddress[1];
        systemId[2] = ownAddress[2];

        // set middle bytes to zero
        systemId[4] = 0x00;
        systemId[3] = 0x00;

        // shift three bytes up
        systemId[7] = ownAddress[5];
        systemId[6] = ownAddress[4];
        systemId[5] = ownAddress[3];

        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);

        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          // Display device address
          HalLcdWriteString( bdAddr2Str( ownAddress ),  HAL_LCD_LINE_2 );
          HalLcdWriteString( "Initialized",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    case GAPROLE_ADVERTISING:   //��棬�ȴ�����
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Advertising",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    case GAPROLE_CONNECTED:    //���ӳɹ�
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Connected",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    case GAPROLE_WAITING:      //�Ͽ�����
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Disconnected",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    case GAPROLE_WAITING_AFTER_TIMEOUT:   //��ʱ
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Timed Out",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    case GAPROLE_ERROR:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "Error",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

    default:
      {
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteString( "",  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
      }
      break;

  }

  gapProfileState = newState;

#if !defined( CC2540_MINIDK )
  VOID gapProfileState;     // added to prevent compiler warning with
                            // "CC2540 Slave" configurations
#endif


}

/*********************************************************************
 * @fn      performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets
 *          called every five seconds as a result of the SBP_PERIODIC_EVT
 *          OSAL event. In this example, the value of the third
 *          characteristic in the SimpleGATTProfile service is retrieved
 *          from the profile, and then copied into the value of the
 *          the fourth characteristic.
 *
 * @param   none
 *
 * @return  none
 */
#if 0
static void performPeriodicTask( void )
{
#if 0    
  uint8 valueToCopy;
  uint8 stat;

  // Call to retrieve the value of the third characteristic in the profile
  stat = SimpleProfile_GetParameter( SIMPLEPROFILE_CHAR3, &valueToCopy);

  if( stat == SUCCESS )
  {
    /*
     * Call to set that value of the fourth characteristic in the profile. Note
     * that if notifications of the fourth characteristic have been enabled by
     * a GATT client device, then a notification will be sent every time this
     * function is called.
     */
    SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, sizeof(uint8), &valueToCopy);
  }
#else
    static uint8 index = 0;

    index++;    // �ۼ�
    
    //��̬�ı�㲥���ݣ�1�������ۼ�1�� ��������ݼ��ܽ�������� �Ϳ��Դﵽ����Ŀ��
    advertData_Ex[4+0] = index;   
    advertData_Ex[4+1] = index;   
    advertData_Ex[4+2] = index;   
    advertData_Ex[4+3] = index;   
    advertData_Ex[4+4] = index;   
    advertData_Ex[4+5] = index;   
    advertData_Ex[4+6] = index;   
    advertData_Ex[4+7] = index;   

    // �������º����ﵽ���¹㲥���ݵ�Ŀ��
    GAP_UpdateAdvertisingData( simpleBLEPeripheral_TaskID, TRUE, sizeof( advertData_Ex ), advertData_Ex );
#endif
}
#endif
/*********************************************************************
 * @fn      simpleProfileChangeCB
 *
 * @brief   Callback from SimpleBLEProfile indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
//����
static void simpleProfileChangeCB( uint8 paramID )
{
  uint8 newChar1Value[SIMPLEPROFILE_CHAR1_LEN];
  uint8 returnBytes;
#if PRINTF_DEBUG   
  NPI_PrintString("���տ�ʼ\r\n");  //�س�
#endif

  switch( paramID )
  {
    case SIMPLEPROFILE_CHAR1:
      SimpleProfile_GetParameter( SIMPLEPROFILE_CHAR1, newChar1Value, &returnBytes );
#if PRINTF_DEBUG   //��ӡ����
       NPI_PrintValue("FFF1-len=", returnBytes, 16);
       NPI_PrintString("\n");
       NPI_PrintString("FFF1-app to bt:\n");
       
       for (uint8 i = 0;i <returnBytes;i++) {
           NPI_PrintValue("", newChar1Value[i], 16);
       }
       NPI_PrintString("\n");
#endif
      #if 0// (defined HAL_LCD) && (HAL_LCD == TRUE)
        HalLcdWriteStringValue( "Char 1:", (uint16)(newValue), 10,  HAL_LCD_LINE_3 );
      #endif
      if(returnBytes > 0)   //������
      {
        app_to_bt(newChar1Value,returnBytes);   
      }
      break;

    default:
      // should not reach here!
      break;
  }
}

#if (defined HAL_LCD) && (HAL_LCD == TRUE)
/*********************************************************************
 * @fn      bdAddr2Str
 *
 * @brief   Convert Bluetooth address to string. Only needed when
 *          LCD display is used.
 *
 * @return  none
 */
char *bdAddr2Str( uint8 *pAddr )
{
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
#endif // (defined HAL_LCD) && (HAL_LCD == TRUE)

#if (HAL_UART==TRUE)
// ���ڻص������� ����Ѹûص�������ʵ�ֵĹ��ܽ���һ��
/*
1, ˼·:  �������յ����ݺ󣬾ͻ����ϵ������»ص���������ʵ�ʲ����з��֣��˻ص�
��������Ƶ���� ����㲻ִ��NPI_ReadTransport�������ж�ȡ�� ��ô����ص������ͻ�
Ƶ���ر�ִ�У����ǣ���ͨ�����ڷ���һ�����ݣ� �㱾�����봦����һ����һ�ε����ݣ����ԣ�
����������������ʱ��Ĵ������� Ҳ�����յ����ݹ�����߳�ʱ���Ͷ�ȡһ�����ݣ� 
Ȼ����ݵ�ǰ��״̬����ִ�У����û�������ϣ��Ͱ��������ݵ���AT����� �������
���ˣ��Ͱ������͵��Զˡ�  ---------------amomcu   2014.08.17
*/
static void NpiSerialCallback( uint8 port, uint8 events )
{
    (void)port;//�Ӹ� (void)����δ�˱������澯����ȷ���߻�������������������

    if (events & (HAL_UART_RX_TIMEOUT | HAL_UART_RX_FULL))   //����������
    {
        uint8 numBytes = 0;

        numBytes = NPI_RxBufLen();           //�������ڻ������ж����ֽ�
        
        if(numBytes == 0)
        {
            return;
        }
        else
        {
            //���뻺����buffer
            uint8 *buffer = osal_mem_alloc(numBytes);
            if(buffer)
            {
                //��ȡ��ȡ���ڻ��������ݣ��ͷŴ�������   
                NPI_ReadTransport(buffer,numBytes);   

                //���յ������ݷ��͵�����-ʵ�ֻػ� 
                NPI_WriteTransport(buffer, numBytes);  

                //�ͷ�����Ļ�����
                osal_mem_free(buffer);
            }
        }
    }
}
#endif

//bt���մ���
void app_to_bt(uint8 *ptr, uint8 len) 
{
#if USER_SW_EN  
    uint8 pktBuffer[3];      //buf
    pktBuffer[0] = 0;
    pktBuffer[1] = 0;
    pktBuffer[2] = 0;
#endif
    
    switch(ptr[0]){
#if USER_SW_EN  
        case KEY_SET_SW:
            if((ptr[0]+ptr[1])==ptr[2]){
                if(ptr[1]<=1){
                    key_sw_sta =ptr[1];
                }
            }
            break;
        case KEY_SW_STA:
            if((ptr[0]+ptr[1])==ptr[2]){
                pktBuffer[0] = 0x40;
                pktBuffer[1] = key_sw_sta;
                pktBuffer[2] = (0x40+key_sw_sta);
                bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0])); 
            }
            break;
#endif        
     default:
        break;    
    }    
}


//���͸�app 
void bt_to_app(uint8 *buf,uint8 len)
{
   if(gapProfileState == GAPROLE_CONNECTED)
   {
      // NPI_PrintString("BT_LOST INIT\r\n");  
       //numBytes = sizeof(pktBuffer) / sizeof(pktBuffer[0]);
       if(len > 19){
#if PRINTF_DEBUG        
           NPI_PrintString("send len err \n");    //���ȴ���
           ble_printf("-", len, 16);
#endif
           len = 19;
       }
       simpleBLE_SendData(buf, len);
   }
   else
   {
#if PRINTF_DEBUG    
       NPI_WriteTransport("Not Connected\r\n", 15); 
#endif
   }
}


// �������ݵ�����----ʹ���Զ����CHAR2
void simpleBLE_SendData(uint8* buffer, uint8 sendBytes)
{
#if PRINTF_DEBUG   //��ӡ����
     NPI_PrintString("FFF2-bt to app:\n");
     NPI_PrintValue("FFF2-len", sendBytes, 16);
	 NPI_PrintString("\n");
     for (uint8 i = 0;i <sendBytes;i++) {
         NPI_PrintValue("", buffer[i], 16);
     }
     NPI_PrintString("\n");
#endif

    if( GAPROLE_CONNECTED == gapProfileState) // ��������
    {   
#if 0  // ֱ��ͨ�����ڷ���                    
         NPI_WriteTransport(buffer, sendBytes); 
#else  // ͨ��CHAR2 ����ֵ���ͳ�ȥ�� ���������ַ���
#if 0  // �����ٶ��� SimpleProfile_SetParameter           
        //simpleBLEChar2DoWrite2 = FALSE;
        ble_printf("����", (uint16)buffer, 16);
        SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR2,sizeof ( uint8 ), &aa );
#else  // �����ٶȿ� GATT_Notification
        static attHandleValueNoti_t pReport;
        pReport.len = sendBytes;
        pReport.handle = 0x0028;   //FFF2 
        osal_memcpy(pReport.value, buffer, sendBytes);
        GATT_Notification(0, &pReport, FALSE );            
#endif    
#endif
    }
    else
    {
#if PRINTF_DEBUG     
       NPI_WriteTransport("Not Connected\r\n", 15); 
#endif
    }
}


#if USER_RSSI_EN

static void peripheralRssiReadCB( int8 rssi )
{
    simpleBle_SetRssi(rssi);
}

static float GUA_CalcDistByRSSI(int rssi)    
{    
    uint8 A = 49;  
    float n = 3.0;  
      
    int iRssi = abs(rssi);    
    float power = (iRssi-A)/(10*n);         
    return pow(10, power);    
}

#if SEND_RSSI_EN
// �󻬶�ƽ��ֵ
#define DIST_MAX   5
int nDistbuf[DIST_MAX];
uint8 index = 0;

static int dist_filer(int dist)
{
    int i = 0;
    int sum = 0;
    int max = 0;
    int min = 1000;
    if(index == DIST_MAX)
    {
         static int index2 = 0;
         nDistbuf[index2++] = dist;
         index2 %= DIST_MAX;

         // ȥ�������Сֵ, ����ƽ��
         
         for(i =0; i< DIST_MAX; i++)
         {
            if(max < nDistbuf[i])
               max = nDistbuf[i];
            if(min > nDistbuf[i])
               min = nDistbuf[i];
            
            sum += nDistbuf[i];
         }
         return (sum-max-min)/(DIST_MAX-2);
    }
    else
    {
        nDistbuf[index++] = dist;
        return 0;
    }
}
#endif


// ����RSSI ��ϵͳ����
void simpleBle_SetRssi(int8 rssi)
{
#if SEND_RSSI_EN
    uint8 pktBuffer[4];      //buf
	uint8 dist_h=0;          //��λ   ��λcm
	uint8 dist_l=0;          //��λ   
	uint16 temp_dist =0;     //temp������ƽ��ֵ  

    pktBuffer[0] = 0x60;   //ͷ
#endif
    if(gapProfileState == GAPROLE_CONNECTED)
    {
#if (defined HAL_LCD) && (HAL_LCD == TRUE)     
        char str[32];    
#endif
        float nfDist = GUA_CalcDistByRSSI(rssi);         //ͨ���㷨���r����λΪm  
        //uint8 aa=(uint8)nfDist;
			
        uint16 nDist = (uint16)(nfDist * 100);                 //��r����ֵ�Ŵ�100������λΪcm
#if SEND_RSSI_EN			
		temp_dist=dist_filer(nDist);

		if(temp_dist >0xff){
			dist_h =(uint8)((temp_dist>>8)&0x00FF);
			dist_l =(uint8)(temp_dist&0x00FF);
		}else{
			dist_h =0;
			dist_l =temp_dist;
		}
		
   		pktBuffer[1] =dist_h; 
		pktBuffer[2] =dist_l; 
		pktBuffer[3] = 0x66;   //β
		bt_to_app(pktBuffer,sizeof(pktBuffer) / sizeof(pktBuffer[0]));
#endif
#if (defined HAL_LCD) && (HAL_LCD == TRUE) 		
      
        sprintf(str, "Rssi=%2d, %4dCM\r\n", (uint8) (-rssi), dist_filer(nDist));

        if(1)
        {
            //NPI_WriteTransport((uint8*)str, strlen(str));

		    HalLcdWriteString(str,  HAL_LCD_LINE_6 );		
	
            //���͵��Զˡ� �����ֻ�
            //qq_write((uint8*)str, strlen(str));
            // �����¼���Ȼ�����¼�����������ʱ����ʱ������ݲ����͵�����
            //osal_set_event( simpleBLETaskId, SBP_UART_EVT );     
        }
#endif
        //LCD_WRITE_STRING(str, HAL_LCD_LINE_5 );
    }  
}


#endif


/*********************************************************************
*********************************************************************/

#define  OTP_VAR_GLOBALS
#include "user.h"

#define DISABLE_ALL_INTERRUPTS() (IEN0 = IEN1 = IEN2 = 0x00)       //三个



/****************************************************************************
* 变量初始化函数
****************************************************************************/
void sys_var_init(void)
{


}


/****************************************************************************
* 初始化系统时钟
****************************************************************************/
void InitClock(void)
{   
    DISABLE_ALL_INTERRUPTS();

    CLKCONCMD &= ~0x40;              //设置系统时钟源为 32MHZ晶振
    while(CLKCONSTA & 0x40);         //等待晶振稳定 
    CLKCONCMD &= ~0x47;              //设置系统主时钟频率为 32MHZ
}

/****************************************************************************
* 系统初始化函数
****************************************************************************/
void sys_init(void)
{   
    InitClock();             //设置系统时钟源为 32MHZ晶振
	InitLed();               //设置LED灯相关IO口
	//InitKey();               //设置按键相关IO口
	//Timer1_Init();           //定时器1
	//Timer3_Init();           //定时器1
	Uart0_Init();            //串口初始化
    InitSensor();            //传感器初始化
    //sys_var_init();
}

/****************************************************************************
* 主程序入口函数
****************************************************************************/
void main(void)
{
    char i; 
    float AvgTemp;   
    char strTemp[6];

    sys_init();              //系统初始化 
    
    while(1)
    { 
        AvgTemp = 0;          
        for (i=0; i<64; i++) 
        {    
            AvgTemp += GetTemperature();              
        }
        
        AvgTemp = AvgTemp/64;             //每次累加后除 64
       
        memset(strTemp, 0, 6);
        sprintf(strTemp,"%.02f", AvgTemp);//将浮点数转成字符串
        UartSendString(strTemp, 5);       //通过串口发给电脑显示芯片温度
        DelayMS(1000);                    //延时
    } 

}

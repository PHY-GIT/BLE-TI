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
    //InitClock();             //设置系统时钟源为 32MHZ晶振
	//InitLed();               //设置LED灯相关IO口
	//InitKey();               //设置按键相关IO口
	//Timer1_Init();           //定时器1
	//Timer3_Init();           //定时器1
	//Uart0_Init();            //串口初始化
    //InitSensor();            //传感器初始化
    //InitSleepTimer();        //初始化休眠定时器
    //sys_var_init();
}


/****************************************************************************
* 主程序入口函数
****************************************************************************/
void main(void)
{
    sys_init();              //系统初始化 
    
    uchar i=0; 
	InitLed();
    LCD_Init();                      //oled 初始化 
    LCD_Fill(0x00);                  //屏全亮
   
    while(1)
    {

		//a(0,0);
    	//a(0,2);
		//a(16,2);
		//b(32,2);
        for(i=0; i<8; i++)
        {
            //LCD_P16x16Ch(i*16,0,i);  //点阵显示
            //LCD_P16x16Ch(i*16,2,i+8);
            //LCD_P16x16Ch(i*16,4,i+16);
            //LCD_P16x16Ch(i*16,6,i+24);
        } 

		LCD_Logo();
        //DelayMS(2000); 
        //LCD_CLS();   
        //LCD_P6x8Str(0,0,"12345678901234567890123");   
        //LCD_P8x16Str(20,2,"OLED DISPLAY");   
		//Draw_BMP(0, 0,0, 0,&BMP[0]);
        //LCD_P8x16Str(8,4,"TEL:18588220515"); 
        //LCD_P8x16Str(8,6,"QQ: 11940507");       
        //LCD_P6x8Str(20,7,"2014-04-08 18:18");    
        //DelayMS(2000);  
    }

}





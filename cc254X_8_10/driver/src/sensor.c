#include "sensor.h"

/****************************************************************************
* 名    称: InitSensor()
* 功    能: 传感器初始化函数
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitSensor(void)
{ 
   Temperature();       //初始化温度传器  
} 

/****************************************************************************
* 名    称: InitSensor()
* 功    能: 温度传感器初始化函数
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void Temperature(void)
{ 
   TR0=0x01;            //设置为1来连接温度传感器到SOC_ADC
   ATEST=0x01;          //使能温度传感
}

/****************************************************************************
* 名    称: GetTemperature()
* 功    能: 获取温度传感器 AD 值
* 入口参数: 无
* 出口参数: 通过计算返回实际的温度值
****************************************************************************/
float GetTemperature(void)
{ 
   uint  value; 
   uchar tmpADCCON3 = ADCCON3;
   
   ADCIF = 0;   //adc中断标志
   
   //选择1.25V为参考电压；12位分辨率；对片内温度传感器采样
   ADCCON3  = (HAL_ADC_REF_1V25 | ADC_12_BIT | ADC_EMP_SENS);            
   
   while(!ADCIF);                     //等待 AD 转换完成 
   value =  ADCL >> 2;                //ADCL 寄存器低 2 位无效，8-2=6 
   value |= (((uint)ADCH) << 6);

   ADCCON3 = tmpADCCON3;

   return ADC_TO_CELSIUS(value);                               
}


/****************************************************************************
* 名    称: TemperatureDispose()
* 功    能: 温度传器处理函数
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void TemperatureDispose(void)
{
    char i; 
    float AvgTemp;   
    char strTemp[6];

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


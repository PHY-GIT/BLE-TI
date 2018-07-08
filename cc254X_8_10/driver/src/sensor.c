#include "sensor.h"

/****************************************************************************
* 名    称: InitSensor()
* 功    能: 温度传感器初始化函数
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitSensor(void)
{ 
   TR0=0x01;                     //设置为1来连接温度传感器到SOC_ADC
   ATEST=0x01;                   //使能温度传感
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
   
   ADCIF = 0;   
   
   //选择1.25V为参考电压；12位分辨率；对片内温度传感器采样
   ADCCON3  = (HAL_ADC_REF_1V25 | ADC_12_BIT | ADC_EMP_SENS);            
   
   while(!ADCIF);                     //等待 AD 转换完成 
   value =  ADCL >> 2;                //ADCL 寄存器低 2 位无效 
   value |= (((unsigned int)ADCH) << 6);

   ADCCON3 = tmpADCCON3;

   return ADC_TO_CELSIUS(value);                               
}




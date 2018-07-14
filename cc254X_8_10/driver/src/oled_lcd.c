#include "oled_lcd.h"
#include "font_table.h"


/*********************LCD 延时1ms************************************/
void LCD_DLY_ms(uint ms)
{                         
    uint a;
    while(ms)
    {
        a=1800;
        while(a--);
        ms--;
    }
    return;
}
/*********************LCD写数据************************************/ 
void LCD_WrDat(uchar dat)     
{
    uchar i=8, temp=0;
    LCD_DC=1;        //数据    
    for(i=0;i<8;i++) //发送一个八位数据 
    {
        LCD_SCL=0;  
        
        temp = dat&0x80;
        if (temp == 0)
        {
            LCD_SDA = 0;
        }
        else
        {
            LCD_SDA = 1;
        }
        LCD_SCL=1;             
        dat<<=1;    
    }
}
/*********************LCD写命令************************************/                                        
void LCD_WrCmd(uchar cmd)
{
    uchar i=8, temp=0;
    LCD_DC=0;        //命令 
    for(i=0;i<8;i++) //发送一个八位数据 
    { 
        LCD_SCL=0; 
       
        temp = cmd&0x80;
        if (temp == 0)
        {
            LCD_SDA = 0;
        }
        else
        {
            LCD_SDA = 1;
        }
        LCD_SCL=1;
        cmd<<=1;;        
    }     
}
/****************************************************************************
* 名    称: LCD_Set_Pos()
* 功    能: LCD 设置坐标
* 入口参数: colum:列地址，page:页地址
* 出口参数: 无
****************************************************************************/
void LCD_Set_Pos(uchar colum, uchar page) 
{ 
    LCD_WrCmd(0xb0+page);                  //写页地址
    LCD_WrCmd(((colum&0xf0)>>4)|0x10);      //写列地址的高四位
    LCD_WrCmd((colum&0x0f)|0x01);           //写列地址的低四位
} 
/****************************************************************************
* 名    称: LCD_Fill()
* 功    能: LCD全屏
* 入口参数: bmp_dat:全屏数据，如0x00全关，0x00全亮
* 出口参数: 无
****************************************************************************/
void LCD_Fill(uchar bmp_dat) 
{
    uchar y,x;
    for(y=0;y<8;y++)
    {
        LCD_WrCmd(0xb0+y);
        LCD_WrCmd(0x01);
        LCD_WrCmd(0x10);
        for(x=0;x<X_WIDTH;x++)
            LCD_WrDat(bmp_dat);
    }
}
/*********************LCD复位************************************/
void LCD_CLS(void)
{
    uchar y,x;    
    for(y=0;y<8;y++)
    {
        LCD_WrCmd(0xb0+y);
        LCD_WrCmd(0x01);
        LCD_WrCmd(0x10); 
        for(x=0;x<X_WIDTH;x++)
            LCD_WrDat(0);
    }
}
/*********************LCD初始化************************************/
void LCD_Init(void)     
{  
    P1SEL &= 0x1b; //让 P1.2 P1.5 P1.6 P1.7为普通IO口           00011011
    P1DIR |= 0xe4; //把 P1.2 P1.3 1.7设置为输出                 11100100    
      
    LCD_SCL=1;
    LCD_RST=0;
    LCD_DLY_ms(50);
    LCD_RST=1;      //从上电到下面开始初始化要有足够的时间，即等待RC复位完毕   
    LCD_WrCmd(0xae);//--turn off oled panel
    LCD_WrCmd(0x00);//---set low column address
    LCD_WrCmd(0x10);//---set high column address
    LCD_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    LCD_WrCmd(0x81);//--set contrast control register
    LCD_WrCmd(0xcf); // Set SEG Output Current Brightness
    LCD_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    LCD_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    LCD_WrCmd(0xa6);//--set normal display
    LCD_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
    LCD_WrCmd(0x3f);//--1/64 duty
    LCD_WrCmd(0xd3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    LCD_WrCmd(0x00);//-not offset
    LCD_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
    LCD_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    LCD_WrCmd(0xd9);//--set pre-charge period
    LCD_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    LCD_WrCmd(0xda);//--set com pins hardware configuration
    LCD_WrCmd(0x12);
    LCD_WrCmd(0xdb);//--set vcomh
    LCD_WrCmd(0x40);//Set VCOM Deselect Level
    LCD_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    LCD_WrCmd(0x02);//
    LCD_WrCmd(0x8d);//--set Charge Pump enable/disable
    LCD_WrCmd(0x14);//--set(0x10) disable
    LCD_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    LCD_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
    LCD_WrCmd(0xaf);//--turn on oled panel
    LCD_Fill(0xff);  //初始清屏
    LCD_Set_Pos(0,0);     
} 
/***************功能描述：显示6*8一组标准ASCII字符串    显示的坐标（x,y），y为页范围0～7****************/
void LCD_P6x8Str(uchar colum, uchar page,uchar ch[])
{
    uchar c=0,i=0,j=0;      
    while (ch[j]!='\0')
    {    
        c =ch[j]-32;
        if(colum>126){colum=0;page++;}
        LCD_Set_Pos(colum,page);    
        for(i=0;i<6;i++)     
            LCD_WrDat(F6x8[c][i]);  
        colum+=6;
        j++;
    }
}
/****************************************************************************
* 名    称: LCD_P8x16Str()
* 功    能: 显示8*16一组标准ASCII字符串
* 入口参数: colum:列地址，page:页地址，ch[]:显示的字符串
* 出口参数: 无
****************************************************************************/
void LCD_P8x16Str(uchar colum, uchar page,uchar ch[])
{
    uchar c=0,i=0,j=0;
    while (ch[j]!='\0')                  //是否到最后一字符
    {    
        c =ch[j]-32;                     //这个是关键，转化计出查表
        if(colum>120){colum=0;page+=2;}  //当写完128列自动转下两页
        LCD_Set_Pos(colum,page);    
        for(i=0;i<8;i++)                 //8*16共16个byte
           LCD_WrDat(F8X16[c*16+i]);     //上面8个字节 
        LCD_Set_Pos(colum,page+1);       //写下一个8个字节，加一页
        for(i=0;i<8;i++)     
            LCD_WrDat(F8X16[c*16+i+8]);  //下面8个字节
        colum+=8;                        //光标右移8    
        j++;                             //下标+1 
    }
}
/****************************************************************************
* 名    称: LCD_P16x16Ch()
* 功    能: 显示16*16点阵
* 入口参数: colum:列地址，page:页地址，N:固定显示在数组开始的下标
* 出口参数: 无
****************************************************************************/
void LCD_P16x16Ch(uchar colum, uchar page, uchar N)
{
    uchar wm=0;
    uint adder=32*N;           		//16*16是有32个byte,*N是因为选第几个字      
    LCD_Set_Pos(colum , page);
    for(wm = 0;wm < 16;wm++)   		//上面16个byte             
    {
        LCD_WrDat(F16x16[adder]);    
        adder += 1;
    }      
    LCD_Set_Pos(colum,page + 1);    //设置一下页的光标 
    for(wm = 0;wm < 16;wm++) 		//下面16个byte         
    {
        LCD_WrDat(F16x16[adder]);
        adder += 1;
    }           
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void Draw_BMP(uchar x0, uchar y0,uchar x1, uchar y1,uchar BMP[])
{     
    uint j=0;
    uchar x,y;
    
    if(y1%8==0) y=y1/8;      
    else y=y1/8+1;
    for(y=y0;y<y1;y++)
    {
        LCD_Set_Pos(x0,y);                
        for(x=x0;x<x1;x++)
        {      
            LCD_WrDat(BMP[j++]);            
        }
    }
} 



/*--  文字:  圳  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
const char qwe[] ={
0x10,0x10,0x10,0xFE,0x10,0x10,0xFE,0x00,0x00,0xFC,0x00,0x00,0x00,0xFE,0x00,0x00,
0x08,0x08,0x04,0x47,0x24,0x18,0x07,0x00,0x00,0x1F,0x00,0x00,0x00,0x7F,0x00,0x00};
const char abc[] ={
0x10,0x61,0x06,0xE0,0x00,0x26,0x22,0x1A,0x02,0xC2,0x0A,0x12,0x32,0x06,0x02,0x00,
0x04,0xFC,0x03,0x20,0x20,0x11,0x11,0x09,0x05,0xFF,0x05,0x09,0x19,0x31,0x10,0x00};

void a(uchar colum,uchar page)
{
	uchar i;
    LCD_Set_Pos(colum,page);
    for (i = 0; i < 16; i++)
    {
        LCD_WrDat(abc[i]);
    }
	LCD_Set_Pos(colum,page+1);
	for (i = 16; i < 32; i++)
    {
        LCD_WrDat(abc[i]);
    }
}
void b(uchar colum,uchar page)
{
	uchar i;
    LCD_Set_Pos(colum,page);
    for (i = 0; i < 16; i++)
    {
        LCD_WrDat(qwe[i]);
    }
	LCD_Set_Pos(colum,page+1);
	for (i = 16; i < 32; i++)
    {
        LCD_WrDat(qwe[i]);
    }
}



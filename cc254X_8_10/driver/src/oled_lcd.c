#include "oled_lcd.h"
#include "font_table.h"


/*********************LCD ��ʱ1ms************************************/
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
/*********************LCDд����************************************/ 
void LCD_WrDat(uchar dat)     
{
    uchar i=8, temp=0;
    LCD_DC=1;        //����    
    for(i=0;i<8;i++) //����һ����λ���� 
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
/*********************LCDд����************************************/                                        
void LCD_WrCmd(uchar cmd)
{
    uchar i=8, temp=0;
    LCD_DC=0;        //���� 
    for(i=0;i<8;i++) //����һ����λ���� 
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
* ��    ��: LCD_Set_Pos()
* ��    ��: LCD ��������
* ��ڲ���: colum:�е�ַ��page:ҳ��ַ
* ���ڲ���: ��
****************************************************************************/
void LCD_Set_Pos(uchar colum, uchar page) 
{ 
    LCD_WrCmd(0xb0+page);                  //дҳ��ַ
    LCD_WrCmd(((colum&0xf0)>>4)|0x10);      //д�е�ַ�ĸ���λ
    LCD_WrCmd((colum&0x0f)|0x01);           //д�е�ַ�ĵ���λ
} 
/****************************************************************************
* ��    ��: LCD_Fill()
* ��    ��: LCDȫ��
* ��ڲ���: bmp_dat:ȫ�����ݣ���0x00ȫ�أ�0x00ȫ��
* ���ڲ���: ��
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
/*********************LCD��λ************************************/
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
/*********************LCD��ʼ��************************************/
void LCD_Init(void)     
{  
    P1SEL &= 0x1b; //�� P1.2 P1.5 P1.6 P1.7Ϊ��ͨIO��           00011011
    P1DIR |= 0xe4; //�� P1.2 P1.3 1.7����Ϊ���                 11100100    
      
    LCD_SCL=1;
    LCD_RST=0;
    LCD_DLY_ms(50);
    LCD_RST=1;      //���ϵ絽���濪ʼ��ʼ��Ҫ���㹻��ʱ�䣬���ȴ�RC��λ���   
    LCD_WrCmd(0xae);//--turn off oled panel
    LCD_WrCmd(0x00);//---set low column address
    LCD_WrCmd(0x10);//---set high column address
    LCD_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    LCD_WrCmd(0x81);//--set contrast control register
    LCD_WrCmd(0xcf); // Set SEG Output Current Brightness
    LCD_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    LCD_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
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
    LCD_Fill(0xff);  //��ʼ����
    LCD_Set_Pos(0,0);     
} 
/***************������������ʾ6*8һ���׼ASCII�ַ���    ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
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
* ��    ��: LCD_P8x16Str()
* ��    ��: ��ʾ8*16һ���׼ASCII�ַ���
* ��ڲ���: colum:�е�ַ��page:ҳ��ַ��ch[]:��ʾ���ַ���
* ���ڲ���: ��
****************************************************************************/
void LCD_P8x16Str(uchar colum, uchar page,uchar ch[])
{
    uchar c=0,i=0,j=0;
    while (ch[j]!='\0')                  //�Ƿ����һ�ַ�
    {    
        c =ch[j]-32;                     //����ǹؼ���ת���Ƴ����
        if(colum>120){colum=0;page+=2;}  //��д��128���Զ�ת����ҳ
        LCD_Set_Pos(colum,page);    
        for(i=0;i<8;i++)                 //8*16��16��byte
           LCD_WrDat(F8X16[c*16+i]);     //����8���ֽ� 
        LCD_Set_Pos(colum,page+1);       //д��һ��8���ֽڣ���һҳ
        for(i=0;i<8;i++)     
            LCD_WrDat(F8X16[c*16+i+8]);  //����8���ֽ�
        colum+=8;                        //�������8    
        j++;                             //�±�+1 
    }
}
/****************************************************************************
* ��    ��: LCD_P16x16Ch()
* ��    ��: ��ʾ16*16����
* ��ڲ���: colum:�е�ַ��page:ҳ��ַ��N:�̶���ʾ�����鿪ʼ���±�
* ���ڲ���: ��
****************************************************************************/
void LCD_P16x16Ch(uchar colum, uchar page, uchar N)
{
    uchar wm=0;
    uint adder=32*N;           		//16*16����32��byte,*N����Ϊѡ�ڼ�����      
    LCD_Set_Pos(colum , page);
    for(wm = 0;wm < 16;wm++)   		//����16��byte             
    {
        LCD_WrDat(F16x16[adder]);    
        adder += 1;
    }      
    LCD_Set_Pos(colum,page + 1);    //����һ��ҳ�Ĺ�� 
    for(wm = 0;wm < 16;wm++) 		//����16��byte         
    {
        LCD_WrDat(F16x16[adder]);
        adder += 1;
    }           
}
/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
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



/*--  ����:  ��  --*/
/*--  ����12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
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



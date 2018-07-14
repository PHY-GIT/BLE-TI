#ifndef _OLED_LCD_H_
#define _OLED_LCD_H_

#include "comm.h"

//io_set
#define LCD_SCL P1_5       //SCLK  ʱ�� D0��SCLK��
#define LCD_SDA P1_6       //SDA   D1��MOSI�� ����
#define LCD_RST P1_7       //_RES  hardware reset   ��λ 
#define LCD_DC  P1_2       //A0  H/L ��������ѡͨ�ˣ�H�����ݣ�L:����

//SSD1306
#define XLevelL        0x00
#define XLevelH        0x10
#define XLevel         ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column     128
#define Max_Row        64
#define Brightness     0xCF 
#define X_WIDTH        128
#define Y_WIDTH        64



void LCD_DLY_ms(uint ms);
void LCD_WrDat(uchar dat);
void LCD_WrCmd(uchar cmd);
void LCD_Set_Pos(uchar colum, uchar page); 
void LCD_Fill(uchar bmp_dat);
void LCD_CLS(void);
void LCD_Init(void);
void LCD_P6x8Str(uchar colum, uchar page,uchar ch[]);
void LCD_P8x16Str(uchar colum, uchar page,uchar ch[]);
void LCD_P16x16Ch(uchar colum, uchar page, uchar N);
void Draw_BMP(uchar x0, uchar y0,uchar x1, uchar y1,uchar BMP[]);

void a(uchar colum,uchar page);
void b(uchar colum,uchar page);
#endif

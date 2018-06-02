#ifndef __OLED_H
#define __OLED_H	

#include "sys.h"


//-----------------OLED端口定义----------------//  
//              GND  GND
//              VCC  3V3
//              D0   SCLK
//              D1   SDIN
//              RES  RST
//              DC   DC
//              CS   CS 
/****************时钟*********************/
#define OLED_SCLK_PORT  (GPIOC)
#define OLED_SCLK_PIN  (GPIO_Pin_9)

/****************数据*********************/
#define OLED_SDIN_PORT  (GPIOC)
#define OLED_SDIN_PIN  (GPIO_Pin_8)

/****************复位*********************/
#define OLED_RST_PORT  (GPIOC)
#define OLED_RST_PIN  (GPIO_Pin_7)

/****************数据/命令*********************/
#define OLED_DC_PORT  (GPIOC)
#define OLED_DC_PIN  (GPIO_Pin_6)

/****************片选*********************/
#define OLED_CS_PORT  (GPIOB)
#define OLED_CS_PIN  (GPIO_Pin_0)

#define OLED_SCLK_Clr()  GPIO_ResetBits(OLED_SCLK_PORT, OLED_SCLK_PIN)
#define OLED_SCLK_Set()  GPIO_SetBits(OLED_SCLK_PORT, OLED_SCLK_PIN)

#define OLED_SDIN_Clr() GPIO_ResetBits(OLED_SDIN_PORT, OLED_SDIN_PIN)
#define OLED_SDIN_Set() GPIO_SetBits(OLED_SDIN_PORT, OLED_SDIN_PIN)

#define OLED_RST_Clr() GPIO_ResetBits(OLED_RST_PORT, OLED_RST_PIN)
#define OLED_RST_Set() GPIO_SetBits(OLED_RST_PORT, OLED_RST_PIN)

#define OLED_DC_Clr() GPIO_ResetBits(OLED_DC_PORT, OLED_DC_PIN)
#define OLED_DC_Set() GPIO_SetBits(OLED_DC_PORT, OLED_DC_PIN)

#define OLED_CS_Clr()  GPIO_ResetBits(OLED_CS_PORT, OLED_CS_PIN)
#define OLED_CS_Set()  GPIO_SetBits(OLED_CS_PORT, OLED_CS_PIN)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
					  
//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);
void OLED_ShowString(u8 x,u8 y, u8 *p);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);

#endif  
	 




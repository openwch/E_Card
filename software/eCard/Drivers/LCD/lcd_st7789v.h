/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐飞科技
 * All rights reserved.
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		IPS 2.0寸液晶
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技
 * @version    		查看doc内version文件 版本说明
 * @Software 		IAR 8.3 or MDK 5.26
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-09-17
 * @note		
 接线定义：
 ------------------------------------
 模块管脚            单片机管脚
 D0                 	C0
 D1                 	C1
 D2                 	C2
 D3                 	C3
 D4                 	C4
 D5                 	C5
 D6                 	C6
 D7                 	C7

 BL                 	B9
 RD                 	C8
 WR                 	C9
 RS                 	C10
 CS                 	C11
 TE						C12
 RST                	C13


 电源引脚
 VCC 3.3V电源
 GND 电源地
 最大分辨率240*320
 ------------------------------------
 //初始化屏幕之前，请先初始化GPIO.
 ********************************************************************************************************************/

#ifndef _LCD_ST7789V_H
#define _LCD_ST7789V_H

//#include "app_lcd.h"
#include "fonts/fonts.h"
#include <stdio.h>
#include <ch32v20x.h>
//--------------引脚定义--------------

//寄存器操作引脚.
#define GPIOAout(x,n)	(((x) == 0) ? (GPIOA->OUTDR &= ~(GPIO_Pin_0<<(n))) : (GPIOA->OUTDR |= (GPIO_Pin_0<<(n))))
#define GPIOBout(x,n)	(((x) == 0) ? (GPIOB->OUTDR &= ~(GPIO_Pin_0<<(n))) : (GPIOB->OUTDR |= (GPIO_Pin_0<<(n))))
#define GPIOCout(x,n)	(((x) == 0) ? (GPIOC->OUTDR &= ~(GPIO_Pin_0<<(n))) : (GPIOC->OUTDR |= (GPIO_Pin_0<<(n))))
#define GPIODout(x,n)	(((x) == 0) ? (GPIOD->OUTDR &= ~(GPIO_Pin_0<<(n))) : (GPIOD->OUTDR |= (GPIO_Pin_0<<(n))))

//命令引脚，使用之前,请先初始化引脚。
#define IPS200_RD(x)        	GPIOCout((x),(8))
#define IPS200_WR(x)        	GPIOCout((x),(9))
#define IPS200_RS(x)        	GPIOCout((x),(10))
#define IPS200_CS(x)			GPIOCout((x),(11))
#define IPS200_RST(x)  			GPIOCout((x),(13))
#define IPS200_BL(x) 			GPIOBout((x),(9))

#define IPS200_RD_0        	    (GPIOC->OUTDR &= ~(GPIO_Pin_8))
#define IPS200_WR_0        	    (GPIOC->OUTDR &= ~(GPIO_Pin_9))
#define IPS200_RS_0        	    (GPIOC->OUTDR &= ~(GPIO_Pin_10))
#define IPS200_CS_0			    (GPIOC->OUTDR &= ~(GPIO_Pin_11))
#define IPS200_RST_0 			(GPIOC->OUTDR &= ~(GPIO_Pin_13))
#define IPS200_BL_0 			(GPIOBout->OUTDR &= ~(GPIO_Pin_9))

#define IPS200_WR_CS_0          (GPIOC->OUTDR &= ~(GPIO_Pin_11|GPIO_Pin_9))


#define IPS200_RD_1        	    (GPIOC->OUTDR |= (GPIO_Pin_8))
#define IPS200_WR_1        	    (GPIOC->OUTDR |= (GPIO_Pin_9))
#define IPS200_RS_1        	    (GPIOC->OUTDR |= (GPIO_Pin_10))
#define IPS200_CS_1			    (GPIOC->OUTDR |= (GPIO_Pin_11))
#define IPS200_RST_1  			(GPIOC->OUTDR |= (GPIO_Pin_13))
#define IPS200_BL_1 			(GPIOB->OUTDR |= (GPIO_Pin_9))
//8个数据引脚必须连续 例如B0-B7,B6-B13等等。
//--------------数据端口寄存器--------------
#define IPS200_DATAPORT     	GPIOC->OUTDR

//--------------数据端口起始地址偏移--------------
#define DATA_START_NUM			0

//-------常用颜色在FONT.h文件中定义----------
//#define RED          	0xF800	//红色
//#define BLUE         	0x001F  //蓝色
//#define YELLOW       	0xFFE0	//黄色
//#define GREEN        	0x07E0	//绿色
//#define WHITE        	0xFFFF	//白色
//#define BLACK        	0x0000	//黑色
//#define GRAY  		0X8430 	//灰色
//#define BROWN 		0XBC40 	//棕色
//#define PURPLE    	0XF81F	//紫色
//#define PINK    		0XFE19	//粉色


//定义写字笔的颜色
#define IPS200_PENCOLOR         BLACK

//定义背景颜色
#define IPS200_BGCOLOR          0x001F


#define IPS200_W                240	    	
#define IPS200_H                320	   		


//--------------定义显示方向--------------
//0 竖屏模式
//1 竖屏模式  旋转180°
//2 横屏模式
//3 横屏模式  旋转180°
#define IPS200_DISPLAY_DIR 		0

#if (0==IPS200_DISPLAY_DIR || 1==IPS200_DISPLAY_DIR)
#define	IPS200_X_MAX			IPS200_W	//液晶X方宽度
#define IPS200_Y_MAX			IPS200_H   //液晶Y方宽度
     
#elif (2==IPS200_DISPLAY_DIR || 3==IPS200_DISPLAY_DIR)
#define	IPS200_X_MAX			IPS200_H	//液晶X方宽度
#define IPS200_Y_MAX			IPS200_W   //液晶Y方宽度
     
#else
#error "IPS200_DISPLAY_DIR 定义错误"
     
#endif

extern uint16 g_BGColor;

void st7789v_gpio_config(void);
void st7789v_init(void); 
//void st7789v_w_data(uint8 dat);
void st7789v_wr_reg(uint16 com);
void st7789v_wr_data(uint8 dat);
void st7789v_wrcp_data16(uint16 dat);
void st7789v_wr_data16(uint16 dat);
void st7789v_w_reg(uint8 com,uint8 dat);
void st7789v_address_set(uint16 x1,uint16 y1,uint16 x2,uint16 y2);
void st7789v_clear(uint16 color);
void st7789v_drawpoint(uint16 x,uint16 y,uint16 color);
void st7789v_showchar(uint16 x,uint16 y,uint8 dat);
void st7789v_showstr(uint16 x,uint16 y,uint8 dat[]);
void st7789v_showdatenum(uint16 x, uint8 dat);
void st7789v_showbattnum(uint16 x, uint8 dat);
void st7789v_showdevicenamenum(uint16 x, uint8 dat);

void st7789v_showint8(uint16 x,uint16 y,int8 dat);
void st7789v_showuint8(uint16 x,uint16 y,uint8 dat);
void st7789v_showint16(uint16 x,uint16 y,int16 dat);
void st7789v_showuint16(uint16 x,uint16 y,uint16 dat);
void st7789v_showimage(uint16 x,uint16 y,uint16 w,uint16 l,const unsigned char *p);
void st7789v_display_chinese(uint16 x, uint16 y, uint8 size, const uint8 *p, uint8 number, uint16 color);
void st7789v_display_chinese_Hcent(uint16 y, uint8 size, const uint8 *p, uint8 number, uint16 color);

void st7789v_showstr_Hcent(uint16 y, uint8 dat[ ]);
void st7789v_showDeviceName_Hcent(uint16 y, uint8 dat[]);

static inline void st7789v_w_16data(uint16 dat)			//写数据
{
	IPS200_DATAPORT = (dat) ;//| (IPS200_DATAPORT & ~((uint32) (0xFF))); //<< DATA_START_NUM
}
#endif


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

#include "stdlib.h"
#include "string.h"

#include "lcd_st7789v.h"
#include "Debug/debug.h"

#include "ch32v20x_gpio.h"
#include "ch32v20x_rcc.h"



uint16 g_BGColor = 0;

#define PWM_MODE1   0
#define PWM_MODE2   1

/* PWM Output Mode Selection */
//#define PWM_MODE PWM_MODE1
#define PWM_MODE PWM_MODE2

/****************************************************************************************
 * @fn      st7789v_w_data
 * @brief   写数据,内部调用，用户无需关心
 * 
 * @param   dat 
 * @return  void
 * @since   v1.0
 */
static inline void st7789v_w_data(uint8 dat)			
{
	IPS200_DATAPORT = (dat << DATA_START_NUM) | (IPS200_DATAPORT & ~((uint32) (0xFF << DATA_START_NUM)));
}

/****************************************************************************************
 * @fn      st7789v_wr_reg
 * @brief   写命令,内部调用，用户无需关心
 * 
 * @param   com 
 * @return  void
 * @since   v1.0
 */
void st7789v_wr_reg(uint16 com)			
{
	IPS200_RS( 0 );
	IPS200_RD( 1 );
	st7789v_w_data( com );
	IPS200_CS( 0 );
	IPS200_WR( 0 );
	IPS200_WR( 1 );
	IPS200_CS( 1 );
}

/****************************************************************************************
 * @brief 向液晶屏写一个8位数据,内部调用，用户无需关心
 * @param dat 
 * @return     void				
 * @since      v1.0    
 */
void st7789v_wr_data(uint8 dat)			
{
	IPS200_RS( 1 );
	IPS200_RD( 1 );
	st7789v_w_data( dat );
	IPS200_CS( 0 );
	IPS200_WR( 0 );
	IPS200_WR( 1 );
	IPS200_CS( 1 );
}

/****************************************************************************************
 * @fn st7789v_wrcp_data16
 * 
 * @brief 向液晶屏写一个16位数据内部调用,特用来传指令参数，用户无需关心
 * 
 * @param dat
 * 
 * @return  void
 * 
 * @since      v1.0
 */
void st7789v_wrcp_data16(uint16 dat)		
{
	IPS200_RS( 1 );
	IPS200_RD( 1 );
	st7789v_w_data( dat >> 8 );
	IPS200_CS( 0 );
	IPS200_WR( 0 );
	IPS200_WR( 1 );
	st7789v_w_data( dat );
	IPS200_CS( 0 );
	IPS200_WR( 0 );
	IPS200_WR( 1 );
	IPS200_CS( 1 );
}

/****************************************************************************************
 * @fn          st7789v_wrcp_data16
 * 
 * @brief       向液晶屏写一个16位数据内部调用，用户无需关心，此处改动为了多于操作浪费的时间，
 *              有效改善刷屏。
 * 
 * @param       dat 
 * 
 * @return      void
 * 
 * @since       v1.0
 */
void st7789v_wr_data16(uint16 dat)
{
	st7789v_w_16data( (1<<8)|(1<<10)|(1<<13)|(dat >> 8) );//
	IPS200_WR_1;
	st7789v_w_16data((dat&0xff)|(1<<8)|(1<<10)|(1<<13));//
	IPS200_WR_1;
}

/****************************************************************************************
 * @fn        st7789v_w_reg
 * @brief     写寄存器
 * 
 * @param     com 
 * @param     dat 
 * @return    void
 * @since     v1.0
 */
void st7789v_w_reg(uint8 com, uint8 dat)		
{
	st7789v_wr_reg( com );
	st7789v_wr_data( dat );
}

/****************************************************************************************
 *  @fn         st7789v_address_set
 *  @brief      设置屏幕数据写入的范围,内部调用，用户无需关心
 *  @param x1   范围起点横坐标
 *  @param y1   范围起点纵坐标
 *  @param x2   范围终点横坐标
 *  @param y2   范围终点纵坐标
 *  @return     void
 *  @since      v1.0
 */

void st7789v_address_set(uint16 x1, uint16 y1, uint16 x2, uint16 y2)
{
	st7789v_wr_reg( 0x2a );
    IPS200_RS_1;
	st7789v_wrcp_data16( x1 );
	st7789v_wrcp_data16( x2 );
	
	st7789v_wr_reg( 0x2b );
    IPS200_RS_1;
	st7789v_wrcp_data16( y1 );
	st7789v_wrcp_data16( y2 );
	
	st7789v_wr_reg( 0x2c );
}

/****************************************************************************************
 * @fn      BGLight_PWMOut_Init
 *
 * @brief   Initializes BL output compare.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */

void BGLight_PWMOut_Init(u16 arr, u16 psc, u16 ccp)
{

	GPIO_InitTypeDef GPIO_InitStructure = {
		0
	};
	TIM_OCInitTypeDef TIM_OCInitStructure = {
		0
	};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {
		0
	};

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM4, &TIM_TimeBaseInitStructure );

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ccp;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC4Init( TIM4, &TIM_OCInitStructure );

	TIM_CtrlPWMOutputs( TIM4, ENABLE );
	TIM_OC4PreloadConfig( TIM4, TIM_OCPreload_Disable );
	TIM_ARRPreloadConfig( TIM4, ENABLE );
	TIM_Cmd( TIM4, ENABLE );
}

/****************************************************************************************
 *  @fn         st7789v_gpio_config
 *  @brief      2.0寸 IPS液晶GPIO初始化
 *  @param      void
 *  @return     void
 *  @since      v1.0
 *  Sample usage:               
 */
void st7789v_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOC, ENABLE );	 //使能PA端口时钟,使能PE端口时钟。

	GPIO_ResetBits( GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |
					GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7| GPIO_Pin_8 | GPIO_Pin_9 |
					GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 );
	
	//数据端口初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |
					GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init( GPIOC, &GPIO_InitStructure );
	
	//命令端口初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 |
					GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init( GPIOC, &GPIO_InitStructure );

	BGLight_PWMOut_Init( 0xFF, 500-1, 0xff );

}

/****************************************************************************************
 *  @fn         st7789v_init
 *  @brief      2.0寸 IPS液晶初始化
 *  @param      void
 *  @return     void
 *  @since      v1.0             
 */
void st7789v_init(void)
{
	IPS200_BL(1);
	IPS200_RST( 0 );
	Delay_Ms( 5 );
	IPS200_RST( 1 );
	Delay_Ms( 5 );

	st7789v_wr_reg( 0x11 );
	Delay_Ms( 120 );

	st7789v_wr_reg( 0x36 );
	if (IPS200_DISPLAY_DIR == 0)
		st7789v_wr_data( 0x00 );
	else if (IPS200_DISPLAY_DIR == 1)
		st7789v_wr_data( 0xC0 );
	else if (IPS200_DISPLAY_DIR == 2)
		st7789v_wr_data( 0x70 );
	else
		st7789v_wr_data( 0xA0 );

	st7789v_wr_reg( 0x3A );
	st7789v_wr_data( 0x05 );

	st7789v_wr_reg( 0xB2 );
	st7789v_wr_data( 0x0C );
	st7789v_wr_data( 0x0C );
	st7789v_wr_data( 0x00 );
	st7789v_wr_data( 0x33 );
	st7789v_wr_data( 0x33 );

	st7789v_wr_reg( 0xB7 );
	st7789v_wr_data( 0x35 );

	st7789v_wr_reg( 0xBB );
	st7789v_wr_data( 0x29 ); //32 Vcom=1.35V

	st7789v_wr_reg( 0xC2 );
	st7789v_wr_data( 0x01 );

	st7789v_wr_reg( 0xC3 );
	st7789v_wr_data( 0x19 ); //GVDD=4.8V

	st7789v_wr_reg( 0xC4 );
	st7789v_wr_data( 0x20 ); //VDV, 0x20:0v

	st7789v_wr_reg( 0xC5 );
	st7789v_wr_data( 0x1A ); //VCOM Offset Set

	st7789v_wr_reg( 0xC6 );
	st7789v_wr_data( 0x01F ); //0x0F:60Hz

	st7789v_wr_reg( 0xD0 );
	st7789v_wr_data( 0xA4 );
	st7789v_wr_data( 0xA1 );

	st7789v_wr_reg( 0xE0 );
	st7789v_wr_data( 0xD0 );
	st7789v_wr_data( 0x08 );
	st7789v_wr_data( 0x0E );
	st7789v_wr_data( 0x09 );
	st7789v_wr_data( 0x09 );
	st7789v_wr_data( 0x05 );
	st7789v_wr_data( 0x31 );
	st7789v_wr_data( 0x33 );
	st7789v_wr_data( 0x48 );
	st7789v_wr_data( 0x17 );
	st7789v_wr_data( 0x14 );
	st7789v_wr_data( 0x15 );
	st7789v_wr_data( 0x31 );
	st7789v_wr_data( 0x34 );

	st7789v_wr_reg( 0xE1 );
	st7789v_wr_data( 0xD0 );
	st7789v_wr_data( 0x08 );
	st7789v_wr_data( 0x0E );
	st7789v_wr_data( 0x09 );
	st7789v_wr_data( 0x09 );
	st7789v_wr_data( 0x15 );
	st7789v_wr_data( 0x31 );
	st7789v_wr_data( 0x33 );
	st7789v_wr_data( 0x48 );
	st7789v_wr_data( 0x17 );
	st7789v_wr_data( 0x14 );
	st7789v_wr_data( 0x15 );
	st7789v_wr_data( 0x31 );
	st7789v_wr_data( 0x34 );
	
	st7789v_wr_reg( 0x21 );
	st7789v_wr_reg( 0x29 );

}

/****************************************************************************************
 *  @brief          液晶清屏函数
 *  @param          color     		颜色设置
 *  @return         void
 *  @since          v1.0
 *  Sample usage:   st7789v_clear(YELLOW);// 全屏设置为黄色
 */
void st7789v_clear(uint16 color)
{
    uint16 i, j;
    st7789v_address_set(0, 0, IPS200_X_MAX - 1, IPS200_Y_MAX - 1);

    IPS200_RS_1;
    IPS200_RD_1;
    for (i = 0; i < IPS200_X_MAX; i++)
    {
        for (j = 0; j < IPS200_Y_MAX; j++)
        {
            st7789v_wr_data16(color);
        }
    }
    // IPS200_CS_1;

}


/****************************************************************************************
 *  @brief      液晶画点
 *  @param      x     	        坐标x方向的起点
 *  @param      y     	        坐标y方向的起点
 *  @param      color           需要显示的颜色
 *  @return     void
 *  @since      v1.0
 *  Sample usage:               st7789v_drawpoint(0,0,RED);  //坐标0,0画一个红色的点
 */
void st7789v_drawpoint(uint16 x, uint16 y, uint16 color)
{
	st7789v_address_set( x, y, x, y );
	st7789v_wr_data16( color );
}

/****************************************************************************************
 *  @brief      液晶显示字符
 *  @param      x     	        坐标x方向的起点 参数范围 0 - (IPS200_X_MAX-1)
 *  @param      y     	        坐标y方向的起点 参数范围 0 - (IPS200_Y_MAX/16-1)
 *  @param      dat       	    需要显示的字符
 *  @return     void
 *  @since      v1.0
 *  Sample usage:               st7789v_showchar(0,0,'x');//坐标0,0写一个字符x
 */
void st7789v_showchar(uint16 x, uint16 y, uint8 dat)
{
	uint8 i, j;
	uint8 temp;

	for (i = 0; i < 16; i++)
	{
		st7789v_address_set( x, y + i, x + 7, y + i );
		temp = tft_ascii[ (uint16) dat - 32 ][ i ];	//减32因为是取模是从空格开始取得 空格在ascii中序号是32
		for (j = 0; j < 8; j++)
		{
			if (temp & 0x01)
				st7789v_wr_data16( IPS200_PENCOLOR );
			else
				st7789v_wr_data16( g_BGColor );
			temp >>= 1;
		}
	}
}
void st7789v_showdatenum(uint16 x, uint8 dat)
{
	uint16 i;
    uint16*data = NULL;
	uint8 temp;
    temp = dat-'0';
    data = (uint16*)dateNum[temp];
    st7789v_address_set(x, DRAW_DATE_TIME_COY, x - 1 + DATENUM_W, DRAW_DATE_TIME_COY - 1 + DATENUM_H);
    for (i = 0; i < DATENUM_SIZE/2; i++)
    {
        st7789v_wr_data16(*(data+i));
    }
}

void st7789v_showbattnum(uint16 x, uint8 dat)
{
	uint16 i;
    uint16*data = NULL;
	uint8 temp;
    temp = dat-'0';
    data = (uint16*)battNum[temp];
    st7789v_address_set(x, DRAW_BATTERY_COY, x - 1 + BATTERYNUM_W, DRAW_BATTERY_COY - 1 + BATTERYNUM_H);
    for (i = 0; i < BATTERYNUM_SIZE/2; i++)
    {
        st7789v_wr_data16(*(data+i));
    }
}

void st7789v_showdevicenamenum(uint16 x, uint8 dat)
{
    uint16 i;
    uint16 *data = NULL;
    uint8 temp;
    if ((dat >= '0') && (dat <= '9'))
        temp = dat - '0';
    else if ((dat >= 'a') && (dat <= 'f'))
        temp = dat - 'a' + 10;
    else if ((dat >= 'A') && (dat <= 'F'))
        temp = dat - 'A' + 10;
    else temp = 0;


    data = (uint16 *)dateNum[temp];
    st7789v_address_set(x, DEVICENAME_COY, x - 1 + DATENUM_W, DEVICENAME_COY - 1 + DATENUM_H);
    for (i = 0; i < DATENUM_SIZE / 2; i++)
    {
        st7789v_wr_data16(*(data + i));
    }
}
/****************************************************************************************
 *  @brief      液晶显示字符串
 *  @param      x     	坐标x方向的起点  参数范围 0 - (IPS200_X_MAX-1)
 *  @param      y     	坐标y方向的起点  参数范围 0 - (IPS200_Y_MAX/16-1)
 *  @param      dat     需要显示的字符串
 *  @return     void
 *  @since      v1.0
 *  Sample usage:       st7789v_showstr(0,0,"seekfree");
 */
void st7789v_showstr(uint16 x, uint16 y, uint8 dat[ ])
{
	uint16 j;
	j = 0;
	while(dat[j] != '\0')
	{
		st7789v_showchar(x+8*j,y*16,dat[j]);
		j++;
	}
}

/****************************************************************************************
 *  @brief      居中液晶显示字符串
 *  @param      y     	坐标y方向的起点  参数范围 0 - (IPS200_Y_MAX/16-1)
 *  @param      dat     需要显示的字符串
 *  @return     void
 *  @since      v1.0
 *  Sample usage:       st7789v_showstr(0,0,"seekfree");
 */
void st7789v_showstr_Hcent(uint16 y, uint8 dat[])
{
    uint16 j, x;
    j = 0;
    x = (240 - strlen((char *)dat) * 8) / 2;
    while (dat[j] != '\0')
    {
        st7789v_showchar(x + 8 * j, y * 16, dat[j]);
        j++;
    }
}

/****************************************************************************************
 *  @brief      居中液晶显示蓝牙设备名称
 *  @param      y     	        坐标y方向的起点  参数范围 0 - (IPS200_Y_MAX/16-1)
 *  @param      dat       	    需要显示的字符串
 *  @return     void
 *  @since      v1.0
 *  Sample usage:               st7789v_showstr(0,0,"seekfree");
 */
void st7789v_showDeviceName_Hcent(uint16 y, uint8 dat[])
{
    uint16 j, x;
    j = 0;
    x = (240 - strlen((char *)dat) * 8) / 2;
    while (dat[j] != '\0')
    {
        st7789v_showchar(x + 8 * j, y , dat[j]);
        j++;
    }
}
/****************************************************************************************
 *  @fn         st7789v_showint8
 *  @brief      液晶显示8位有符号
 *  @param      x     	        坐标x方向的起点  参数范围 0 -（IPS_X_MAX-1）
 *  @param      y     	        坐标y方向的起点  参数范围 0 -（IPS_Y_MAX/16-1）
 *  @param      dat       	    需要显示的变量，数据类型int8
 *  @return     void
 *  @since      v1.0
 *  Sample usage:               st7789v_showint8(0,0,x);//x为int8类型
 */
void st7789v_showint8(uint16 x, uint16 y, int8 dat)
{
    uint8 a[3];
    uint8 i;
    if (dat < 0)
    {
        st7789v_showchar(x, y * 16, '-');
        dat = -dat;
    }
    else
        st7789v_showchar(x, y * 16, ' ');

    a[0] = dat / 100;
    a[1] = dat / 10 % 10;
    a[2] = dat % 10;
    i    = 0;
    while (i < 3)
    {
        st7789v_showchar(x + (8 * (i + 1)), y * 16, '0' + a[i]);
        i++;
    }
}

/****************************************************************************************
 *  @fn         st7789v_showuint8
 *  @brief      液晶显示8位无符号
 *  @param      x     	        坐标x方向的起点 参数范围 0 -（IPS200_X_MAX-1）
 *  @param      y     	        坐标y方向的起点 参数范围 0 -（IPS200_Y_MAX/16-1）
 *  @param      dat       	    需要显示的变量，数据类型uint8
 *  @return     void
 *  @since      v1.0
 *  Sample usage:               st7789v_showuint8(0,0,x);//x为uint8类型
 */
void st7789v_showuint8(uint16 x, uint16 y, uint8 dat)
{
	uint8 a[ 3 ];
	uint8 i;
	
	a[ 0 ] = dat / 100;
	a[ 1 ] = dat / 10 % 10;
	a[ 2 ] = dat % 10;
	i = 0;
	while(i<3)
	{
		st7789v_showchar(x+(8*i),y*16,'0' + a[i]);
		i++;
	}
}

/****************************************************************************************
 *  @fn         st7789v_showint16
 *  @brief      液晶显示16位有符号
 *  @param      x     	        坐标x方向的起点 参数范围 0 -（IPS200_X_MAX-1）
 *  @param      y     	        坐标y方向的起点 参数范围 0 -（IPS200_Y_MAX/16-1）
 *  @param      dat       	    需要显示的变量，数据类型int16
 *  @return     void
 *  @since      v1.0
 *  Sample usage:               st7789v_showint16(0,0,x);//x为int16类型
 */
void st7789v_showint16(uint16 x, uint16 y, int16 dat)
{
	uint8 a[ 5 ];
	uint8 i;
	if (dat < 0)
	{
		st7789v_showchar( x, y * 16, '-' );
		dat = -dat;
	}
	else
		st7789v_showchar( x, y * 16, ' ' );

	a[ 0 ] = dat / 10000;
	a[ 1 ] = dat / 1000 % 10;
	a[ 2 ] = dat / 100 % 10;
	a[ 3 ] = dat / 10 % 10;
	a[ 4 ] = dat % 10;
	
	i = 0;
	while(i<5)
	{
		st7789v_showchar(x+(8*(i+1)),y*16,'0' + a[i]);
		i++;
	}
}

/****************************************************************************************
 *  @brief      液晶显示16位无符号
 *  @param      x     	        坐标x方向的起点 参数范围 0 -（IPS200_X_MAX-1）
 *  @param      y     	        坐标y方向的起点 参数范围 0 -（IPS200_Y_MAX/16-1）
 *  @param      dat       	    需要显示的变量，数据类型uint16
 *  @return     void
 *  @since      v1.0
 *  Sample usage:               st7789v_showuint16(0,0,x);//x为uint16类型
 */
void st7789v_showuint16(uint16 x, uint16 y, uint16 dat)
{
	uint8 a[ 5 ];
	uint8 i;
	
	a[ 0 ] = dat / 10000;
	a[ 1 ] = dat / 1000 % 10;
	a[ 2 ] = dat / 100 % 10;
	a[ 3 ] = dat / 10 % 10;
	a[ 4 ] = dat % 10;

	i = 0;
	while(i<5)
	{
		st7789v_showchar(x+(8*i),y*16,'0' + a[i]);
		i++;
	}
}

/****************************************************************************************
 *  @brief      液晶显示8位无符号
 *  @param      x     	        坐标x方向的起点 参数范围 0 -（IPS200_X_MAX-1）
 *  @param      y     	        坐标y方向的起点 参数范围 0 -（IPS200_Y_MAX/16-1）
 *  @param      w       	    图像宽度
 *  @param      l       	    图像高度
 *  @param      *p       	    图像数组地址
 *  @return     void
 *  @since      v1.0
 *  Sample usage:               st7789v_showimage(0,0,10,20,image);//图像起点(0,0)。宽10，高20。
 */
void st7789v_showimage(uint16 x, uint16 y, uint16 w, uint16 l, const unsigned char *p)
{
    int i,imageSize;
    imageSize = w * l;
    st7789v_address_set(x, y, x + w - 1, y + l - 1);
    for (i = 0; i <imageSize ; i++)
    {
        uint16_t pic = *(uint16_t *)(p + i * 2);
        st7789v_wr_data16(pic); 
    }
}

/****************************************************************************************
 *  @brief      汉字显示
 *  @param      x       横坐标 0-（IPS200_X_MAX-1）
 *  @param      y       纵坐标 0-（IPS200_Y_MAX-1）
 *  @param      size    取模的时候设置的汉字字体大小，也就是一个汉字占用的点阵长宽为多少个点
 *                      取模的时候需要长宽是一样的。
 *  @param      *p      需要显示的汉字数组
 *  @param      number  需要显示多少位
 *  @param      color   显示颜色
 *  @return     void
 *  @since      v1.0
 *  @note       使用PCtoLCD2002软件取模		    阴码、逐行式、顺向   16*16
 * 
 *  Sample usage:		st7789v_display_chinese(0,0,16,chinese_test[0],4,RED);//显示font文件里面的 示例
 */
void st7789v_display_chinese(uint16 x, uint16 y, uint8 size, const uint8 *p, uint8 number, uint16 color)
{
	int i, j, k;
	uint8 temp, temp1, temp2;
	const uint8 *p_data;
	temp2 = size / 8;

	st7789v_address_set( x, y, number * size - 1 + x, y + size - 1 );

	for (i = 0; i < size; i++)
	{
		temp1 = number;
		p_data = p + i * temp2;
		while(temp1--)
		{
			for(k=0;k<temp2;k++)
			{
				for(j=8;j>0;j--)
				{
					temp = (*p_data>>(j-1)) & 0x01;
					if(temp) st7789v_wr_data16(color);
					else st7789v_wr_data16(g_BGColor);
				}
				p_data++;
			}
			p_data = p_data - temp2 + temp2*size;
		}
	}
}

void st7789v_display_chinese_Hcent(uint16 y, uint8 size, const uint8 *p, uint8 number, uint16 color)
{
	int i, j, k;
	uint8 temp, temp1, temp2;
	const uint8 *p_data;

    uint16 x;
    x = (240 - number * size) / 2;

	temp2 = size / 8;

	st7789v_address_set( x, y, number * size - 1 + x, y + size - 1 );

	for (i = 0; i < size; i++)
	{
		temp1 = number;
		p_data = p + i * temp2;
		while(temp1--)
		{
			for(k=0;k<temp2;k++)
			{
				for(j=8;j>0;j--)
				{
					temp = (*p_data>>(j-1)) & 0x01;
					if(temp) st7789v_wr_data16(color);
					else st7789v_wr_data16(g_BGColor);
				}
				p_data++;
			}
			p_data = p_data - temp2 + temp2*size;
		}
	}
}

/*
 * app_QRcode.c
 *
 *  Created on: Jul 20, 2022
 *      Author: TECH66
 */
#include "app_QRcode.h"
#include "LCD/app_lcd.h"
#include "Debug/debug.h"

void DISPLAY_RENCODE_TO_TFT(uint8_t *qrcode_data)
{
	uint8_t i,j;
	uint16_t x,y,p,str_loc = 0;

	EncodeData((char *)qrcode_data);


	// PRINT("Version = %u\r\n",m_nVersion);

	if(m_nSymbleSize*2>240)
	{
		st7789v_showstr_Hcent(str_loc,(uint8 *)"too big");
		return;
	}
	for(i=0;i<10;i++)
	{
		if((m_nSymbleSize*i*2)>112)	break;
	}
	i=3;
	p = (i-1)*2+1;//
	x = (240-m_nSymbleSize*p)/2+2;
	y = QRCODE_Y;

	for(i=0;i<m_nSymbleSize;i++)
	{
		for(j=0;j<m_nSymbleSize;j++)
		{

			if(m_byModuleData[i][j]==1)
			{
				LCD_Fill_Rect(x+p*i,y+p*j,p,p, BLACK);
            }
			else
			{
			}
		}
	}
}

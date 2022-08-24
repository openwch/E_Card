/********************************** (C) COPYRIGHT *******************************
 * File Name          : lwns_show_interface.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : Jul 26, 2022
 * Description        :
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#include "lwns_show_interface.h"
#include <LCD/app_lcd.h>

void show_vote_start()
{
    voteflag = 1;
    voteKeyVal = 'W';
    LCD_PageRefresh(PAGE_4_0);
}

void show_vote_end()
{
    voteflag = 0;
    voteKeyVal = 'W';
    LCD_PageRefresh(PAGE_3);
}

void show_draw_start()
{
    // LCD_PageRefresh(PAGE_5);
}

void show_draw_end()
{
    LCD_PageRefresh(PAGE_3);
}

void show_draw_win(uint8_t level)
{
    switch (level)
    {
        case '1':
            break;
        case '2':
            break;
        case '3':
            break;
        default:
            break;
    }
}

uint32_t get_vote_value()
{
	switch (g_CurrentPageId)
	{
		case PAGE_4_1:
			voteKeyVal = 'A';
			break;
		case PAGE_4_2:
			voteKeyVal = 'B';
			break;
		case PAGE_4_3:
			voteKeyVal = 'C';
			break;
		case PAGE_4_4:
			voteKeyVal = 'D';
			break;
		default:
			voteKeyVal = 'W';
			break;
	}
    return voteKeyVal;
}



void show_conference(uint8_t num)
{
    if (g_ReportContentIdx != num)
    {
        g_ReportContentIdx = num;
        LCD_PageRefresh(PAGE_6);
    }
}

void show_testinit()
{
	LCD_PageRefresh(PAGE_3);
}

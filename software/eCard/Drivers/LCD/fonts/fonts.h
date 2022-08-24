
#ifndef __FONTS_H_
#define __FONTS_H_

#include <LCD/common.h>
#include "LCD/app_lcd.h"

//-------常用颜色----------
#define RED          	0xF800	//红色
#define BLUE         	0x001F  //蓝色
#define YELLOW       	0xFFE0	//黄色
#define GREEN        	0x07E0	//绿色
#define WHITE        	0xFFFF	//白色
#define BLACK        	0x0000	//黑色 
#define GRAY  			0X8430 	//灰色
#define BROWN 			0XBC40 	//棕色
#define PURPLE    		0XF81F	//紫色
#define PINK    		0XFE19	//粉色

extern const uint8 tft_ascii[95][16];

extern const uint8 gImage_qq[3200];

extern const uint8 chinese_test[8][16];

extern const uint8 zh_string16x16[160];

extern const uint8 zh_string24x24[360];

extern const uint8 zh_string32x32[640];

extern const uint8 zh_string40x40[1000];

extern const uint8_t dateNum[16][216];

extern const uint8_t battNum[10][192];

extern const uint8_t gImage_uname1[828];

extern const unsigned char gImage_uname[828];

extern const unsigned char gImage_point_104_296[264];

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

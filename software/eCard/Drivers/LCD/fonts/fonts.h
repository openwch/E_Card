
#ifndef __FONTS_H_
#define __FONTS_H_

#include <LCD/common.h>
#include "LCD/app_lcd.h"

//-------������ɫ----------
#define RED          	0xF800	//��ɫ
#define BLUE         	0x001F  //��ɫ
#define YELLOW       	0xFFE0	//��ɫ
#define GREEN        	0x07E0	//��ɫ
#define WHITE        	0xFFFF	//��ɫ
#define BLACK        	0x0000	//��ɫ 
#define GRAY  			0X8430 	//��ɫ
#define BROWN 			0XBC40 	//��ɫ
#define PURPLE    		0XF81F	//��ɫ
#define PINK    		0XFE19	//��ɫ

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

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

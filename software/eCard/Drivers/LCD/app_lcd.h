/*
 * display.h
 *
 *  Created on: Jul 19, 2022
 *      Author: TECH66
 */

#ifndef _APP_LCD_H
#define _APP_LCD_H

#include "common.h"
#include "fonts/fonts.h"
#include "LCD/lcd_st7789v.h"
#include "SPIFLASH/spi_flash.h"

/* 定义LCD显示区域的分辨率 */
#define LCD_20_HEIGHT 320 /* 3.0寸宽屏 高度，单位：像素 */
#define LCD_20_WIDTH  240 /* 3.0寸宽屏 宽度，单位：像素 */

/* 支持的驱动芯片ID */

#define CHIP_STR_V208 "CH32V208"

/*
    LCD 颜色代码，CL_是Color的简写
    16Bit由高位至低位， RRRR RGGG GGGB BBBB

    下面的RGB 宏将24位的RGB值转换为16位格式。
    启动windows的画笔程序，点击编辑颜色，选择自定义颜色，可以获得的RGB值。

    推荐使用迷你取色器软件获得你看到的界面颜色。
*/
#define RGB(R, G, B) (((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3)) /* 将8位R,G,B转化为 16位RGB565格式 */

/* 解码出 R=8bit G=8bit B=8bit */
#define RGB565_R(x) ((x >> 8) & 0xF8)
#define RGB565_G(x) ((x >> 3) & 0xFC)
#define RGB565_B(x) ((x << 3) & 0xF8)

/* 解码出 R=5bit G=6bit B=5bit */
#define RGB565_R2(x) ((x >> 11) & 0x1F)
#define RGB565_G2(x) ((x >> 5) & 0x3F)
#define RGB565_B2(x) ((x >> 0) & 0x1F)

#define UNAME_COX         46
#define UNAME_COY         81
#define UNAME_W           144
#define UNAME_H           46
#define UNAME_SIZE        828
#define UNAME_ADDR        0x0806B800

#define DRAW_DATE_TIME_COY  15
#define DRAWTIME_SIZE       5

#define DRAWTIME_HOUR_COX   119
#define DRAWTIME_HOUR_SIZE  2
#define DRAWTIME_MINU_COX   141
#define DRAWTIME_MINU_SIZE  2

#define DRAWTIME_YEAR_COX   24
#define DRAWTIME_YEAR_SIZE  4
#define DRAWTIME_MONTH_COX  66
#define DRAWTIME_MONTH_SIZE 2
#define DRAWTIME_DAY_COX    90
#define DRAWTIME_DAY_SIZE   2
#define DRAWDATE_SIZE       10

#define DRAWBATT_VAL_COX    192
#define DRAWBATT_VAL_COY    30
#define DRAWBATT_VAL_SIZE   4

#define DATENUM_W           9
#define DATENUM_H           12
#define DATENUM_SIZE        216

#define DRAW_BATTERY_COY    14
#define DRAW_BATTERY_COX    194
#define BATTERY_W           28
#define BATTERYNUM_W        8
#define BATTERYNUM_H        12
#define BATTERYNUM_SIZE     192

#define DEVICENAME_COY      123
#define DEVICENAME_COX      123

enum
{
    NUM0=0,
    NUM1,
    NUM2,
    NUM3,
    NUM4,
    NUM5,
    NUM6,
    NUM7,
    NUM8,
    NUM9
};

const unsigned char gImage_uname[828];
const uint8_t image_data_uname[828];
const uint8_t gImage_uname1[828];
enum
{
    CL_WHITE  = RGB(255, 255, 255), /* 白色 */
    CL_BLACK  = RGB(0, 0, 0),       /* 黑色 */
    CL_RED    = RGB(255, 0, 0),     /* 红色 */
    CL_GREEN  = RGB(0, 255, 0),     /* 绿色 */
    CL_BLUE   = RGB(0, 0, 255),     /* 蓝色 */
    CL_YELLOW = RGB(255, 255, 0),   /* 黄色 */

    CL_GREY  = RGB(98, 98, 98),    /* 深灰色 */
    CL_GREY1 = RGB(150, 150, 150), /* 浅灰色 */
    CL_GREY2 = RGB(180, 180, 180), /* 浅灰色 */
    CL_GREY3 = RGB(200, 200, 200), /* 最浅灰色 */
    CL_GREY4 = RGB(230, 230, 230), /* 最浅灰色 */

    CL_BUTTON_GREY = RGB(220, 220, 220), /* WINDOWS 按钮表面灰色 */

    CL_MAGENTA = 0xF81F, /* 红紫色，洋红色 */
    CL_CYAN    = 0x7FFF, /* 蓝绿色，青色 */

    CL_BLUE1 = RGB(0, 0, 240),   /* 深蓝色 */
    CL_BLUE2 = RGB(0, 0, 128),   /* 深蓝色 */
    CL_BLUE3 = RGB(68, 68, 255), /* 浅蓝色1 */
    CL_BLUE4 = RGB(0, 64, 128),  /* 浅蓝色1 */

    /* UI 界面 Windows控件常用色 */
    CL_BTN_FACE = RGB(236, 233, 216), /* 按钮表面颜色(灰) */

    CL_BTN_FONT = CL_BLACK, /* 按钮字体颜色（黑） */

    CL_BOX_BORDER1 = RGB(172, 168, 153), /* 分组框主线颜色 */
    CL_BOX_BORDER2 = RGB(255, 255, 255), /* 分组框阴影线颜色 */

    CL_MASK = 0x9999 /* 颜色掩码，用于文字背景透明 */
};

/* 背景光控制 */
#define BRIGHT_MAX     255
#define BRIGHT_MIN     0
#define BRIGHT_DEFAULT 30
#define BRIGHT_STEP    5

typedef enum
{
    PAGE_INVALID = 0,
    PAGE_1, 
    PAGE_2,
    PAGE_3,
    PAGE_4_0,
    PAGE_4_1,
    PAGE_4_2,
    PAGE_4_3,
    PAGE_4_4,
    PAGE_5_1,
    PAGE_5_2,
    PAGE_5_3,
    PAGE_5_4,
    PAGE_5_5,
    PAGE_5_6,
    PAGE_6
} PAGEID_typedefEnmu;

typedef enum
{
    KEY_INVALID =0,
    KEY_RIGHT,
    KEY_TOP ,
    KEY_LEFT,
    KEY_DOWN,
    KEY_RIGHT_LONG,
    KEY_TOP_LONG ,
    KEY_LEFT_LONG,
    KEY_DOWN_LONG
} SWITCHKEY_typedefEnmu;

typedef struct
{
    // PAGEID_typedefEnmu self_page_id;
    PAGEID_typedefEnmu top_page_id;
    PAGEID_typedefEnmu down_page_id;
    PAGEID_typedefEnmu left_page_id;
    PAGEID_typedefEnmu right_page_id;
    PAGEID_typedefEnmu long_top_page_id;
    PAGEID_typedefEnmu long_down_page_id;
    PAGEID_typedefEnmu long_left_page_id;
    PAGEID_typedefEnmu long_right_page_id;
} PAGE_typedefStruct;

typedef struct _HEADCOLOR
{
    unsigned char scan;
    unsigned char gray;
    unsigned short w;
    unsigned short h;
    unsigned char is565;
    unsigned char rgb;
    unsigned short gImage[2]; // 320*240,16位真彩
} HEADCOLOR;                  //__attribute__ ((aligned (2)))

typedef struct _IMAGEHEAD
{
    unsigned char scan;
    unsigned char gray;
    unsigned short w;
    unsigned short h;
    unsigned char is565;
    unsigned char rgb;
} IMAGEHEAD; //__attribute__ ((aligned (2)))

enum CurrentAgenda
{
    Agenda_0,
    AGENDA_1,
    AGENDA_2,
    AGENDA_3,
    AGENDA_4,
    AGENDA_5,
    AGENDA_6,
    AGENDA_7,
    AGENDA_8,
    AGENDA_9,
    AGENDA_10,
    AGENDA_11,
    AGENDA_12,
    AGENDA_13,
    AGENDA_14,
    AGENDA_15,
    AGENDA_16,
    AGENDA_17,
    AGENDA_18,
    AGENDA_19,
    AGENDA_20,
    AGENDA_21,
    AGENDA_22,
    AGENDA_23,
    AGENDA_24,
	AGENDA_25
};

// 索引     文件名          大小             偏移量            强制插入
//  1      0.bin        0x00025808        0x00000000             否
//  2      1.bin        0x00025808        0x00025808             否
//  3      2.bin        0x00025808        0x0004B010             否
//  4      3.bin        0x00025808        0x00070818             否
//  5      4-0.bin      0x00025808        0x00096020             否
//  6      4-1.bin      0x00025808        0x000BB828             否
//  7      4-2.bin      0x00025808        0x000E1030             否
//  8      4-3.bin      0x00025808        0x00106838             否
//  9      4-4.bin      0x00025808        0x0012C040             否
//  10     5-1.bin      0x00025808        0x00151848             否
//  11     5-2.bin      0x00025808        0x00177050             否
//  12     5-3.bin      0x00025808        0x0019C858             否
//  13     5-4.bin      0x00025808        0x001C2060             否
//  14     5-5.bin      0x00025808        0x001E7868             否
//  15     5-6.bin      0x00025808        0x0020D070             否
//  16     6-0.bin      0x00025808        0x00232878             否
//  17     6-1.bin      0x00025808        0x00258080             否
//  18     6-2.bin      0x00025808        0x0027D888             否
//  19     6-3.bin      0x00025808        0x002A3090             否
//  20     6-4.bin      0x00025808        0x002C8898             否
//  21     6-5.bin      0x00025808        0x002EE0A0             否
//  22     6-6.bin      0x00025808        0x003138A8             否
//  23     6-7.bin      0x00025808        0x003390B0             否
//  24     6-8.bin      0x00025808        0x0035E8B8             否
//  25     6-9.bin      0x00025808        0x003840C0             否
//  26     6-10.bin     0x00025808        0x003A98C8             否
//  27     6-11.bin     0x00025808        0x003CF0D0             否
//  28     6-12.bin     0x00025808        0x003F48D8             否
//  29     6-13.bin     0x00025808        0x0041A0E0             否
//  30     6-14.bin     0x00025808        0x0043F8E8             否
//  31     6-15.bin     0x00025808        0x004650F0             否
//  32     6-16.bin     0x00025808        0x0048A8F8             否
//  33     6-17.bin     0x00025808        0x004B0100             否
//  34     6-18.bin     0x00025808        0x004D5908             否
//  35     6-19.bin     0x00025808        0x004FB110             否
//  36     6-20.bin     0x00025808        0x00520918             否
//  37     6-21.bin     0x00025808        0x00546120             否
//  38     6-22.bin     0x00025808        0x0056B928             否
//  39     6-23.bin     0x00025808        0x00591130             否
//	40     6-24.bin		0x00025808        0x005B6938  			 否
//  41	   6-25.bin		0x00025808        0x005DC140             否

#define IMAGEADDR0              0x00000000
#define IMAGEADDR1              0x00025808
#define IMAGEADDR2              0x0004B010
#define IMAGEADDR3              0x00070818
#define IMAGEADDR4_0            0x00096020
#define IMAGEADDR4_1            0x000BB828
#define IMAGEADDR4_2            0x000E1030
#define IMAGEADDR4_3            0x00106838
#define IMAGEADDR4_4            0x0012C040
#define IMAGEADDR5_1            0x00151848
#define IMAGEADDR5_2            0x00177050
#define IMAGEADDR5_3            0x0019C858
#define IMAGEADDR5_4            0x001C2060
#define IMAGEADDR5_5            0x001E7868
#define IMAGEADDR5_6          	0x0020D070
#define IMAGEADDR6_0            0x00232878
#define IMAGEADDR6_1            0x00258080
#define IMAGEADDR6_2            0x0027D888
#define IMAGEADDR6_3            0x002A3090
#define IMAGEADDR6_4            0x002C8898
#define IMAGEADDR6_5            0x002EE0A0
#define IMAGEADDR6_6            0x003138A8
#define IMAGEADDR6_7            0x003390B0
#define IMAGEADDR6_8            0x0035E8B8
#define IMAGEADDR6_9            0x003840C0
#define IMAGEADDR6_10           0x003A98C8
#define IMAGEADDR6_11           0x003CF0D0
#define IMAGEADDR6_12           0x003F48D8
#define IMAGEADDR6_13           0x0041A0E0
#define IMAGEADDR6_14           0x0043F8E8
#define IMAGEADDR6_15           0x004650F0
#define IMAGEADDR6_16           0x0048A8F8
#define IMAGEADDR6_17           0x004B0100
#define IMAGEADDR6_18           0x004D5908
#define IMAGEADDR6_19           0x004FB110
#define IMAGEADDR6_20           0x00520918
#define IMAGEADDR6_21           0x00546120
#define IMAGEADDR6_22           0x0056B928
#define IMAGEADDR6_23           0x00591130
#define IMAGEADDR6_24		   	0x005B6938 
#define IMAGEADDR6_25		   	0x005DC140 

#define IMAGE_BUF_SIZE          480
#define IMAGE_DATA_SIZE         240

#define FLAG_IMAGE_REFUSH_START (1U << 0)
#define FLAG_IMAGE_BUF1         (1U << 1)
#define FLAG_IMAGE_BUF2         (1U << 2)
#define FLAG_IMAGE_BUF1_MAX     (1U << 3)
#define FLAG_IMAGE_BUF2_MAX     (1U << 4)
#define FLAG_IMAGE_HEADER       (1U << 5)

extern volatile uint8_t imageBufFlag;
extern volatile uint8_t currImageBuf;
extern volatile uint8_t nextImageBuf;
extern volatile uint8_t voteflag;
extern volatile uint8_t voteKeyVal;

#define PAGE_NUM   15
extern PAGE_typedefStruct PAGE[PAGE_NUM];
extern PAGEID_typedefEnmu g_CurrentPageId;
extern uint8_t imageBuf1[IMAGE_BUF_SIZE];
extern uint8_t imageBuf2[IMAGE_BUF_SIZE];
extern uint8_t spiDmaSendBuf[IMAGE_BUF_SIZE];

void LCD_DisplayPageInit(PAGE_typedefStruct *page,
                         PAGEID_typedefEnmu top_page_id,
                         PAGEID_typedefEnmu down_page_id,
                         PAGEID_typedefEnmu left_page_id,
                         PAGEID_typedefEnmu right_page_id,
                         PAGEID_typedefEnmu long_top_page_id,
                         PAGEID_typedefEnmu long_down_page_id,
                         PAGEID_typedefEnmu long_left_page_id,
                         PAGEID_typedefEnmu long_right_page_id);


void LCD_DisplayPage_Switch(uint8_t switch_key_val);

/* 可供外部模块调用的函数 */
void LCD_InitHard(void);
uint16_t LCD_GetHeight(void);
uint16_t LCD_GetWidth(void);
uint16 LCD_rgb_2_565(uint32 rgb_color_value);
void LCD_ClrScr(uint16_t _usColor);
void LCD_DrawBMP(uint16_t _usX, uint16_t _usY,HEADCOLOR *image);
void LCD_DrawBMP_DMA(uint16_t _usX, uint16_t _usY,uint32_t imageAddr);
void LCD_DrawBMPWithColorFill(uint16_t _usX, uint16_t _usY, HEADCOLOR *image, uint16_t _usColor);
void LCD_SetBackLight(uint8_t _bright);
uint8_t LCD_GetBackLight(void);
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void LCD_PageRefresh(PAGEID_typedefEnmu pageId);
void LCD_DrawTime(u8 hour,u8 min);
void LCD_DrawDate(u16 w_year,u8 w_month,u8 w_date);
void LCD_DrawBattVal(u8 batteryVal);
void LCD_DrawDeviceName(uint8* deviceName,uint8 len);
void LCD_DisplayPage_start(void);
/* 下面3个变量，主要用于使程序同时支持不同的屏 */
extern uint16_t g_LcdHeight;   /* 显示屏分辨率-高度 */
extern uint16_t g_LcdWidth;    /* 显示屏分辨率-宽度 */
extern uint8_t g_LcdDirection; /* 显示方向.0，1，2，3 */
extern uint8_t g_ReportContentIdx;

#endif /* APP_LCD_APP_LCD_H_ */

/*
 * display.c
 *
 *  Created on: Jul 19, 2022
 *      Author: TECH66
 */

#include <config.h>
#include "app_lcd.h"
#include <BLE/peripheral.h>
#include <QRcode/app_QRcode.h>
#include <RTC/app_rtc.h>
#include "TOUCHKEY.h"
#include <BLE/ble_control.h>
#include "HAL.h"
#include <BLE/lwns_name_tag.h>

/* 下面3个变量，主要用于使程序同时支持不同的屏 */
uint16_t g_LcdHeight = IPS200_Y_MAX; /* 显示屏分辨率-高度 */
uint16_t g_LcdWidth  = IPS200_X_MAX; /* 显示屏分辨率-宽度 */
uint8_t s_ucBright;                  /* 背光亮度参数 */
uint8_t g_LcdDirection = 0;          /* 显示方向.0，1，2，3 */
uint8_t g_ReportContentIdx = 1;

uint32_t currAgendaIdx[26] =
    {
        IMAGEADDR6_0, IMAGEADDR6_1, IMAGEADDR6_2, IMAGEADDR6_3, IMAGEADDR6_4,
        IMAGEADDR6_5, IMAGEADDR6_6, IMAGEADDR6_7, IMAGEADDR6_8, IMAGEADDR6_9,
        IMAGEADDR6_10, IMAGEADDR6_11, IMAGEADDR6_12, IMAGEADDR6_13, IMAGEADDR6_14,
        IMAGEADDR6_15, IMAGEADDR6_16, IMAGEADDR6_17, IMAGEADDR6_18, IMAGEADDR6_19,
        IMAGEADDR6_20, IMAGEADDR6_21, IMAGEADDR6_22, IMAGEADDR6_23, IMAGEADDR6_24, 
        IMAGEADDR6_25};

PAGEID_typedefEnmu g_CurrentPageId;

PAGE_typedefStruct PAGE[PAGE_NUM] = {0};

volatile uint8_t imageBufFlag = 0;
volatile uint8_t currImageBuf = 0;
volatile uint8_t nextImageBuf = 0;
volatile uint8_t voteflag     = 0;
volatile uint8_t voteKeyVal   = 0;

__attribute__((aligned(2))) uint8_t imageBuf1[IMAGE_BUF_SIZE] = {0xFF};
__attribute__((aligned(2))) uint8_t imageBuf2[IMAGE_BUF_SIZE] = {0xFF};
uint8_t spiDmaSendBuf[IMAGE_BUF_SIZE]                         = {0xFF};
static void LCD_SetPwmBackLight(uint8_t _bright);

void LCD_DisplayPage_1(void);
void LCD_DisplayPage_2(void);
void LCD_DisplayPage_3(void);
void LCD_DisplayPage_4_0(void);
void LCD_DisplayPage_4_1(void);
void LCD_DisplayPage_4_2(void);
void LCD_DisplayPage_4_3(void);
void LCD_DisplayPage_4_4(void);
void LCD_DisplayPage_5_1(void);
void LCD_DisplayPage_5_2(void);
void LCD_DisplayPage_5_3(void);
void LCD_DisplayPage_5_4(void);
void LCD_DisplayPage_5_5(void);
void LCD_DisplayPage_5_6(void);
void LCD_DisplayPage_6(void);
void LCD_DisplayPage_RepContent(uint8_t RepContentIdx);
void XAA(const uint8_t *p);

/****************************************************************************************
 * @fn      LCD_SetPwmBackLight
 * @brief   初始化控制LCD背景光的GPIO,配置为PWM模式。
 *			当关闭背光时，将CPU IO设置为浮动输入模式（推荐设置为推挽输出，并驱动到低电平);
 *          将TIM3关闭 省电
 *
 * @param _bright 亮度，0是灭，255是最亮
 *
 * @return void
 */
static void LCD_SetPwmBackLight(uint8_t _bright)
{
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_OCInitStructure.TIM_OCMode        = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState   = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse         = 0xFF - _bright;
    TIM_OCInitStructure.TIM_OCPolarity    = TIM_OCPolarity_High;
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
}

/****************************************************************************************
 * @fn      LCD_SetBackLight
 * @brief   初始化控制LCD背景光的GPIO,配置为PWM模式。
 *			当关闭背光时，将CPU IO设置为浮动输入模式（推荐设置为推挽输出，并驱动到低电平);
 *          将TIM3关闭 省电
 *
 * @param   _bright 亮度，0是灭，255是最亮
 *
 * @return void
 */
void LCD_SetBackLight(uint8_t _bright)
{
    s_ucBright = _bright; /* 保存背光值 */

    LCD_SetPwmBackLight(s_ucBright);
}

/****************************************************************************************
 * @fn      LCD_GetBackLight
 * @brief   获得背光亮度参数
 *
 * @param   void
 *
 * @return uint8_t 背光亮度参数
 */
uint8_t LCD_GetBackLight(void)
{
    return s_ucBright;
}

/****************************************************************************************
 * @fn      LCD_GetHeight
 * @brief   读取LCD分辨率之高度
 *
 * @param   void
 *
 * @return uint16_t LCD分辨率之高度
 */
uint16_t LCD_GetHeight(void)
{
    return g_LcdHeight;
}

/****************************************************************************************
 * @fn      LCD_GetWidth
 * @brief   读取LCD分辨率之宽度
 *
 * @param   void
 *
 * @return uint16_t LCD分辨率之宽度
 */
uint16_t LCD_GetWidth(void)
{
    return g_LcdWidth;
}

/****************************************************************************************
 * @fn      LCD_ClrScr
 * @brief   根据输入的颜色值清屏
 *
 * @param _usColor 背景色
 * @return  void
 */
void LCD_ClrScr(uint16_t _usColor)
{
    uint16 i, j;
    st7789v_address_set(0, 0, IPS200_X_MAX - 1, IPS200_Y_MAX - 1);
    IPS200_RS_1;
    IPS200_RD_1;
    for (i = 0; i < IPS200_X_MAX; i++)
    {
        for (j = 0; j < IPS200_Y_MAX; j++)
        {
            st7789v_wr_data16(_usColor);
        }
    }
}

/****************************************************************************************
 * @fn      LCD_Fill_Rect
 * @brief   用一个颜色值填充一个矩形。
 *
 * @param _usX      矩形左上角的坐标
 * @param _usY      矩形左上角的坐标
 * @param _usHeight 矩形的高度
 * @param _usWidth  矩形的宽度
 * @param _usColor  填充的颜色值
 *
 * @return void
 */
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
    uint16 i, j;
    st7789v_address_set(_usX, _usY, _usX + _usWidth - 1, _usY + _usHeight - 1);
    for (i = _usX; i < _usX + _usWidth; i++)
    {
        for (j = _usY; j < _usY + _usHeight; j++)
        {
            st7789v_wr_data16(_usColor);
        }
    }
}

/****************************************************************************************
 * @fn      LCD_DrawUname
 * @brief   用一个颜色值填充一个矩形。
 *
 * @param _usX      矩形左上角的坐标
 * @param _usY      矩形左上角的坐标
 * @param _usHeight 矩形的高度
 * @param _usWidth  矩形的高度
 * @param _usColor  填充的颜色值
 *
 * @return void
 */
void LCD_DrawUname(const uint8 *p, uint16 color)
{
    int i, j;
    uint8 temp;
    const uint8 *p_data;

    st7789v_address_set(UNAME_COX, UNAME_COY - 5, UNAME_W - 1 + UNAME_COX, UNAME_H - 6 + UNAME_COY);

    for (i = 0; i < UNAME_SIZE; i++)
    {
        p_data = p + i;

        for (j = 8; j > 0; j--)
        {
            temp = (*p_data >> (j - 1)) & 0x01;

                if (temp)
                    st7789v_wr_data16(color);
                else
                    st7789v_wr_data16(g_BGColor);
        }
    }
}
/****************************************************************************************
 * @fn      LCD_DrawBMP
 * @brief   在LCD上显示一个BMP位图，位图点阵扫描次序: 从左到右，从上到下
 *
 * @param _usX      图片的横坐标
 * @param _usY      图片的纵坐标
 * @param _pImage   图片点阵指针
 *
 * @return  void
 */
void LCD_DrawBMP(uint16_t _usX, uint16_t _usY, HEADCOLOR *_pImage)
{
    int i, imageSize;
    imageSize = (_pImage->w) * (_pImage->h);
    st7789v_address_set(_usX, _usY, _usX - 1 + _pImage->w, _usY - 1 + _pImage->h);
    for (i = 0; i < imageSize; i++)
    {
        st7789v_wr_data16(*(_pImage->gImage + i));
    }
}

/****************************************************************************************
 * @fn      LCD_DrawBMP_DMA
 * @brief   通过DMA双缓冲的方式从外置SPIFLASH中读取BMP位图显示在LCD屏幕上，位图点阵扫描次序:
 *          从左到右，从上到下
 *
 * @param _usX      图片的左上角起始横坐标
 * @param _usY      图片的左上角起始纵坐标
 * @param imageAddr 图片在外置flash中的地址
 *
 * @return  void
 */
void LCD_DrawBMP_DMA(uint16_t _usX, uint16_t _usY, uint32_t imageAddr)
{
    register int i, j;
    int imageSize, imageblock, imageremain;
    u32 imageAddrs      = imageAddr;
    u16 *imageRefushBuf = NULL;

    uint8_t imageiniwrite[12] = {0xff};
    uint8_t imageiniread[12]  = {0xff};
    IMAGEHEAD *imageHead      = (IMAGEHEAD *)(imageiniread + 4);

    imageBufFlag     = 0;
    currImageBuf     = 0;
    nextImageBuf     = 0;
    imageiniwrite[0] = W25X_ReadData;
    imageiniwrite[1] = (u8)((imageAddrs) >> 16);
    imageiniwrite[2] = (u8)((imageAddrs) >> 8);
    imageiniwrite[3] = (u8)((imageAddrs));

    SF_CS_0();
    DMA_ClearFlag(DMA1_FLAG_TC5);
    DMA_ClearFlag(DMA1_FLAG_TC4);

    SPI_DMA_Recive(imageiniread, 12);
    SPI_DMA_Send(imageiniwrite, 12);
    while (!(imageBufFlag && FLAG_IMAGE_HEADER))
        ;

    imageSize = (imageHead->w) * (imageHead->h) * 2;
    // PRINT("imageHead->w:%d,imageHead->h:%d\r\n", imageHead->w, imageHead->h);

    imageBufFlag |= FLAG_IMAGE_REFUSH_START;

    if ((imageHead->w == 0) || (imageHead->h == 0)||(imageHead->w > LCD_20_WIDTH)|| (imageHead->h > LCD_20_HEIGHT)) return;
    imageblock  = imageSize / IMAGE_BUF_SIZE;
    imageremain = imageSize % IMAGE_BUF_SIZE;
    imageremain /= 2;
    if (imageblock >= 1)
    {
        SPI_DMA_Recive(imageBuf1, IMAGE_BUF_SIZE);
        SPI_DMA_Send(spiDmaSendBuf, IMAGE_BUF_SIZE);
        while (nextImageBuf != FLAG_IMAGE_BUF1)
            ;
    }
    st7789v_address_set(_usX, _usY, _usX - 1 + imageHead->w, _usY - 1 + imageHead->h);
    for (i = 0; i < imageblock; i++)
    {
        while (currImageBuf == nextImageBuf)
            ;
        currImageBuf = nextImageBuf;

        if (currImageBuf == FLAG_IMAGE_BUF1)
        {
            SPI_DMA_Recive(imageBuf2, IMAGE_BUF_SIZE);
            imageRefushBuf = (uint16_t *)imageBuf1;
        }
        else if (currImageBuf == FLAG_IMAGE_BUF2)
        {
            SPI_DMA_Recive(imageBuf1, IMAGE_BUF_SIZE);
            imageRefushBuf = (uint16_t *)imageBuf2;
        }
        SPI_DMA_Send(spiDmaSendBuf, IMAGE_BUF_SIZE);

        for (j = 0; j < IMAGE_DATA_SIZE; j++)
        {
            st7789v_wr_data16(*(imageRefushBuf + j));
        }

        if (currImageBuf == FLAG_IMAGE_BUF1)
        {
            imageBufFlag &= ~(uint8_t)FLAG_IMAGE_BUF1_MAX; // BUF1满标志清零
        }
        else if (currImageBuf == FLAG_IMAGE_BUF2)
        {
            imageBufFlag &= ~(uint8_t)FLAG_IMAGE_BUF2_MAX; // BUF2满标志清零
        }
    }

    while (currImageBuf == nextImageBuf)
        ;
    currImageBuf = nextImageBuf;

    SF_CS_1();

    for (j = 0; j < imageremain; j++)
    {
        st7789v_wr_data16(*(imageRefushBuf + j));
    }

    // imageBufFlag &= ~(uint8_t)FLAG_IMAGE_REFUSH_START;
    imageBufFlag &= ~(uint8_t)(FLAG_IMAGE_REFUSH_START | FLAG_IMAGE_BUF1_MAX | FLAG_IMAGE_BUF2_MAX);
}

/****************************************************************************************
 * @fn     LCD_DrawBMPWithColorFill
 * @brief  在LCD上显示一个BMP位图，位图点阵扫描次序: 从左到右，从上到下,其周围用_usColor填充
 *
 * @param _usX      图片的横坐标
 * @param _usY      图片的纵坐标
 * @param _pImage   图片点阵指针
 * @param _usColor  填充颜色
 *
 * @return  void
 */
void LCD_DrawBMPWithColorFill(uint16_t _usX, uint16_t _usY, HEADCOLOR *_pImage, uint16_t _usColor)
{
    int i, imageSize;
    uint16 k, j;
    imageSize = (_pImage->w) * (_pImage->h);

    uint16 fill_X1[4] = {0, 0, 0, _usX + _pImage->w};
    uint16 fill_Y1[4] = {0, _usY, _usY + _pImage->h, _usY};
    uint16 fill_X2[4] = {IPS200_X_MAX, _usX, IPS200_X_MAX, IPS200_X_MAX};
    uint16 fill_Y2[4] = {_usY, _usY + _pImage->h, IPS200_Y_MAX, _usY + _pImage->h};
    // 填充图片周围区域,填充顺序上,左，下，右
    for (i = 0; i < 3; i++)
    {
        st7789v_address_set(fill_X1[i], fill_Y1[i], fill_X2[i] - 1, fill_Y2[i] - 1);
        for (k = fill_X1[i]; k < fill_X2[i]; k++)
        {
            for (j = fill_Y1[i]; j < fill_Y2[i]; j++)
            {
                st7789v_wr_data16(_usColor);
            }
        }
    }
    st7789v_address_set(_usX, _usY, _usX - 1 + _pImage->w, _usY - 1 + _pImage->h);
    for (i = 0; i < imageSize; i++)
    {
        st7789v_wr_data16(*(_pImage->gImage + i));
    }

    st7789v_address_set(fill_X1[3], fill_Y1[3], fill_X2[3] - 1, fill_Y2[3] - 1);
    for (k = fill_X1[3]; k < fill_X2[3]; k++)
    {
        for (j = fill_Y1[3]; j < fill_Y2[3]; j++)
        {
            st7789v_wr_data16(_usColor);
        }
    }
}

/****************************************************************************************
 * @fn      LCD_DisplayPageInit
 * @brief   初始化显示的多个页面之间结构
 *
 * @param page          待初始化的页面结构体指针
 * @param top_page_id   该页面的上方页面
 * @param down_page_id  该页面的下方页面
 * @param left_page_id  该页面的左侧页面
 * @param right_page_id 该页面的右侧页面
 *
 * @return  void
 */
void LCD_DisplayPageInit(PAGE_typedefStruct *page,
                         PAGEID_typedefEnmu top_page_id,
                         PAGEID_typedefEnmu down_page_id,
                         PAGEID_typedefEnmu left_page_id,
                         PAGEID_typedefEnmu right_page_id,
                         PAGEID_typedefEnmu long_top_page_id,
                         PAGEID_typedefEnmu long_down_page_id,
                         PAGEID_typedefEnmu long_left_page_id,
                         PAGEID_typedefEnmu long_right_page_id)
{
    page->top_page_id   = top_page_id;
    page->down_page_id  = down_page_id;
    page->left_page_id  = left_page_id;
    page->right_page_id = right_page_id;
    page->long_top_page_id   = long_top_page_id;
    page->long_down_page_id  = long_down_page_id;
    page->long_left_page_id  = long_left_page_id;
    page->long_right_page_id = long_right_page_id;
}

/****************************************************************************************
 * @fn      LCD_DisplayPage_Switch
 * @brief   通过按键在LCD上显示的多个页面之间的切换
 *
 * @param switch_key_val 切换的键值
 * @return  void
 */
void LCD_DisplayPage_Switch(uint8_t switch_key_val)
{
    PAGEID_typedefEnmu next_page_id = 0;
    switch ((SWITCHKEY_typedefEnmu)switch_key_val)
    {
        case KEY_TOP:
            next_page_id = PAGE[g_CurrentPageId - 1].top_page_id;
            break;
        case KEY_DOWN:
            next_page_id = PAGE[g_CurrentPageId - 1].down_page_id;
            break;
        case KEY_LEFT:
            next_page_id = PAGE[g_CurrentPageId - 1].left_page_id;
            break;
        case KEY_RIGHT:
            next_page_id = PAGE[g_CurrentPageId - 1].right_page_id;
            break;
        case KEY_TOP_LONG:
            next_page_id = PAGE[g_CurrentPageId - 1].long_top_page_id;
            break;
        case KEY_DOWN_LONG:
            next_page_id = PAGE[g_CurrentPageId - 1].long_down_page_id;
            break;
        case KEY_LEFT_LONG:
            next_page_id = PAGE[g_CurrentPageId - 1].long_left_page_id;
            break;
        case KEY_RIGHT_LONG:
            if(g_CurrentPageId == PAGE_2) g_enter_main_sign=1;
            next_page_id = PAGE[g_CurrentPageId - 1].long_right_page_id;
            break;
        default:
            // printf("PAGE_INVALID\r\n");
            next_page_id = PAGE_INVALID;
            break;
    }
    if (PAGE_INVALID == next_page_id) return;

    LCD_PageRefresh(next_page_id);
}

/****************************************************************************************
 * @fn      LCD_PageRefresh
 * @brief   在LCD上刷新一个页面
 *
 * @param   pageId 要刷新的页面
 *
 * @return  void
 */
void LCD_PageRefresh(PAGEID_typedefEnmu pageId)
{
    g_CurrentPageId = pageId;

    switch (g_CurrentPageId)
    {
        case PAGE_1:
            LCD_DisplayPage_1();
            break;
        case PAGE_2:
            LCD_DisplayPage_2();
            break;
        case PAGE_3:
            LCD_DisplayPage_3();
            break;
         case PAGE_4_0:
            LCD_DisplayPage_4_0();
            break;
        case PAGE_4_1:
            LCD_DisplayPage_4_1();
            break;
        case PAGE_4_2:
            LCD_DisplayPage_4_2();
            break;
        case PAGE_4_3:
            LCD_DisplayPage_4_3();
            break;
        case PAGE_4_4:
            LCD_DisplayPage_4_4();
            break;
        case PAGE_5_1:
            LCD_DisplayPage_5_1();
            break;
        case PAGE_5_2:
            LCD_DisplayPage_5_2();
            break;
        case PAGE_5_3:
            LCD_DisplayPage_5_3();
            break;
        case PAGE_5_4:
            LCD_DisplayPage_5_4();
            break;
        case PAGE_5_5:
            LCD_DisplayPage_5_5();
            break;
        case PAGE_5_6:
            LCD_DisplayPage_5_6();
            break;
        case PAGE_6:
            LCD_DisplayPage_6();
            break;
        default:
            break;
    }
}

void screentimeProcess(void)
{
    screenTimeoutCount = 0;
    screenTimeout      = DISABLE;
    LCD_SetBackLight(BRIGHT_DEFAULT);
}

/****************************************************************************************
 * @fn      LCD_DisplayPage_x
 * @brief   指定展示页面的实现
 * @param   void
 * @return  void
 */

void LCD_DisplayPage_start(void)
{
    // screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR0);
    
}
//小程序二维码
void LCD_DisplayPage_1(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR1);
    
}

//蓝牙设备名称二维码界面
void LCD_DisplayPage_2(void)
{
    screentimeProcess();
    g_BGColor = LCD_rgb_2_565(0xffffff);
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR2);
    // st7789v_showDeviceName_Hcent(120, (uint8 *)attDeviceName);
    LCD_DrawDeviceName((uint8 *)attDeviceName, 0x0D);
    DISPLAY_RENCODE_TO_TFT((uint8 *)attDeviceName);
}

//用户主界面，姓名，时间，日期，电量
void LCD_DisplayPage_3(void)
{
    uint8_t *pData = NULL;
    if (g_people_name[0])
        pData = (uint8_t *)UNAME_ADDR;
    else
        // pData = (uint8_t *)gImage_PH;
    pData = (uint8_t*)gImage_uname;//image_data_uname;
    screentimeProcess();

    g_BGColor = LCD_rgb_2_565(0xffffff);
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR3);
    LCD_DrawDate(calendar.w_year, calendar.w_month, calendar.w_date);
    LCD_DrawTime(calendar.hour, calendar.min);
    LCD_DrawBattVal(g_BattVal&0xff);
    LCD_DrawUname(pData, LCD_rgb_2_565(0x0646cc));
    // XAA(pData);
}


//投票主界面
void LCD_DisplayPage_4_0(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR4_0);
}

//投票选项A
void LCD_DisplayPage_4_1(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR4_1);
}

//投票选项B
void LCD_DisplayPage_4_2(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR4_2);
}

//投票选项C
void LCD_DisplayPage_4_3(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR4_3);
}

//投票选项D
void LCD_DisplayPage_4_4(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR4_4);
}

//大会议程1
void LCD_DisplayPage_5_1(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR5_1);
    // LCD_DisplayPage_RepContent(g_ReportContentIdx);
}
//大会议程2
void LCD_DisplayPage_5_2(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR5_2);
}

//大会议程3
void LCD_DisplayPage_5_3(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR5_3);
}
//大会议程4
void LCD_DisplayPage_5_4(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR5_4);
}
//大会议程5
void LCD_DisplayPage_5_5(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR5_5);
}

//大会议程6
void LCD_DisplayPage_5_6(void)
{
    screentimeProcess();
    LCD_DrawBMP_DMA(0, 0, (uint32_t)IMAGEADDR5_6);
}

//当前议程
void LCD_DisplayPage_6(void)
{
    screentimeProcess();
    LCD_DisplayPage_RepContent(g_ReportContentIdx);
}

void LCD_DisplayPage_RepContent(uint8_t RepContentIdx)
{    
    int i;
    uint16_t* image = (uint16_t*)gImage_point_104_296;
    if(RepContentIdx > 25) RepContentIdx = 0;
    LCD_DrawBMP_DMA(0, 0, currAgendaIdx[RepContentIdx]);
    st7789v_address_set(104, 296, 136, 299);
    for (i = 0; i < 132; i++)
    {
        st7789v_wr_data16(*(image + i));
    }
}
/****************************************************************************************
 * @fn      LCD_rgb_2_565
 * @brief   颜色格式转换，从RGB24位转到16位rdb565格式
 *
 * @param rgb_color_value 24位颜色代码，例: 0xffffff
 * @return uint16 rgb565格式颜色代码
 */
uint16 LCD_rgb_2_565(uint32 rgb_color_value)
{
    uint16 value_565 = 0;

    uint8 red, blue, green;

    if (rgb_color_value > 0xffffff)
        return 0;

    blue = (rgb_color_value & 0xff) * 31 / 0xff;

    green = ((rgb_color_value >> 8) & 0xff) * 63 / 0xff;

    red = ((rgb_color_value >> 16) & 0xff) * 31 / 0xff;

    value_565 = (uint16)((red & 0x1f) << 11) | ((green & 0x3f) << 5) | (blue & 0x1f);

    return value_565;
}

void LCD_DrawTime(u8 hour, u8 min)
{
    char timestr[DRAWTIME_SIZE + 4];
    uint8 j;

    if (g_CurrentPageId == PAGE_3)
    {
        sprintf(timestr, "%02u%02u", hour, min);

        for (j = 0; j < DRAWTIME_HOUR_SIZE; j++)
        {
            st7789v_showdatenum(DRAWTIME_HOUR_COX + DATENUM_W * j, timestr[j]);
        }
        for (j = 0; j < DRAWTIME_MINU_SIZE; j++)
        {
            st7789v_showdatenum(DRAWTIME_MINU_COX + DATENUM_W * j, timestr[j + DRAWTIME_HOUR_SIZE]);
        }
    }
}

void LCD_DrawDate(u16 w_year, u8 w_month, u8 w_date)
{
    char datestr[DRAWDATE_SIZE + 6];
    uint8 j;
    if (g_CurrentPageId == PAGE_3)
    {
        sprintf(datestr, "%04u%02u%02u", w_year, w_month, w_date);

        for (j = 0; j < DRAWTIME_YEAR_SIZE; j++)
        {
            st7789v_showdatenum(DRAWTIME_YEAR_COX + DATENUM_W * j, datestr[j]);
        }
        for (j = 0; j < DRAWTIME_MONTH_SIZE; j++)
        {
            st7789v_showdatenum(DRAWTIME_MONTH_COX + DATENUM_W * j, datestr[j + DRAWTIME_YEAR_SIZE]);
        }
        for (j = 0; j < DRAWTIME_DAY_SIZE; j++)
        {
            st7789v_showdatenum(DRAWTIME_DAY_COX + DATENUM_W * j, datestr[j + DRAWTIME_YEAR_SIZE + DRAWTIME_MONTH_SIZE]);
        }
    }
}

void LCD_DrawBattVal(u8 batteryVal)
{
    char str[DRAWBATT_VAL_SIZE];
    uint8 j, valSize;
    uint16 x;
    if (g_CurrentPageId == PAGE_3)
    {

        LCD_Fill_Rect(DRAW_BATTERY_COX, DRAW_BATTERY_COY, BATTERYNUM_H, BATTERY_W, BLACK);
        if (batteryVal == 100)
        {
            valSize = 3;
            sprintf(str, "%3u", batteryVal);
        }
        else if (batteryVal < 10)
        {
            valSize = 1;
            sprintf(str, "%u", batteryVal);
        }
        else
        {
            valSize = 2;
            sprintf(str, "%2u", batteryVal);
        }

        x = DRAW_BATTERY_COX + ((BATTERY_W - (valSize * BATTERYNUM_W)) / 2);
        for (j = 0; j < valSize; j++)
        {
            st7789v_showbattnum(x + BATTERYNUM_W * j, str[j]);
        }
    }
}

void LCD_DrawDeviceName(uint8 *deviceName, uint8 len)
{
    uint8 *namestr = NULL;
    uint8 j;
    namestr = deviceName + 7;
    if (g_CurrentPageId == PAGE_2)
    {
        for (j = 0; j < 6; j++)
        {
            st7789v_showdevicenamenum(DEVICENAME_COX + DATENUM_W * j, *(namestr + j));
        }
    }
}

/****************************************************************************************
 * @fn      LCD_InitHard
 * @brief   初始化LCD及相关需要显示的页面结构
 *
 * @param   void
 *
 * @return  void
 */
void LCD_InitHard(void)
{
    st7789v_gpio_config();
    st7789v_init(); //初始化屏幕之前，先初始化GPIO.
    
    LCD_DisplayPageInit(&PAGE[0], PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_2,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_1
    LCD_DisplayPageInit(&PAGE[1], PAGE_INVALID, PAGE_INVALID, PAGE_1,       PAGE_INVALID,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_3      ); // PAGE_2
    LCD_DisplayPageInit(&PAGE[2], PAGE_INVALID, PAGE_INVALID, PAGE_6,       PAGE_5_1,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_3
    LCD_DisplayPageInit(&PAGE[3], PAGE_4_2,     PAGE_4_4,     PAGE_4_1,     PAGE_4_3, 
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_4_0
    LCD_DisplayPageInit(&PAGE[4], PAGE_4_2, 	PAGE_4_4	, PAGE_4_0, 	PAGE_4_3,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_4_1
    LCD_DisplayPageInit(&PAGE[5], PAGE_4_0, 	PAGE_4_4, 	  PAGE_4_1, 	PAGE_4_3,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_4_2
    LCD_DisplayPageInit(&PAGE[6], PAGE_4_2, 	PAGE_4_4, 	  PAGE_4_1, 	PAGE_4_0,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_4_3
    LCD_DisplayPageInit(&PAGE[7], PAGE_4_2, 	PAGE_4_0, 	  PAGE_4_1, 	PAGE_4_3,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_4_4
    LCD_DisplayPageInit(&PAGE[8], PAGE_INVALID, PAGE_5_2,     PAGE_3,       PAGE_INVALID,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_5_1
    LCD_DisplayPageInit(&PAGE[9], PAGE_5_1,     PAGE_5_3,     PAGE_3,       PAGE_INVALID,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_5_2
    LCD_DisplayPageInit(&PAGE[10],PAGE_5_2,     PAGE_5_4,     PAGE_3,       PAGE_INVALID, 
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_5_3
    LCD_DisplayPageInit(&PAGE[11],PAGE_5_3,     PAGE_5_5,     PAGE_3,       PAGE_INVALID,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_5_4
    LCD_DisplayPageInit(&PAGE[12],PAGE_5_4,     PAGE_5_6,     PAGE_3,       PAGE_INVALID,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_5_5
    LCD_DisplayPageInit(&PAGE[13],PAGE_5_5,     PAGE_INVALID, PAGE_3,       PAGE_INVALID,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_5_6
    LCD_DisplayPageInit(&PAGE[14],PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_3,
                                  PAGE_INVALID, PAGE_INVALID, PAGE_INVALID, PAGE_INVALID); // PAGE_6
    LCD_SetBackLight(BRIGHT_DEFAULT);                                            /* 打开背光，设置为缺省亮度 */

    
}

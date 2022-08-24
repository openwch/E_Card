/********************************** (C) COPYRIGHT *******************************
 * File Name          : TOUCHKEY.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2022/07/25
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "HAL.h"
#include <LCD/app_lcd.h>

/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/

static uint8_t halTKeySavedKeys; /* 保留按键最后的状态，用于查询是否有键值变化 */
volatile uint16_t longPressCounter;
volatile uint8_t key_zero_flag;
static uint16_t touchKeysBaseLine[4] = {0};
static uint16_t touchKeyThreshold[4] = {0};
// uint16_t g_touchKeyVal[12] = {0};

volatile uint8_t batteryMonitorFlag = DISABLE;
uint16_t g_BattVal = 0;
volatile uint8_t screenTimeout = DISABLE;

volatile uint16_t screenTimeoutCount;

#define Threshold  400
#define SAMPLE_NUM 50

/**************************************************************************************************
 *                                        FUNCTIONS - Local
 **************************************************************************************************/
static halTouchKeyCBack_t pHalTKeyProcessFunction; /* callback function */

/**************************************************************************************************
 * @fn      HAL_KeyInit
 *
 * @brief   Initilize Key Service
 *
 * @param   none
 *
 * @return  None
 **************************************************************************************************/
void HAL_TouchKeyInit(void)
{

    u8 i = 0, j = 0;
    s32 touchKeysValue[4] = {0};

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    ADC_InitTypeDef ADC_InitStructure   = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode       = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Cmd(ADC1, ENABLE);

    ADC1->CTLR2 |= ADC_TSVREFE;

    TKey1->CTLR1 |= (1 << 26) | (1 << 24);
    TKey1->IDATAR1 = 0x1c; // Charging Time
    

    for (j = 0; j < 100; j++)
    {
        for (i = 0; i < 4; i++)
        {
            touchKeysValue[i] += Hal_TouchKeyRead(i);
        }
    }
    for (i = 0; i < 4; i++)
    {
        touchKeysBaseLine[i] = touchKeysValue[i] / 100;

        touchKeyThreshold[i] = Threshold;
    }    
    batteryMonitorFlag = ENABLE;        //电池监控开启
}
/*********************************************************************
 * @fn      Hal_TouchKeyRead
 *
 * @brief   Returns ADCx conversion result data.
 *
 * @param   ch - ADC channel.
 *            ADC_Channel_0 - ADC Channel0 selected.
 *            ADC_Channel_1 - ADC Channel1 selected.
 *            ADC_Channel_2 - ADC Channel2 selected.
 *            ADC_Channel_3 - ADC Channel3 selected.
 *            ADC_Channel_4 - ADC Channel4 selected.
 *            ADC_Channel_5 - ADC Channel5 selected.
 *            ADC_Channel_6 - ADC Channel6 selected.
 *            ADC_Channel_7 - ADC Channel7 selected.
 *            ADC_Channel_8 - ADC Channel8 selected.
 *            ADC_Channel_9 - ADC Channel9 selected.
 *            ADC_Channel_10 - ADC Channel10 selected.
 *            ADC_Channel_11 - ADC Channel11 selected.
 *            ADC_Channel_12 - ADC Channel12 selected.
 *            ADC_Channel_13 - ADC Channel13 selected.
 *            ADC_Channel_14 - ADC Channel14 selected.
 *            ADC_Channel_15 - ADC Channel15 selected.
 *            ADC_Channel_16 - ADC Channel16 selected.
 *            ADC_Channel_17 - ADC Channel17 selected.
 *
 * @return  val - The Data conversion value.
 */
u16 Hal_TouchKeyRead(u8 ch)
{
    TKey1->CTLR1 |= (1 << 24);//(1 << 26) | 
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_7Cycles5);
    TKey1->IDATAR1 = 0x1c; // Charging Time
    TKey1->RDATAR  = 0x8;  // Discharging Time
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;
    return (uint16_t)TKey1->RDATAR;
}

u8 Hal_TouchKeyReadPoll(void)
{
    u8 i = 0, j = 0;
    s32 touchKeysValue[4] = {0};
    for (j = 0; j < SAMPLE_NUM; j++)
    {
        for (i = 0; i < 4; i++)
        {
            touchKeysValue[i] += Hal_TouchKeyRead(i);
        }
    }

    for (i = 0; i < 4; i++)
    {
        touchKeysValue[i] /= SAMPLE_NUM;
        touchKeysValue[i] = touchKeysBaseLine[i] - touchKeysValue[i];
        if (touchKeysValue[i] < 0) touchKeysValue[i] = 0;

    }

    j = 0;

    for (i = 0; i < 4; i++)
    {
        if (touchKeysValue[0] <= touchKeysValue[i])
        {
            touchKeysValue[0] = touchKeysValue[i];
            if (touchKeysValue[i] > touchKeyThreshold[i])
                j = i + 1;
        }
    }
    return j;
}
/**************************************************************************************************
 * @fn      Hal_TouchKeyConfig
 *
 * @brief   Configure the Key serivce
 *
 * @param   cback - pointer to the CallBack function
 *
 * @return  None
 **************************************************************************************************/
void Hal_TouchKeyConfig(halKeyCBack_t cback)
{
    /* Register the callback fucntion */
    pHalTKeyProcessFunction = cback;
    tmos_start_reload_task(halTaskID, HAL_KEY_EVENT, MS1_TO_SYSTEM_TIME(10)); /* Kick off polling */
    
}

/**************************************************************************************************
 * @fn      HAL_TouchKeyPoll
 *
 * @brief   Called by hal_driver to poll the keys
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HAL_TouchKeyPoll(void)
{
    uint8_t keys = 0;
    keys         = Hal_TouchKeyReadPoll();


    if(keys==0) 
    {
        key_zero_flag =1;
        longPressCounter=0;
    }

    if ((keys == halTKeySavedKeys)||(!key_zero_flag))
    { /* Exit - since no keys have changed */
        
        if(keys) longPressCounter++;
        if(longPressCounter > 100)//&& key_zero_flag
        {
            longPressCounter = 0;
            keys += 4; 
        }
        else return;
    }

    key_zero_flag = 0;

    halTKeySavedKeys = keys; /* Store the current keys for comparation next time */

    /* Invoke Callback if new keys were depressed */
    if (keys && (pHalTKeyProcessFunction))
    {
        screenTimeoutCount = 0;
        if(screenTimeout == ENABLE)
        {
            screenTimeout= DISABLE ;
            LCD_SetBackLight(BRIGHT_DEFAULT);
            return;
        }
//        if (voteflag)
//        {
//            voteflag = 0;
//            switch ((SWITCHKEY_typedefEnmu)keys)
//            {
//                case KEY_TOP:
//                    voteKeyVal = 'B';
//                    break;
//                case KEY_DOWN:
//                    voteKeyVal = 'D';
//                    break;
//                case KEY_LEFT:
//                    voteKeyVal = 'A';
//                    break;
//                case KEY_RIGHT:
//                    voteKeyVal = 'C';
//                    break;
//                default:
//                    voteKeyVal = 'W';
//                    break;
//            }
//        }
        (pHalTKeyProcessFunction)(keys);
    }
}

u16 batteryRead(void)
{
    s32 batteryVal = 0;
    static s32 locBatteryVal=0;
    s32 diffBatteryVal=0;
    s32 vrefVal    = 0;
    u8 i           = 0;
    TKey1->CTLR1 &= ~(1 << 24);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_7Cycles5);

    for (i = 0; i < 30; i++)
    {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
            ;
        vrefVal += ADC1->RDATAR;
    }
    vrefVal /= 30;
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_7Cycles5);

    for (i = 0; i < 30; i++)
    {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
            ;
        batteryVal += ADC1->RDATAR;
    }
    batteryVal /= 30;
    batteryVal =2400 * batteryVal / vrefVal;
    diffBatteryVal = locBatteryVal-batteryVal;
    if((diffBatteryVal>10) || (diffBatteryVal<-10))
    {
        locBatteryVal = batteryVal;
    }
    batteryVal = locBatteryVal/10 + 10;
    if (batteryVal < 321)
        return 321;
    if (batteryVal > 420)
        return 420;
    return batteryVal;
}

/******************************** endfile @ touchkey ******************************/

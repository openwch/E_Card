/********************************** (C) COPYRIGHT *******************************
 * File Name          : KEY.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2022/01/18
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "HAL.h"
#include <LCD/app_lcd.h>

/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/

static uint8_t halKeySavedKeys; /* ������������״̬�����ڲ�ѯ�Ƿ��м�ֵ�仯 */

/**************************************************************************************************
 *                                        FUNCTIONS - Local
 **************************************************************************************************/
static halKeyCBack_t pHalKeyProcessFunction; /* callback function */

/**************************************************************************************************
 * @fn      HAL_KeyInit
 *
 * @brief   Initilize Key Service
 *
 * @param   none
 *
 * @return  None
 **************************************************************************************************/
void HAL_KeyInit(void)
{
    /* Initialize previous key to 0 */
    halKeySavedKeys = 0;
    /* Initialize callback function */
    pHalKeyProcessFunction = NULL;

    RCC_APB2PeriphClockCmd(KEY1_PCENR, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY1_GPIO, &GPIO_InitStructure);
}

/**************************************************************************************************
 * @fn      HalKeyConfig
 *
 * @brief   Configure the Key serivce
 *
 * @param   cback - pointer to the CallBack function
 *
 * @return  None
 **************************************************************************************************/
void HalKeyConfig(halKeyCBack_t cback)
{
    /* Register the callback fucntion */
    pHalKeyProcessFunction = cback;
    tmos_start_task(halTaskID, HAL_KEY_EVENT, HAL_KEY_POLLING_VALUE); /* Kick off polling */
}

/**************************************************************************************************
 * @fn      HalKeyRead
 *
 * @brief   Read the current value of a key
 *
 * @param   None
 *
 * @return  keys - current keys status
 **************************************************************************************************/
uint8_t HalKeyRead(void)
{
    uint8_t keys = 0;

    if(HAL_PUSH_BUTTON1())
    { //������1
        keys |= HAL_KEY_SW_1;
    }
    if(HAL_PUSH_BUTTON2())
    { //������1
        keys |= HAL_KEY_SW_2;
    }
    if(HAL_PUSH_BUTTON3())
    { //������1
        keys |= HAL_KEY_SW_3;
    }
    if(HAL_PUSH_BUTTON4())
    { //������1
        keys |= HAL_KEY_SW_4;
    }
    return keys;
}

/**************************************************************************************************
 * @fn      HAL_KeyPoll
 *
 * @brief   Called by hal_driver to poll the keys
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HAL_KeyPoll(void)
{
    uint8_t keys = 0;
    if (HAL_PUSH_BUTTON1())
    { //������1
        keys |= KEY_LEFT;
    }
    if (HAL_PUSH_BUTTON2())
    { //������1
        keys |= KEY_DOWN;
    }
    if (HAL_PUSH_BUTTON3())
    { //������1
        keys |= KEY_RIGHT;
    }
    if (HAL_PUSH_BUTTON4())
    { //������1
        keys |= KEY_TOP;
    }
    if(keys == halKeySavedKeys)
    { /* Exit - since no keys have changed */
        return;
    }
    halKeySavedKeys = keys; /* Store the current keys for comparation next time */
    /* Invoke Callback if new keys were depressed */
    if(keys && (pHalKeyProcessFunction))
    {
        (pHalKeyProcessFunction)(keys);
    }
}

/******************************** endfile @ key ******************************/

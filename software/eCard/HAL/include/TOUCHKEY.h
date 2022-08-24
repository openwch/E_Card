/********************************** (C) COPYRIGHT *******************************
 * File Name          : KEY.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2016/04/12
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#ifndef __TOUCHKEY_H
#define __TOUCHKEY_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/
#define HAL_TOUCHKEY_POLLING_VALUE    100

/* Switches (keys) */
#define HAL_TOUCHKEY_SW_1             0x01  // key1
#define HAL_TOUCHKEY_SW_2             0x02  // key2
#define HAL_TOUCHKEY_SW_3             0x04  // key3
#define HAL_TOUCHKEY_SW_4             0x08  // key4

/* 按键定义 */

/* 1 - KEY */
#define TOUCHKEY1_PCENR               (RCC_APB2Periph_GPIOA)
#define TOUCHKEY2_PCENR               (RCC_APB2Periph_GPIOA)
#define TOUCHKEY3_PCENR               (RCC_APB2Periph_GPIOA)
#define TOUCHKEY4_PCENR               (RCC_APB2Periph_GPIOA)

#define TOUCHKEY1_GPIO                (GPIOA)
#define TOUCHKEY2_GPIO                (GPIOA)
#define TOUCHKEY3_GPIO                (GPIOA)
#define TOUCHKEY4_GPIO                (GPIOA)

#define TOUCHKEY1_BV                  BV(0)
#define TOUCHKEY2_BV                  BV(1)
#define TOUCHKEY3_BV                  BV(2)
#define TOUCHKEY4_BV                  BV(3)

#define TOUCHKEY1_IN                  (GPIO_ReadInputDataBit(KEY1_GPIO, KEY1_BV)==0)
#define TOUCHKEY2_IN                  (GPIO_ReadInputDataBit(KEY2_GPIO, KEY2_BV)==0)
#define TOUCHKEY3_IN                  (GPIO_ReadInputDataBit(KEY3_GPIO, KEY3_BV)==0)
#define TOUCHKEY4_IN                  (GPIO_ReadInputDataBit(KEY4_GPIO, KEY4_BV)==0)

#define HAL_TOUCHKEY1_PRESS()       (TOUCHKEY1_IN) //添加自定义按键
#define HAL_TOUCHKEY2_PRESS()       (TOUCHKEY2_IN)
#define HAL_TOUCHKEY3_PRESS()       (TOUCHKEY3_IN)
#define HAL_TOUCHKEY4_PRESS()       (TOUCHKEY4_IN)

/**************************************************************************************************
 * TYPEDEFS
 **************************************************************************************************/
typedef void (*halTouchKeyCBack_t)(uint8_t touchkeys);

typedef struct
{
    uint8_t touchkeys; // keys
} TouchkeyChange_t;

/**************************************************************************************************
 *                                             GLOBAL VARIABLES
 **************************************************************************************************/
extern uint16_t g_touchKeyVal[12];
/*********************************************************************
 * FUNCTIONS
 */

/**
 * @brief   Initialize the Key Service
 */
void HAL_TouchKeyInit(void);

/**
 * @brief   This is for internal used by hal_driver
 */
void HAL_TouchKeyPoll(void);

/**
 * @brief   Configure the Key serivce
 *
 * @param   cback - pointer to the CallBack function
 */
void Hal_TouchKeyConfig(const halTouchKeyCBack_t cback);

/**
 * @brief   Read the Key callback
 */
void Hal_TouchKeyCallback(uint8_t keys);

/**
 * @brief   Read the TouchKey status
 */
u16 Hal_TouchKeyRead(u8 ch);


u16 batteryRead(void);

void batteryMonitorInit(void);
/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
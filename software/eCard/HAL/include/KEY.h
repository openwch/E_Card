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
#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/
#define HAL_KEY_POLLING_VALUE    100

/* Switches (keys) */
#define HAL_KEY_SW_1             0x01  // key1
#define HAL_KEY_SW_2             0x02  // key2
#define HAL_KEY_SW_3             0x04  // key3
#define HAL_KEY_SW_4             0x08  // key4

/* 按键定义 */

/* 1 - KEY */
#define KEY1_PCENR               (RCC_APB2Periph_GPIOA)
#define KEY2_PCENR               (RCC_APB2Periph_GPIOA)
#define KEY3_PCENR               (RCC_APB2Periph_GPIOA)
#define KEY4_PCENR               (RCC_APB2Periph_GPIOA)

#define KEY1_GPIO                (GPIOA)
#define KEY2_GPIO                (GPIOA)
#define KEY3_GPIO                (GPIOA)
#define KEY4_GPIO                (GPIOA)

#define KEY1_BV                  BV(4)
#define KEY2_BV                  BV(5)
#define KEY3_BV                  BV(6)
#define KEY4_BV                  BV(7)

#define KEY1_IN                  (GPIO_ReadInputDataBit(KEY1_GPIO, KEY1_BV)==0)
#define KEY2_IN                  (GPIO_ReadInputDataBit(KEY2_GPIO, KEY2_BV)==0)
#define KEY3_IN                  (GPIO_ReadInputDataBit(KEY3_GPIO, KEY3_BV)==0)
#define KEY4_IN                  (GPIO_ReadInputDataBit(KEY4_GPIO, KEY4_BV)==0)

#define HAL_PUSH_BUTTON1()       (KEY1_IN) //添加自定义按键
#define HAL_PUSH_BUTTON2()       (KEY2_IN)
#define HAL_PUSH_BUTTON3()       (KEY3_IN)
#define HAL_PUSH_BUTTON4()       (KEY4_IN)

/**************************************************************************************************
 * TYPEDEFS
 **************************************************************************************************/
typedef void (*halKeyCBack_t)(uint8_t keys);

typedef struct
{
    uint8_t keys; // keys
} keyChange_t;

/**************************************************************************************************
 *                                             GLOBAL VARIABLES
 **************************************************************************************************/

/*********************************************************************
 * FUNCTIONS
 */

/**
 * @brief   Initialize the Key Service
 */
void HAL_KeyInit(void);

/**
 * @brief   This is for internal used by hal_driver
 */
void HAL_KeyPoll(void);

/**
 * @brief   Configure the Key serivce
 *
 * @param   cback - pointer to the CallBack function
 */
void HalKeyConfig(const halKeyCBack_t cback);

/**
 * @brief   Read the Key callback
 */
void HalKeyCallback(uint8_t keys);

/**
 * @brief   Read the Key status
 */
uint8_t HalKeyRead(void);

/**************************************************************************************************
**************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif

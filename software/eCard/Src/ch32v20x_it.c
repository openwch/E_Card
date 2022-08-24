/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v20x_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/06/16
 * Description        : Main Interrupt Service Routines.
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <ch32v20x_it.h>
#include "CONFIG.h"

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void BB_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void LLE_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
//void EXTI4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  None
 */
void NMI_Handler(void)
{
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  None
 */
void HardFault_Handler(void)
{
    while(1)
    {
    }
}

/*********************************************************************
 * @fn      BB_IRQHandler
 *
 * @brief   BB Interrupt for BLE.
 *
 * @return  None
 */
void BB_IRQHandler(void)
{
    BB_IRQLibHandler();
}

/*********************************************************************
 * @fn      LLE_IRQHandler
 *
 * @brief   LLE Interrupt for BLE.
 *
 * @return  None
 */
void LLE_IRQHandler(void)
{
    LLE_IRQLibHandler();
}

/*********************************************************************
 * @fn      EXTI4_IRQHandler
 *
 * @brief   This function handles EXTI0 Handler.
 *
 * @return  none
 */
//void EXTI4_IRQHandler(void)
//{
//  if(EXTI_GetITStatus(EXTI_Line4)!=RESET)
//  {
//    printf("PC:0x%08X\r\n",__get_MEPC());
//    EXTI_ClearITPendingBit(EXTI_Line4);     /* Clear Flag */
//  }
//}

/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2020/08/06
 * Description        : 外设从机应用主函数及任务系统初始化
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "HAL.h"
#include "Debug/debug.h"
#include "BLE/peripheral.h"
#include "QRcode/app_QRcode.h"
#include "LCD/app_lcd.h"
#include "SPIFLASH/spi_flash.h"
#include "BLE/lwns_adapter_blemesh_mac.h"
#include "BLE/lwns_name_tag.h"
#include "TFDB/tinyflashdb.h"
#include "RTC/app_rtc.h"

#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
/* Global define */

/* Global Variable */

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if (defined(BLE_MAC)) && (BLE_MAC == TRUE)
const uint8_t MacAddr[6] = {0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02};
#endif
/*********************************************************************
 * @fn      Main_Circulation
 *
 * @brief   tmos主循环.
 *
 * @param   None.
 *
 * @return  None.
 */
__attribute__((noinline))
__attribute__((section(".highcode"))) void
Main_Circulation()
{
    while (1)
    {
        TMOS_SystemProcess();
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   主函数
 *
 * @return  none
 */
int main(void)
{
    
    setvbuf(stdout, NULL, _IONBF, 0);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
#ifdef DEBUG
    USART_Printf_Init(460800);
#endif

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    Delay_Ms(200);
    if (!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4))
        g_flashProcessFlag = 1;

    if (g_flashProcessFlag)
    {
        LCD_InitHard();
        LCD_ClrScr(GREEN);
        Set_USBConfig();
        USB_Init();
        USB_Port_Set(DISABLE, DISABLE);
        Delay_Ms(700);
        USB_Port_Set(ENABLE, ENABLE);
        USB_Interrupts_Config();
        SPI_Flash_Update();
    }
    else
    {
        //    EXTI4_INT_INIT();
        SPI_Flash_Init();
        LCD_InitHard();
        LCD_DisplayPage_start(); //show startup interface
        APP_RTC_Init();
        WCHBLE_Init();          // Initialize BLE protocol stack
        HAL_Init();
        GAPRole_PeripheralInit();
        RF_RoleInit();
        RF_Init();
        lwns_init(); //Initialize lwns protocol stack
        lwns_name_tag_init();
        Peripheral_Init();
        LCD_PageRefresh(PAGE_1); // InitComplete
        Main_Circulation();         
    }
}

/******************************** endfile @ main ******************************/
//void EXTI4_INT_INIT(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure = {0};
//    EXTI_InitTypeDef EXTI_InitStructure = {0};
//    NVIC_InitTypeDef NVIC_InitStructure = {0};
//
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD, ENABLE);
//
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//
//    /* GPIOD ----> EXTI_Line4 */
//    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource4);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);
//
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}
/********************************** (C) COPYRIGHT  *******************************
 * File Name          : debug.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : This file contains all the functions prototypes for UART
 *                      Printf , Delay functions.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#include "stdio.h"
#include "ch32v20x.h"

/* UART Printf Definition */
#define DEBUG_UART1    1
#define DEBUG_UART2    2
#define DEBUG_UART3    3

/* DEBUG UATR Definition */
//#define DEBUG          DEBUG_UART1
//#define DEBUG   DEBUG_UART2
//#define DEBUG   DEBUG_UART3
#define RECEIVE_DATA_LEN		4096		//串口接收缓冲区大小
extern u8  USART_RX_BUF[RECEIVE_DATA_LEN]; 	//接收缓冲，最大USART_REC_LEN个字节，末字节为换行符
extern u16 USART_RX_STA;         		//接收状态标记

void Delay_Init(void);
void Delay_Us(uint32_t n);
void Delay_Ms(uint32_t n);
void USART_Printf_Init(uint32_t baudrate);
extern volatile uint32_t data_stat;
extern volatile uint32_t data_stat_test;
extern volatile uint8_t start_flag;
extern volatile uint8_t data_flag;

#if(DEBUG)
  #define PRINT(format, ...)    printf(format, ##__VA_ARGS__)
#else
  #define PRINT(X...)
#endif
#endif

/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_def.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : This file contains all the functions prototypes for the  
 *                      USB definition firmware library.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/ 
#ifndef __USB_DEF_H
#define __USB_DEF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v20x.h"

typedef enum _RECIPIENT_TYPE
{
  DEVICE_RECIPIENT,     
  INTERFACE_RECIPIENT,  
  ENDPOINT_RECIPIENT,   
  OTHER_RECIPIENT
} RECIPIENT_TYPE;

typedef enum _STANDARD_REQUESTS
{
  GET_STATUS = 0,
  CLEAR_FEATURE,
  RESERVED1,
  SET_FEATURE,
  RESERVED2,
  SET_ADDRESS,
  GET_DESCRIPTOR,
  SET_DESCRIPTOR,
  GET_CONFIGURATION,
  SET_CONFIGURATION,
  GET_INTERFACE,
  SET_INTERFACE,
  TOTAL_sREQUEST,  
  SYNCH_FRAME = 12
} STANDARD_REQUESTS;

/* Definition of "USBwValue" */
typedef enum _DESCRIPTOR_TYPE
{
  DEVICE_DESCRIPTOR = 1,
  CONFIG_DESCRIPTOR,
  STRING_DESCRIPTOR,
  INTERFACE_DESCRIPTOR,
  ENDPOINT_DESCRIPTOR
} DESCRIPTOR_TYPE;

/* Feature selector of a SET_FEATURE or CLEAR_FEATURE */
typedef enum _FEATURE_SELECTOR
{
  ENDPOINT_STALL,
  DEVICE_REMOTE_WAKEUP
} FEATURE_SELECTOR;

/* Definition of "USBbmRequestType" */
#define REQUEST_TYPE      0x60  
#define STANDARD_REQUEST  0x00  
#define CLASS_REQUEST     0x20 
#define VENDOR_REQUEST    0x40  
#define RECIPIENT         0x1F  


/* 类请求 */
//  3.1 Requests---Abstract Control Model
#define DEF_SEND_ENCAPSULATED_COMMAND  0x00
#define DEF_GET_ENCAPSULATED_RESPONSE  0x01
#define DEF_SET_COMM_FEATURE           0x02
#define DEF_GET_COMM_FEATURE           0x03
#define DEF_CLEAR_COMM_FEATURE         0x04
#define DEF_SET_LINE_CODING          0x20   // Configures DTE rate, stop-bits, parity, and number-of-character
#define DEF_GET_LINE_CODING          0x21   // This request allows the host to find out the currently configured line coding.
//#define DEF_SET_CTL_LINE_STE         0X22   // This request generates RS-232/V.24 style control signals.
#define DEF_SET_CONTROL_LINE_STATE     0x22
#define DEF_SEND_BREAK                 0x23

//  3.2 Notifications---Abstract Control Model
#define DEF_NETWORK_CONNECTION         0x00
#define DEF_RESPONSE_AVAILABLE         0x01
#define DEF_SERIAL_STATE               0x20

//Line Code结构
 typedef struct __packed _LINE_CODE
{
  uint32_t  BaudRate; /* 波特率 */
  uint8_t StopBits;   /* 停止位计数，0：1停止位，1：1.5停止位，2：2停止位 */
  uint8_t ParityType; /* 校验位，0：None，1：Odd，2：Even，3：Mark，4：Space */
  uint8_t DataBits;   /* 数据位计数：5，6，7，8，16 */
}LINE_CODE, *PLINE_CODE;



#ifdef __cplusplus
}
#endif

#endif /* __USB_DEF_H */







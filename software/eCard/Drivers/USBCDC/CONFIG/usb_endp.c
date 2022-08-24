/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_endp.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : Endpoint routines
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/ 
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define IN_FRAME_INTERVAL             5

/* Private variables */
uint8_t  USB_Rx_Buffer_1[USBD_DATA_SIZE];
uint8_t  USB_Rx_Buffer_2[USBD_DATA_SIZE];
uint16_t USB_Rx_Cnt_1 = 0;
uint16_t USB_Rx_Cnt_2 = 0;


uint8_t ep1_rx_flag = 0;  //0：没有数据 1：有数据
uint8_t ep1_rx_buf[USBD_DATA_SIZE];
uint8_t ep1_rx_len = 0;

uint8_t ep1_tx_flag = 1; //1：空闲  0：忙
uint8_t ep1_tx_buf[USBD_DATA_SIZE];
uint8_t ep1_tx_len = 0;

/*******************************************************************************
 * @fn        EP1_IN_Callback
 *
 * @brief     Endpoint 1 IN.
 *
 * @param     None.
 *
 * @return    None.
 */
void EP1_IN_Callback (void)
{
    ep1_tx_flag = 1;
    //SetEPTxValid(ENDP1);
    _SetEPTxStatus(ENDP1, EP_TX_NAK);

//    UserToPMABufferCopy(USB_Rx_Buffer_1, ENDP1_TXADDR, USB_Rx_Cnt_1);
//    SetEPTxCount(ENDP1, USB_Rx_Cnt_1);
//    SetEPTxValid(ENDP1);
}

/*******************************************************************************
 * @fn        EP1_OUT_Callback
 *
 * @brief     Endpoint 1 IN.
 *
 * @return    None.
 */
void EP1_OUT_Callback(void)
{
    ep1_rx_flag = 1;
    _SetEPRxStatus(ENDP1, EP_RX_NAK);
    ep1_rx_len = USB_SIL_Read(EP1_OUT, ep1_rx_buf);

//    uint8_t i;

//    USB_Rx_Cnt_1 = USB_SIL_Read(EP1_OUT, USB_Rx_Buffer_1);
//
//    for(i=0;i<USB_Rx_Cnt_1;i++)
//    {
//        USB_Rx_Buffer_1[i] = ~USB_Rx_Buffer_1[i];
//    }
//
//    EP1_IN_Callback( );
//    SetEPRxValid(ENDP1);
}

//端点1获取接收标志
uint8_t EP1CheckRecvSta(void)
{
    return ep1_rx_flag;
}

//端点1获取接收长度
uint8_t EP1CheckRecvLen(void)
{
    return ep1_rx_len;
}

//端点1获取数据内容
void EP1GetRecvData(uint8_t len ,uint8_t *p_dat)
{
    uint8_t i;
    uint8_t *p_save_dat;

    if(!ep1_rx_flag) return;

    p_save_dat = p_dat;
    for(i=0; i<len; i++)
    {
        *p_save_dat++ = ep1_rx_buf[i];
    }
    ep1_rx_flag = 0;
    _SetEPRxStatus(ENDP1, EP_RX_VALID);
}

//端点1查询发送标志
uint8_t EP1CheckSendSta(void)
{
    return ep1_tx_flag;
}

//端点1发送数据
void EP1SendData(uint8_t len ,uint8_t *p_dat)
{
    uint8_t i;
    uint8_t *p_send_data;

    if(len > 64) return;
    p_send_data = p_dat;
    //send_len = len;
    for(i=0; i<len; i++)
    {
        ep1_tx_buf[i] = *p_send_data++;
    }
    ep1_rx_flag = 0;
    ep1_tx_len = len;
    UserToPMABufferCopy(ep1_tx_buf, ENDP1_TXADDR, ep1_tx_len);
    SetEPTxCount(ENDP1, ep1_tx_len);
    SetEPTxValid(ENDP1);
}




/*******************************************************************************
 * @fn        EP2_IN_Callback
 *
 * @brief     Endpoint 2 IN.
 *
 * @param     None.
 *
 * @return    None.
 */
void EP2_IN_Callback (void)
{ 
//	UserToPMABufferCopy(USB_Rx_Buffer_2, ENDP2_TXADDR, USB_Rx_Cnt_2);
//	SetEPTxCount(ENDP2, USB_Rx_Cnt_2);
//	SetEPTxValid(ENDP2);
}

/*******************************************************************************
 * @fn        EP2_OUT_Callback
 *
 * @brief     Endpoint 2 IN.
 *
 * @return    None.
 */
void EP2_OUT_Callback(void)
{
//	uint8_t i;
//
//	USB_Rx_Cnt_2 = USB_SIL_Read(EP2_OUT, USB_Rx_Buffer_2);
//
//    for(i=0;i<USB_Rx_Cnt_2;i++)
//	{
//        USB_Rx_Buffer_2[i] = ~USB_Rx_Buffer_2[i];
//	}
//
//    EP2_IN_Callback( );
//	SetEPRxValid(ENDP2);
}

/*******************************************************************************
 * @fn       SOF_Callback
 *
 * @brief    SOF call back.
 *
 * @return   None.
 */
void SOF_Callback(void)
{
	static uint32_t FrameCount = 0;

	if(bDeviceState == CONFIGURED)
	{
		if (FrameCount++ == IN_FRAME_INTERVAL)
		{		
			FrameCount = 0;
			//Handle_USBAsynchXfer();
		}
	}  
}







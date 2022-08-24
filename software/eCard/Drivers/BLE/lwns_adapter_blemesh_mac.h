/********************************** (C) COPYRIGHT *******************************
 * File Name          : lwns_adapter_blemesh_mac.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/06/20
 * Description        : lwns��������ģ��ble sig mesh��macЭ��
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#ifndef _LWNS_ADAPTER_BLEMESH_MAC_H_
#define _LWNS_ADAPTER_BLEMESH_MAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lwns_config.h"

#define LWNS_USE_BLEMESH_MAC    1  //�Ƿ�ʹ��ģ��blemesh��macЭ�飬ע��ֻ��ʹ��һ��mac��Э�顣

#if LWNS_USE_BLEMESH_MAC

struct blemesh_mac_phy_manage_struct
{
    struct blemesh_mac_phy_manage_struct *next;
    uint8_t                              *data;
}; //ģ��blemesh mac�㷢�͹���ṹ��

  #define LWNS_MAC_TRANSMIT_TIMES           3     //һ�η��ͣ�����Ӳ�����ͼ���

  #define LWNS_MAC_PERIOD_MS                10    //mac�������ڣ������л�

  #define LWNS_MAC_SEND_DELAY_MAX_MS        10    //����mesh����10ms���ڵ��������

  #define LWNS_MAC_SEND_PACKET_MAX_NUM      16     //�����������֧�ּ������ݰ��ȴ�����

  #define BLE_PHY_ONE_PACKET_MAX_625US      5     //ble mac����һ������������Ҫ������

  #define LLE_MODE_ORIGINAL_RX              (0x80) //�������LLEMODEʱ���ϴ˺꣬����յ�һ�ֽ�Ϊԭʼ���ݣ�ԭ��ΪRSSI��

  #define LWNS_HTIMER_PERIOD_MS             20    //Ϊ(1000/HTIMER_SECOND_NUM)

    //RF_TX��RF_RX���õ����ͣ������޸ģ����Ƽ���
  #define USER_RF_RX_TX_TYPE                0xff

  #define LWNS_PHY_OUTPUT_TIMEOUT_MS        5

    //receive process evt
  #define LWNS_PHY_RX_OPEN_EVT              1
  #define LWNS_PHY_RX_CHANGE_CHANNEL_EVT    2
    //send process evt
  #define LWNS_HTIMER_PERIOD_EVT            1
  #define LWNS_PHY_OUTPUT_PREPARE_EVT       2
  #define LWNS_PHY_OUTPUT_FINISH_EVT        4

extern void RF_Init(void);

extern void lwns_init(void);

extern void lwns_shut(void);

extern void lwns_start(void);

  #ifdef __cplusplus
}
  #endif

#endif /* LWNS_USE_BLEMESH_MAC */

#endif /* _LWNS_ADAPTER_BLEMESH_MAC_H_ */

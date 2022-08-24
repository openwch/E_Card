/********************************** (C) COPYRIGHT *******************************
 * File Name          : lwns_config.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/11/17
 * Description        : lwns����ѡ��
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#ifndef _LWNS_CONFIG_H_
#define _LWNS_CONFIG_H_

#include "config.h"
#include "WCH_LWNS_LIB.h"

#define LWNS_ADDR_USE_BLE_MAC     1   //�Ƿ�ʹ������Ӳ����mac��ַ��ΪĬ��lwns��ַ

#define LWNS_ENCRYPT_ENABLE       0   //�Ƿ�ʹ�ܼ���

#define QBUF_MANUAL_NUM           4   //qbuf������������

#define ROUTE_ENTRY_MANUAL_NUM    0  //·����Ŀ��������

#define LWNS_NEIGHBOR_MAX_NUM     8   //����ھ�����

#endif /* _LWNS_CONFIG_H_ */

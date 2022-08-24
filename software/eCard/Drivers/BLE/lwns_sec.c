/********************************** (C) COPYRIGHT *******************************
 * File Name          : lwns_sec.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/09/17
 * Description        : lwns��Ϣ����
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#include "lwns_sec.h"

static uint8_t lwns_sec_key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; //�û����и���Ϊ�Լ�����Կ�����߸�Ϊ���Դ�������ȡ�����洢��eeprom��

/*********************************************************************
 * @fn      lwns_msg_encrypt
 *
 * @brief   lwns��Ϣ����
 *
 * @param   src     -   �����ܵ����ݻ���ͷָ��.
 * @param   to      -   ���洢�������ݵ����ݻ�����ͷָ��.
 * @param   mlen    -   �����ܵ����ݳ���.
 *
 * @return  ���ܺ�����ݳ���.
 */
int lwns_msg_encrypt(uint8_t *src, uint8_t *to, uint8_t mlen)
{
    uint16_t i = 0;
    uint8_t  esrc[16];
    while (1)
    {
        if ((mlen - i) < 16)
        {
            tmos_memcpy(esrc, src + i, (mlen - i)); //���䵽16�ֽڣ�����Ϊ0
            LL_Encrypt(lwns_sec_key, esrc, to + i);
        }
        else
        {
            LL_Encrypt(lwns_sec_key, src + i, to + i);
        }
        i += 16;
        if (i >= mlen)
        {
            break;
        }
    }
    return i; //���ؼ��ܺ����ݳ���
}

/*********************************************************************
 * @fn      lwns_msg_decrypt
 *
 * @brief   lwns��Ϣ����
 *
 * @param   src     -   �����ܵ����ݻ���ͷָ��.
 * @param   to      -   ���洢�������ݵ����ݻ�����ͷָ��.
 * @param   mlen    -   �����ܵ����ݳ��ȣ�����Ϊ16�ı���.
 *
 * @return  ���ܺ�����ݳ���.
 */
int lwns_msg_decrypt(uint8_t *src, uint8_t *to, uint8_t mlen)
{
    unsigned short i = 0;
    while (1)
    {
        LL_Decrypt(lwns_sec_key, src + i, to + i);
        i += 16;
        if (i >= mlen)
        {
            break;
        }
    }
    return i;
}

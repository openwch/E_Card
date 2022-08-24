/********************************** (C) COPYRIGHT *******************************
 * File Name          : ble_control.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : Jul 20, 2022
 * Description        :
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#include "ble_control.h"
#include "lwns_name_tag.h"
#include <RTC/app_rtc.h>
#include <LCD/app_lcd.h>

name_tag_time_t g_name_tag_time;
uint32_t name_pic_addr; /* �������ͼƬ���ݵ�д���ַ */

static void ble_control_read(uint8_t *pValue, uint16_t *pLen);
static void ble_control_write(uint8_t *pValue, uint16_t len);

static rwn_profile_CBs_t ble_control_callbacks =
{
    .pfnrwn_profile_Read  = ble_control_read,
    .pfnrwn_profile_Write = ble_control_write,
};

void ble_control_init()
{
    rwn_profile_RegisterAppCBs(&ble_control_callbacks);
    people_name_init();
}

void ble_control_Notify(uint8_t *pValue, uint16_t len)
{
    attHandleValueNoti_t noti;

    if (len > (peripheralMTU - 3))
    {
        PRINT("Too large noti\n");
        return;
    }

    noti.len    = len;
    noti.pValue = GATT_bm_alloc(peripheralConnList.connHandle, ATT_HANDLE_VALUE_NOTI, noti.len, NULL, 0);

    if (noti.pValue)
    {
        tmos_memcpy(noti.pValue, pValue, noti.len);

        if (rwn_profile_Notify(peripheralConnList.connHandle, &noti) != SUCCESS)
        {
            GATT_bm_free((gattMsg_t *)&noti, ATT_HANDLE_VALUE_NOTI);
        }
    }
}

static void ble_control_read(uint8_t *pValue, uint16_t *pLen)
{
    PRINT("ble_control_read\n");
}

static void ble_control_write(uint8_t *pValue, uint16_t len)
{
    static uint8_t ble_set_name_seq = 0xff;
    uint32_t rw_data[(BLE_BUFF_MAX_LEN - 3) / 4];
    uint8_t resp[4];
    ble_control_packet_t *p = (ble_control_packet_t *)pValue;
    PRINT("ble_control_write:%d\n", len);
    ((ble_control_packet_t *)(resp))->cmd = p->cmd;
    ((ble_control_packet_t *)(resp))->seq = p->seq;
    ((ble_control_packet_t *)(resp))->len = 0;

    if (len >= 4)
    {
        switch (p->cmd)
        {
            case BLE_CONTROL_CMD_SET_NAME_FONT:
                /* �Ȳ������ݣ�׼������ */
                ble_set_name_seq                      = 0;
                ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_NO_ERR;
                tfdb_port_erase(NAME_PIC_DATA_ADDR, 2 * 1024); /* Ԥ��10k�������� */
                name_pic_addr = NAME_PIC_DATA_ADDR;
                break;

            case BLE_CONTROL_CMD_SET_NAME_FONT_DATA:
                /* ��д������ */
                if (ble_set_name_seq == p->seq)
                {
                    ble_set_name_seq++; /* ������һ�ε�seq */
                    tmos_memcpy(rw_data, p->data, p->len);
                    if (tfdb_port_write(name_pic_addr, (uint8_t *)rw_data, p->len) != TFDB_NO_ERR)
                    {
                        ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_ERR_DATA;
                    }
                    name_pic_addr                         = name_pic_addr + p->len;
                    ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_NO_ERR;
                }
                else
                {
                    //��Ŵ���
                    ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_ERR_SEQ;
                }

                break;

            case BLE_CONTROL_CMD_SET_NAME_FONT_END:
                /* ��������ɹ� */
                ble_set_name_seq = 0xff;

                if ((p->len > 0) && (p->len <= PEOPLE_NAME_MAX_LEN))
                {
                    ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_NO_ERR;
                    save_people_name((char *)p->data, p->len); /* �洢���� */
                    g_enter_main_sign  = 1;
                    LCD_PageRefresh(PAGE_3);
                }
                else
                {
                    ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_ERR_DATA;
                }

                /* ��ʼ�ϱ� */
                #if ENABLE_SIGN_WORK /* ǩ�������Ƿ���ǰ��С������ */
                tmos_set_event(lwns_name_tag_taskID, LWNS_NAME_TAG_SIGN_EVT);
                #endif
                // tmos_start_task(lwns_name_tag_taskID, LWNS_NAME_TAG_TEST_EVT, MS1_TO_SYSTEM_TIME(1000));
                break;

            case BLE_CONTROL_CMD_SYNC_TIME:

                /* ����ʱ�� */
                if ((p->len == 7)
                        && (((name_tag_time_t *)(p->data))->format.year >= 2022)
                        && (((name_tag_time_t *)(p->data))->format.mouth <= 12)
                        && (((name_tag_time_t *)(p->data))->format.mouth >= 1)
                        && (((name_tag_time_t *)(p->data))->format.day >= 1)
                        && (((name_tag_time_t *)(p->data))->format.day <= 31)
                        && (((name_tag_time_t *)(p->data))->format.hour < 24)
                        && (((name_tag_time_t *)(p->data))->format.minute < 60)
                        && (((name_tag_time_t *)(p->data))->format.seconds < 60))
                {
                    tmos_memcpy(g_name_tag_time.data, p->data, 7);
                    APP_RTC_Set(g_name_tag_time.format.year,
                                g_name_tag_time.format.mouth,
                                g_name_tag_time.format.day,
                                g_name_tag_time.format.hour,
                                g_name_tag_time.format.minute,
                                g_name_tag_time.format.seconds);
                    ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_NO_ERR; /* ����ʱ��ɹ� */
                    PRINT("time set:%d/%d/%d %d:%d\n", g_name_tag_time.format.year, g_name_tag_time.format.mouth,
                          g_name_tag_time.format.day, g_name_tag_time.format.hour, g_name_tag_time.format.minute);
                }
                else
                {
                    ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_ERR_DATA; /* ����ʱ�����ݳ��� */
                }
                break;

            case 0xff:
                return;

            default:
                ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_ERR_CMD;
                break;
        }
    }
    else
    {
        ((ble_control_packet_t *)(resp))->err = BLE_CONTROL_ERR_DATA;
    }

    ble_control_Notify(resp, 4);
}

/* ��һ���ֽ�Ϊ���� */
uint8_t g_people_name[PEOPLE_NAME_MAX_LEN + 1];

const tfdb_index_t people_name_index =
{
    .end_byte     = 0x00,
    .flash_addr   = 0x08077000,
    .flash_size   = 256,
    .value_length = PEOPLE_NAME_MAX_LEN + 1,
};
tfdb_addr_t people_name_addr = 0; //��ַ����

/*********************************************************************
 * @fn      save_people_name
 *
 * @brief   �洢��������
 *
 * @param   name - �������ݴ�ŵ�������ָ��
 * @param   len - �������ݳ���
 *
 * @return  TFDB_Err_Code
 */
TFDB_Err_Code save_people_name(char *name, uint8_t len)
{
    uint32_t rw_buf[((PEOPLE_NAME_MAX_LEN + 1 + 2 + 3) / 4)];
    TFDB_Err_Code err;
    g_people_name[0] = len;

    if(len)
    {
        tmos_memcpy(g_people_name + 1, name, len);
    }
    err = tfdb_set(&people_name_index, (uint8_t *)rw_buf, &people_name_addr, g_people_name);

    if (err == TFDB_NO_ERR)
    {
        PRINT("save ok\n");
    }
    else
    {
        PRINT("save failed\n");
    }

    return err;
}

/*********************************************************************
 * @fn      read_people_name
 *
 * @brief   ��ȡ��������
 *
 * @param   name - �������ݶ�ȡ���ŵ�������ָ��
 *
 * @return  TFDB_Err_Code
 */
TFDB_Err_Code read_people_name(char *name)
{
    uint32_t rw_buf[((PEOPLE_NAME_MAX_LEN + 1 + 2 + 3) / 4)];
    TFDB_Err_Code err;
    err = tfdb_get(&people_name_index, (uint8_t *)rw_buf, &people_name_addr, name);

    if (err == TFDB_NO_ERR)
    {
        PRINT("get ok\n");
    }
    else
    {
        name[0] = 0;
        PRINT("get failed\n");
    }

    return err;
}

/*********************************************************************
 * @fn      people_name_init
 *
 * @brief   �ϵ��ʼ���������ݣ�����������ݺϷ��������ǿ�ƽ���������
 *
 * @param   void
 *
 * @return  uint8_t ��������,Ϊ0������
 */
uint8_t people_name_init(void)
{
    read_people_name((char *)g_people_name);

    if (g_people_name[0])
    {
        PRINT("people name len:%d\n", g_people_name[0]);
        PRINT("people name data:");

        for (uint8_t i = 1; i < g_people_name[0] + 1; i++)
        {
            PRINT("%02x ", g_people_name[i]);
        }

        PRINT("\n");
    }

    return g_people_name[0];
}

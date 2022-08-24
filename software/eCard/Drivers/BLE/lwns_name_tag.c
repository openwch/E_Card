/********************************** (C) COPYRIGHT *******************************
 * File Name          : lwns_name_tag.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/07/21
 * Description        :
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "lwns_name_tag.h"
#include "lwns_show_interface.h"

static lwns_addr_t master_addr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}; //�����ڵ��ַ

static uint16_t lwns_name_tag_ProcessEvent(uint8_t task_id, uint16_t events);
static int netflood_recv(lwns_controller_ptr ptr, const lwns_addr_t  *from, const lwns_addr_t *originator, uint8_t hops);
static void uninetflood_recv(lwns_controller_ptr ptr, const lwns_addr_t *sender, uint8_t hops); //�������緺����ջص�����
static lwns_netflood_controller netflood;           //���緺����ƽṹ��
static lwns_uninetflood_controller uninetflood;     //�������緺����ƽṹ��
ble_control_packet_t *default_lwns_dataptr;
uint8_t lwns_name_tag_taskID;
uint8_t draw_state = LWNS_NAME_TAG_DRAW_STATE_FREE;
uint8_t vote_state = LWNS_NAME_TAG_VOTE_STATE_FREE;
static uint8_t gs_vote_num = 0, gs_show_conference_num = 0;
uint8_t g_enter_main_sign = 0;

static int netflood_recv(lwns_controller_ptr ptr,
                         const lwns_addr_t  *from,
                         const lwns_addr_t *originator, uint8_t hops)
{
    ble_control_packet_t *packet;
    uint8_t len;
    if ((g_enter_main_sign != 0))         /* �����������ú����Ӧ��Ϣ ,(g_people_name[0] > 0) && */
    {
        if (lwns_addr_cmp(&master_addr, originator))
        {
            len = lwns_buffer_datalen(); //��ȡ��ǰ���������յ������ݳ���
            PRINT("netflood_recv:%d\n", len);
            if (len >= 4)
            {
                packet = lwns_buffer_dataptr();
                PRINT("packet_len:%d\n", packet->len);
                if (packet->len == (len - 4))
                {
                    PRINT("packet->cmd:%d\n", packet->cmd);
                    switch (packet->cmd)
                    {

                    case LWNS_NAME_TAG_NETFLOOD_CMD_DRAW_START:
                        /* ��ʾ��ʼ�齱���棬�رհ�����Ӧ */
                        if(packet->len == 0)
                        {
                            if (draw_state == LWNS_NAME_TAG_DRAW_STATE_FREE)
                            {
                                show_draw_start();
                                draw_state = LWNS_NAME_TAG_DRAW_STATE_WAIT;
                            }
                        }
                        break;
                    case LWNS_NAME_TAG_NETFLOOD_CMD_DRAW_END:
                        /* ��ʾ�����齱���棬�򿪰�����Ӧ */
                        if(packet->len == 0)
                        {
                            if (draw_state == LWNS_NAME_TAG_DRAW_STATE_WAIT)
                            {
                                show_draw_end();
                                draw_state = LWNS_NAME_TAG_DRAW_STATE_FREE;
                            }
                        }
                        break;
                    case LWNS_NAME_TAG_NETFLOOD_CMD_VOTE_START:
                        /* ��ʾͶƱ���� */
                        if(packet->len == 1)
                        {
                            if (vote_state == LWNS_NAME_TAG_VOTE_STATE_FREE)
                            {
                                gs_vote_num = packet->data[0];
                                vote_state = LWNS_NAME_TAG_VOTE_STATE_WAIT;
                                show_vote_start();
                            }
                        }
                        break;
                    case LWNS_NAME_TAG_NETFLOOD_CMD_VOTE_END:
                        /* ��ʾͶƱ���� */
                        if(packet->len == 0)
                        {
                            if (vote_state == LWNS_NAME_TAG_VOTE_STATE_WAIT)
                            {
                                gs_vote_num = 0;
                                vote_state = LWNS_NAME_TAG_VOTE_STATE_FREE;
                                show_vote_end();
                            }
                        }
                        break;
                    case LWNS_NAME_TAG_NETFLOOD_CMD_SHOW_CONFERENCE:
                        if(packet->len == 1)
                        {
                            if (gs_show_conference_num != packet->data[0])
                            {
                                gs_show_conference_num = packet->data[0];
                                show_conference(gs_show_conference_num);
                            }
                        }
                        break;

                    case 0xff:
                    	if((packet->len == 0) && (packet->err == 0xff))
                    	{
                    		char aname = 0;
                    		save_people_name(&aname, 0);
                    		show_testinit();
                    	}
                    	break;
                    default:
                        break;
                    }
                }
            }
        }
    }
    return 1;       //����1���򱾽ڵ㽫��������netflood��ת������
    //return 0;     //����0���򱾽ڵ㲻�ټ���netflood��ֱ����ֹ
}

static void uninetflood_recv(lwns_controller_ptr ptr, const lwns_addr_t *sender, uint8_t hops)
{
    ble_control_packet_t *packet;
    uint8_t len;
    if ((g_enter_main_sign != 0))         /* �����������ú����Ӧ��Ϣ,(g_people_name[0] > 0) &&  */
    {
        len = lwns_buffer_datalen();        //��ȡ��ǰ���������յ������ݳ���
        PRINT("uninetflood_recv:%d\n", len);
        if (lwns_addr_cmp(&master_addr, sender))
        {
            if (len >= 4)
            {
                packet = lwns_buffer_dataptr();
                PRINT("packet_len:%d\n", packet->len);
                if (packet->len == (len - 4))
                {
                    PRINT("packet->cmd:%d\n", packet->cmd);
                    switch (packet->cmd)
                    {
                    case LWNS_NAME_TAG_UNINETFLOOD_CMD_GET_VOTE:
                        /* ��ȡͶƱ��� */
                        if(packet->len == 1)
                        {
                            if (vote_state == LWNS_NAME_TAG_VOTE_STATE_WAIT)
                            {
                                if(gs_vote_num == packet->data[0])
                                {
                                    tmos_set_event(lwns_name_tag_taskID, LWNS_NAME_TAG_VOTE_EVT);
                                }
                            }
                        }
                        break;
                    case LWNS_NAME_TAG_UNINETFLOOD_CMD_DRAW_REQ:
                        /* ѯ���н����Ƿ��ڣ��ڵĻ����ͻظ�������ʾ�н����棬������н�������ӦLWNS_NAME_TAG_NETFLOOD_CMD_DRAW_END�н������� */
                        if(packet->len == 1)
                        {
                            if (draw_state == LWNS_NAME_TAG_DRAW_STATE_WAIT)
                            {
                                draw_state = LWNS_NAME_TAG_DRAW_STATE_WIN;
                                show_draw_win(packet->data[0]);
                                tmos_set_event(lwns_name_tag_taskID, LWNS_NAME_TAG_LUCKY_DRAW_WIN_EVT);
                            }
                        }
                        break;
                    case LWNS_NAME_TAG_UNINETFLOOD_CMD_SIGN_CONFIRM:
                        /* ǩ���ɹ���ֹͣ�ط� */
                        tmos_stop_task(lwns_name_tag_taskID, LWNS_NAME_TAG_SIGN_EVT);
                        tmos_clear_event(lwns_name_tag_taskID, LWNS_NAME_TAG_SIGN_EVT);
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}

/**
 * lwns ���緺��ص������ṹ�壬ע��ص�����
 */
static struct lwns_netflood_callbacks callbacks =
{
    netflood_recv,
    NULL,
    NULL
};

/**
 * lwns �������緺��ص������ṹ�壬ע��ص�����
 */
static struct lwns_uninetflood_callbacks uninetflood_callbacks =
{
    uninetflood_recv,
    NULL
};

static uint16_t lwns_name_tag_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if (events & LWNS_NAME_TAG_SIGN_EVT)
    {
        /* ǩ�������������������������󣬿�ʼǩ�� */
        lwns_buffer_clear();
        default_lwns_dataptr->cmd = LWNS_NAME_TAG_UNINETFLOOD_CMD_SIGN;
        default_lwns_dataptr->len = 0;
        default_lwns_dataptr->seq = 0;
        default_lwns_dataptr->err = 0;
        lwns_buffer_set_datalen(4);
        lwns_uninetflood_send(&uninetflood, &master_addr);
        tmos_start_task(lwns_name_tag_taskID, LWNS_NAME_TAG_SIGN_EVT, MS1_TO_SYSTEM_TIME(500) + tmos_rand() % 256);    /* ����ǩ���ɹ�   */
        return (events ^ LWNS_NAME_TAG_SIGN_EVT);
    }

    if (events & LWNS_NAME_TAG_VOTE_EVT)
    {
        /* ��ȡͶƱ��� */
        PRINT("give vote\n");
        lwns_buffer_clear();
        default_lwns_dataptr->cmd = LWNS_NAME_TAG_UNINETFLOOD_CMD_GIVE_VOTE;
        default_lwns_dataptr->len = 2;
        default_lwns_dataptr->seq = 0;
        default_lwns_dataptr->err = 0;
        default_lwns_dataptr->data[0] = gs_vote_num;
        default_lwns_dataptr->data[1] = get_vote_value();       /* ͶƱ��� */
        lwns_buffer_set_datalen(6);
        lwns_uninetflood_send(&uninetflood, &master_addr);
        return (events ^ LWNS_NAME_TAG_VOTE_EVT);
    }

    if (events & LWNS_NAME_TAG_LUCKY_DRAW_WIN_EVT)
    {
        /* ��ȡͶƱ��� */
        lwns_buffer_clear();
        default_lwns_dataptr->cmd = LWNS_NAME_TAG_UNINETFLOOD_CMD_DRAW_CONFIRM;
        default_lwns_dataptr->len = g_people_name[0];
        default_lwns_dataptr->seq = 0;
        default_lwns_dataptr->err = 0;
        tmos_memcpy(default_lwns_dataptr->data, g_people_name + 1, g_people_name[0]);
        lwns_buffer_set_datalen(4 + g_people_name[0]);
        lwns_uninetflood_send(&uninetflood, &master_addr);
        return (events ^ LWNS_NAME_TAG_LUCKY_DRAW_WIN_EVT);
    }

    if (events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;
        if ((pMsg = tmos_msg_receive(task_id)) != NULL)
        {
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    return 0;
}

void lwns_name_tag_init()
{
    lwns_netflood_init(&netflood,
                       137,                   //��һ���˿ں�Ϊ137�ķ���ṹ��
                       HTIMER_SECOND_NUM / 4, //�ȴ�ת��ʱ��
                       1,                     //�ڵȴ��ڼ䣬���յ�����ͬ�������ݰ���ȡ�������ݰ��ķ���
                       3,                     //���ת���㼶
                       TRUE,                  //�ڵȴ�ת�������У��յ����µ���Ҫת�������ݰ��������ݰ������̷��ͳ�ȥ���Ƕ�����FALSEΪ���̷��ͣ�TRUEΪ������
                       100,                   //����ָ�����
                       &callbacks);           //����0�����ʧ�ܡ�����1�򿪳ɹ���

    lwns_uninetflood_init(&uninetflood,
                          138,                     //��һ���˿ں�Ϊ138�ĵ������緺��ṹ��
                          HTIMER_SECOND_NUM / 4,   //���ȴ�ת��ʱ��
                          1,                       //�ڵȴ��ڼ䣬���յ�����ͬ�������ݰ���ȡ�������ݰ��ķ���
                          3,                       //���ת���㼶
                          TRUE,                    //�ڵȴ�ת�������У��յ����µ���Ҫת�������ݰ��������ݰ������̷��ͳ�ȥ���Ƕ�����FALSEΪ���̷��ͣ�TRUEΪ������
                          100,                     //����ָ�����
                          TRUE,                    //�����Ƿ�ת��Ŀ��Ǳ��������ݰ�������������mesh�Ƿ�����relay���ܡ�
                          &uninetflood_callbacks); //����0�����ʧ�ܡ�����1�򿪳ɹ���

    lwns_name_tag_taskID = TMOS_ProcessEventRegister(lwns_name_tag_ProcessEvent);
    lwns_buffer_clear();
    default_lwns_dataptr = lwns_buffer_dataptr();   /* ��ȡĬ�ϵ�������ָ�� */
}

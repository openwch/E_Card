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

static lwns_addr_t master_addr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x01}}; //主机节点地址

static uint16_t lwns_name_tag_ProcessEvent(uint8_t task_id, uint16_t events);
static int netflood_recv(lwns_controller_ptr ptr, const lwns_addr_t  *from, const lwns_addr_t *originator, uint8_t hops);
static void uninetflood_recv(lwns_controller_ptr ptr, const lwns_addr_t *sender, uint8_t hops); //单播网络泛洪接收回调函数
static lwns_netflood_controller netflood;           //网络泛洪控制结构体
static lwns_uninetflood_controller uninetflood;     //单播网络泛洪控制结构体
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
    if ((g_enter_main_sign != 0))         /* 姓名必须设置后才响应消息 ,(g_people_name[0] > 0) && */
    {
        if (lwns_addr_cmp(&master_addr, originator))
        {
            len = lwns_buffer_datalen(); //获取当前缓冲区接收到的数据长度
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
                        /* 显示开始抽奖画面，关闭按键响应 */
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
                        /* 显示结束抽奖画面，打开按键响应 */
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
                        /* 显示投票画面 */
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
                        /* 显示投票结束 */
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
    return 1;       //返回1，则本节点将继续发送netflood，转发数据
    //return 0;     //返回0，则本节点不再继续netflood，直接终止
}

static void uninetflood_recv(lwns_controller_ptr ptr, const lwns_addr_t *sender, uint8_t hops)
{
    ble_control_packet_t *packet;
    uint8_t len;
    if ((g_enter_main_sign != 0))         /* 姓名必须设置后才响应消息,(g_people_name[0] > 0) &&  */
    {
        len = lwns_buffer_datalen();        //获取当前缓冲区接收到的数据长度
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
                        /* 获取投票结果 */
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
                        /* 询问中奖者是否在，在的话发送回复，并显示中奖画面，如果已中奖，则不响应LWNS_NAME_TAG_NETFLOOD_CMD_DRAW_END中奖结束。 */
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
                        /* 签到成功，停止重发 */
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
 * lwns 网络泛洪回调函数结构体，注册回调函数
 */
static struct lwns_netflood_callbacks callbacks =
{
    netflood_recv,
    NULL,
    NULL
};

/**
 * lwns 单播网络泛洪回调函数结构体，注册回调函数
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
        /* 签到任务，在蓝牙传输设置姓名后，开始签到 */
        lwns_buffer_clear();
        default_lwns_dataptr->cmd = LWNS_NAME_TAG_UNINETFLOOD_CMD_SIGN;
        default_lwns_dataptr->len = 0;
        default_lwns_dataptr->seq = 0;
        default_lwns_dataptr->err = 0;
        lwns_buffer_set_datalen(4);
        lwns_uninetflood_send(&uninetflood, &master_addr);
        tmos_start_task(lwns_name_tag_taskID, LWNS_NAME_TAG_SIGN_EVT, MS1_TO_SYSTEM_TIME(500) + tmos_rand() % 256);    /* 必须签到成功   */
        return (events ^ LWNS_NAME_TAG_SIGN_EVT);
    }

    if (events & LWNS_NAME_TAG_VOTE_EVT)
    {
        /* 获取投票结果 */
        PRINT("give vote\n");
        lwns_buffer_clear();
        default_lwns_dataptr->cmd = LWNS_NAME_TAG_UNINETFLOOD_CMD_GIVE_VOTE;
        default_lwns_dataptr->len = 2;
        default_lwns_dataptr->seq = 0;
        default_lwns_dataptr->err = 0;
        default_lwns_dataptr->data[0] = gs_vote_num;
        default_lwns_dataptr->data[1] = get_vote_value();       /* 投票结果 */
        lwns_buffer_set_datalen(6);
        lwns_uninetflood_send(&uninetflood, &master_addr);
        return (events ^ LWNS_NAME_TAG_VOTE_EVT);
    }

    if (events & LWNS_NAME_TAG_LUCKY_DRAW_WIN_EVT)
    {
        /* 获取投票结果 */
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
                       137,                   //打开一个端口号为137的泛洪结构体
                       HTIMER_SECOND_NUM / 4, //等待转发时间
                       1,                     //在等待期间，接收到几次同样的数据包就取消本数据包的发送
                       3,                     //最大转发层级
                       TRUE,                  //在等待转发过程中，收到了新的需要转发的数据包，旧数据包是立刻发送出去还是丢弃，FALSE为立刻发送，TRUE为丢弃。
                       100,                   //网络恢复参数
                       &callbacks);           //返回0代表打开失败。返回1打开成功。

    lwns_uninetflood_init(&uninetflood,
                          138,                     //打开一个端口号为138的单播网络泛洪结构体
                          HTIMER_SECOND_NUM / 4,   //最大等待转发时间
                          1,                       //在等待期间，接收到几次同样的数据包就取消本数据包的发送
                          3,                       //最大转发层级
                          TRUE,                    //在等待转发过程中，收到了新的需要转发的数据包，旧数据包是立刻发送出去还是丢弃，FALSE为立刻发送，TRUE为丢弃。
                          100,                     //网络恢复参数
                          TRUE,                    //本机是否转发目标非本机的数据包，类似于蓝牙mesh是否启用relay功能。
                          &uninetflood_callbacks); //返回0代表打开失败。返回1打开成功。

    lwns_name_tag_taskID = TMOS_ProcessEventRegister(lwns_name_tag_ProcessEvent);
    lwns_buffer_clear();
    default_lwns_dataptr = lwns_buffer_dataptr();   /* 获取默认的数据区指针 */
}

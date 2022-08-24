/*
 * lwns_name_tag.h
 *
 *  Created on: Jul 21, 2022
 *      Author: OWNER
 */

#ifndef _LWNS_NAME_TAG_H_
#define _LWNS_NAME_TAG_H_

#include "lwns_config.h"
#include "ble_control.h"

#define LWNS_NAME_TAG_SIGN_EVT                  1ul << (0)
#define LWNS_NAME_TAG_LUCKY_DRAW_WIN_EVT        1ul << (1)
#define LWNS_NAME_TAG_VOTE_EVT                  1ul << (2)

/* 主机netflood命令 */
enum
{
    LWNS_NAME_TAG_NETFLOOD_CMD_DRAW_START = 0x20,
    LWNS_NAME_TAG_NETFLOOD_CMD_DRAW_END,
    LWNS_NAME_TAG_NETFLOOD_CMD_VOTE_START,
    LWNS_NAME_TAG_NETFLOOD_CMD_VOTE_END,
    LWNS_NAME_TAG_NETFLOOD_CMD_SHOW_CONFERENCE,
};

/* 主机uninetflood命令 */
enum
{
    LWNS_NAME_TAG_UNINETFLOOD_CMD_SIGN_CONFIRM = 0x60,
    LWNS_NAME_TAG_UNINETFLOOD_CMD_GET_VOTE,
    LWNS_NAME_TAG_UNINETFLOOD_CMD_DRAW_REQ,
};

/* 从机uninetflood命令 */
enum
{
    LWNS_NAME_TAG_UNINETFLOOD_CMD_SIGN = 0xa0,
    LWNS_NAME_TAG_UNINETFLOOD_CMD_GIVE_VOTE,
    LWNS_NAME_TAG_UNINETFLOOD_CMD_DRAW_CONFIRM,
};

enum
{
    LWNS_NAME_TAG_DRAW_STATE_FREE = 0,
    LWNS_NAME_TAG_DRAW_STATE_WAIT,
    LWNS_NAME_TAG_DRAW_STATE_WIN,
};

enum
{
    LWNS_NAME_TAG_VOTE_STATE_FREE = 0,
    LWNS_NAME_TAG_VOTE_STATE_WAIT,
    LWNS_NAME_TAG_VOTE_STATE_END,
};

extern uint8_t lwns_name_tag_taskID;

extern void lwns_name_tag_init();

extern uint8_t g_enter_main_sign;

#endif /* _LWNS_NAME_TAG_H_ */

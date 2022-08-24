/********************************** (C) COPYRIGHT *******************************
 * File Name          : ble_control.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : Jul 20, 2022
 * Description        :
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef _BLE_CONTROL_H_
#define _BLE_CONTROL_H_

#include "config.h"
#include "Profile/rwn_profile.h"
#include "TFDB/tinyflashdb.h"
#include "peripheral.h"

#define NAME_PIC_DATA_ADDR      0x0806B800

#define PEOPLE_NAME_MAX_LEN     19          /* 防止MTU协商失败，按照默认的进行存储 */
extern uint8_t g_people_name[PEOPLE_NAME_MAX_LEN + 1];

enum
{
    BLE_CONTROL_CMD_TEST = 0,               /* 测试通讯 */
    BLE_CONTROL_CMD_SET_NAME_FONT,          /* 开始准备设置姓名  */
    BLE_CONTROL_CMD_SET_NAME_FONT_DATA,     /* 传输姓名数据 */
    BLE_CONTROL_CMD_SET_NAME_FONT_END,      /* 传输姓名结束 */
    BLE_CONTROL_CMD_SYNC_TIME,              /* 时间同步 */
    BLE_CONTROL_CMD_MAX,
};

enum
{
    BLE_CONTROL_NO_ERR = 0,
    BLE_CONTROL_ERR_DATA,
    BLE_CONTROL_ERR_CMD,
    BLE_CONTROL_ERR_SEQ,
};

typedef struct ble_control_packet_struct
{
    uint8_t cmd;
    uint8_t err;
    uint8_t seq;
    uint8_t len;
    uint8_t data[BLE_BUFF_MAX_LEN - 4];
} ble_control_packet_t;

typedef union __attribute__((packed)) _name_tag_time_union
{
    struct {
        uint16_t year;
        uint8_t mouth;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t seconds;
    } format;
    uint8_t data[7];
}name_tag_time_t;

extern name_tag_time_t g_name_tag_time;

extern void ble_control_init(void);
extern void ble_control_Notify(uint8_t *pValue, uint16_t len);
extern TFDB_Err_Code save_people_name(char *name, uint8_t len);
extern TFDB_Err_Code read_people_name(char *name);
extern uint8_t people_name_init(void);

#endif /* _BLE_CONTROL_H_ */

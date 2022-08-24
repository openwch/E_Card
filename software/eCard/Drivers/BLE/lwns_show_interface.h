/********************************** (C) COPYRIGHT *******************************
 * File Name          : lwns_show_interface.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : Jul 26, 2022
 * Description        :
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#ifndef _LWNS_SHOW_INTERFACE_H_
#define _LWNS_SHOW_INTERFACE_H_

#include "config.h"

extern void show_vote_start();

extern void show_vote_end();

extern void show_draw_start();

extern void show_draw_end();

extern void show_draw_win(uint8_t level);

extern uint32_t get_vote_value();

extern void show_conference(uint8_t num);

extern void show_testinit(void);

#endif /* _LWNS_SHOW_INTERFACE_H_ */

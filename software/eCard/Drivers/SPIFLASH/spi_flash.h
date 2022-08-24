/********************************** (C) COPYRIGHT  *******************************
 * File Name          : spi_flash.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/07/26
 * Description        : This file contains all the functions prototypes for SPI flash.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include <stdio.h>
#include <string.h>
#include "ch32v20x.h"

/* Winbond SPIFalsh ID */
#define W25Q80                   0XEF13
#define W25Q16                   0XEF14
#define W25Q32                   0XEF15
#define W25Q64                   0XEF16
#define W25Q128                  0XEF17

/* Winbond SPIFalsh Instruction List */
#define W25X_WriteEnable         0x06
#define W25X_WriteDisable        0x04
#define W25X_ReadStatusReg       0x05
#define W25X_WriteStatusReg      0x01
#define W25X_ReadData            0x03
#define W25X_FastReadData        0x0B
#define W25X_FastReadDual        0x3B
#define W25X_PageProgram         0x02
#define W25X_BlockErase          0xD8
#define W25X_SectorErase         0x20
#define W25X_ChipErase           0xC7
#define W25X_PowerDown           0xB9
#define W25X_ReleasePowerDown    0xAB
#define W25X_DeviceID            0xAB
#define W25X_ManufactDeviceID    0x90
#define W25X_JedecDeviceID       0x9F

#define TotalSize   0x200000 /* 总容量 = 2M */
#define PageSize    0x1000   /* 页面大小 = 4K */


/* 串行Flash的片选GPIO端口 */
#define SF_RCC_CS 			RCC_AHB1Periph_GPIOB
#define SF_PORT_CS			GPIOB
#define SF_PIN_CS			GPIO_Pin_12
#define SF_CS_0()			SF_PORT_CS->BCR = SF_PIN_CS
#define SF_CS_1()			SF_PORT_CS->BSHR = SF_PIN_CS

#define SPI_CS(n)           GPIO_WriteBit(GPIOB, GPIO_Pin_12, (n));
#define SIZE                sizeof(TEXT_Buf)
extern const u8 TEXT_Buf[25];
#define Size 4
u8 TxData[Size];
u8 RxData[Size];
extern uint16_t str_loc;
extern u16 W25QXX_TYPE;	
extern uint8_t g_flashProcessFlag;
extern volatile uint16_t g_usbconnflage;

u8 SPI_ReadWriteByte(u8 TxData);
void SPI2_Init(void);
void SPI_Flash_Init(void);
void W25QXX_Init(void);
u8 SPI_Flash_ReadSR(void);
void SPI_FLASH_Write_SR(u8 sr);
void SPI_Flash_Wait_Busy(void);
void SPI_FLASH_Write_Enable(void);
void SPI_FLASH_Write_Disable(void);
u16 SPI_Flash_ReadID(void);
void SPI_Flash_Erase_Sector(u32 Dst_Addr);
void SPI_Flash_Erase_Block(u32 Dst_Addr);
void SPI_Flash_Read(u8 *pBuffer, u32 ReadAddr, u16 size);
void SPI_Flash_Write_Page(u8 *pBuffer, u32 WriteAddr, u16 size);
void SPI_Flash_Write_NoCheck(u8 *pBuffer, u32 WriteAddr, u16 size);
void SPI_Flash_Write(u8 *pBuffer, u32 WriteAddr, u16 size);
void SPI_Flash_Erase_Chip(void);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);
void SPI_Flash_Test(void);
void SPI_DMA_Send( u8 *buff, u32 len );
void SPI_DMA_Recive( u8 *buff, u32 len );
void SPI_DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);
void SPI_DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);
void SPI_FLASH_SAVE_BMP(u8 *pBuffer, u32 WriteAddr, u32 size);
void SPI_Flash_Update(void);

#endif
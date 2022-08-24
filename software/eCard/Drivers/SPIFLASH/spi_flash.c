/*
 *@Note
 SPI接口操作FLASH外设例程：
 Master：SPI2_SCK(PB13)、SPI2_MISO(PB14)、SPI2_MOSI(PB15)。
 本例程演示 SPI 操作 Winbond W25Qxx SPIFLASH。
 注：
 pins:
    CS   —— PB12
    DO   —— PB14(SPI2_MISO)
    WP   —— 3.3V
    DI   —— PB15(SPI2_MOSI)
    CLK  —— PB13(SPI2_SCK)
    HOLD —— 3.3V

*/
#include "spi_flash.h"
#include "Debug/debug.h"
#include "LCD/app_lcd.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
/* Global define */
// #define SIZE    sizeof(TEXT_Buf)
/* Global Variable */
const u8 TEXT_Buf[25] = {"CH32V208 SPI FLASH"};

uint16_t str_loc = 0;

/* Global define */
// #define Size 4

/* Global Variable */
u8 TxData[Size] = { W25X_ManufactDeviceID,0X00,0X00,0X00 };
u8 RxData[Size];
u16 W25QXX_TYPE;
uint8_t g_flashProcessFlag = 0;
volatile uint16_t g_usbconnflage = 1;//0,连接成功；非0，没有连接上

void DMA1_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      SPI_ReadWriteByte
 *
 * @brief   SPI1 read or write one byte.
 *
 * @param   TxData - write one byte data.
 *
 * @return  Read one byte data.
 */
u8 SPI_ReadWriteByte(u8 TxData)
{
    u8 i = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
    {
        i++;
        if (i > 200)
            return 0;
    }

    SPI_I2S_SendData(SPI2, TxData);
    i = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    {
        i++;
        if (i > 200)
            return 0;
    }

    return SPI_I2S_ReceiveData(SPI2);
}
/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void SPI_DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr     = memadr;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = bufsize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

    DMA_Cmd(DMA_CHx, DISABLE);
}

/*********************************************************************
 * @fn      DMA_Rx_Init
 *
 * @brief   Initializes the SPI1 DMA Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void SPI_DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr     = memadr;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = bufsize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

    DMA_ITConfig(DMA_CHx, DMA_IT_TC, ENABLE); //开启 DMA1_Channel4 传输完成中断
    DMA_ITConfig(DMA_CHx, DMA_IT_TE, ENABLE); //开启 DMA1_Channel4 传输错误中断
    /* DISABLE SPI2 DMA TX request */
    DMA_Cmd(DMA_CHx, DISABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/****************************************************************************************
 * @fn      SPI2_Init
 *
 * @brief   Configuring the SPI for operation flash.
 *
 * @return  none
 */
void SPI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure   = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12; // CS
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13; // CLK
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14; // MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15; // MOSI
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI2, &SPI_InitStructure);
}
/****************************************************************************************
 * @fn      SPI_Flash_Init
 *
 * @brief   Configuring the SPI for operation flash.
 *
 * @return  none
 */
void SPI_Flash_Init(void)
{
    SPI2_Init();

    SPI2->HSCR |= 0x0001;
    SPI2->CTLR2 |= 0X0003;//发送\接收缓冲区DMA使能
    SPI_Cmd(SPI2, ENABLE);

    SPI_DMA_Tx_Init( DMA1_Channel5, (u32)&SPI2->DATAR, (u32)spiDmaSendBuf, IMAGE_BUF_SIZE );
	SPI_DMA_Rx_Init( DMA1_Channel4, (u32)&SPI2->DATAR, (u32)imageBuf1, IMAGE_BUF_SIZE );
}


/*******************************************************************************
* Function Name  : SPI2_DMA_Send
* Description    : SPI2的DMA方式发送
* Input          : buff[len]
* Output         : None
* Return         : None
* Attention             : 关闭DMA通道5之前必须等待TXE为1，等待忙标志为0
*******************************************************************************/

/****************************************************************************************
 * @fn      SPI_DMA_Send
 * @brief   SPI2的DMA方式发送
 * 
 * @param   buff 发送的内存地址
 * @param   len  发送的数据长度
 * @return  void
 * @note    关闭DMA通道5之前必须等待TXE为1，等待忙标志为0
 */
void SPI_DMA_Send( u8 *buff, u32 len )
{
    DMA1_Channel5->CFGR &= (uint16_t)(~DMA_CFGR1_EN); //关闭DMA通道5
    DMA1_Channel5->PADDR = (u32)&SPI2->DATAR;         //外设地址
    DMA1_Channel5->MADDR = (u32)buff;                 // mem地址
    DMA1_Channel5->CNTR  = len;                       //传输长度
    DMA1_Channel5->CFGR |= (1U);                      // 通道开启
}

/****************************************************************************************
 * @fn      SPI_DMA_Recive
 * @brief   SPI2的DMA方式接收
 * 
 * @param   buff 接收的内存地址
 * @param   len  接收的数据长度
 * @return  void
 * @note    必须要先关闭通道4,然后再配置通道4的参数
 */
void SPI_DMA_Recive(u8 *buff, u32 len)
{
    DMA1_Channel4->CFGR &= (uint16_t)(~DMA_CFGR1_EN); //关闭通道4
    DMA1_Channel4->PADDR = (u32)&SPI2->DATAR;         //外设地址
    DMA1_Channel4->MADDR = (uint32_t)buff;            // mem地址
    DMA1_Channel4->CNTR  = len;                       //传输长度
    DMA1_Channel4->CFGR |= (1U);                      //通道开启
}
/*********************************************************************
 * @fn      SPI_Flash_ReadSR
 *
 * @brief   Read W25Qxx status register.
 *        ——BIT7  6   5   4   3   2   1   0
 *        ——SPR   RV  TB  BP2 BP1 BP0 WEL BUSY
 *
 * @return  byte - status register value.
 */
u8 SPI_Flash_ReadSR(void)
{
    u8 byte = 0;

    SF_CS_0();
    SPI_ReadWriteByte(W25X_ReadStatusReg);
    byte = SPI_ReadWriteByte(0Xff);
    SF_CS_1();

    return byte;
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_SR
 *
 * @brief   Write W25Qxx status register.
 *
 * @param   sr - status register value.
 *
 * @return  none
 */
void SPI_FLASH_Write_SR(u8 sr)
{
    SF_CS_0();
    SPI_ReadWriteByte(W25X_WriteStatusReg);
    SPI_ReadWriteByte(sr);
    SF_CS_1();
}

/*********************************************************************
 * @fn      SPI_Flash_Wait_Busy
 *
 * @brief   Wait flash free.
 *
 * @return  none
 */
void SPI_Flash_Wait_Busy(void)
{
    while ((SPI_Flash_ReadSR() & 0x01) == 0x01)
        ;
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_Enable
 *
 * @brief   Enable flash write.
 *
 * @return  none
 */
void SPI_FLASH_Write_Enable(void)
{
    SF_CS_0();
    SPI_ReadWriteByte(W25X_WriteEnable);
    SF_CS_1();
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_Disable
 *
 * @brief   Disable flash write.
 *
 * @return  none
 */
void SPI_FLASH_Write_Disable(void)
{
    SF_CS_0();
    SPI_ReadWriteByte(W25X_WriteDisable);
    SF_CS_1();
}

/*********************************************************************
 * @fn      SPI_Flash_ReadID
 *
 * @brief   Read flash ID.
 *
 * @return  Temp - FLASH ID.
 */
u16 SPI_Flash_ReadID(void)
{
    u16 Temp = 0;

    SF_CS_0();
    SPI_ReadWriteByte(W25X_ManufactDeviceID);
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    Temp |= SPI_ReadWriteByte(0xFF) << 8;
    Temp |= SPI_ReadWriteByte(0xFF);
    SF_CS_1();

    return Temp;
}

/*********************************************************************
 * @fn      SPI_Flash_Erase_Sector
 *
 * @brief   Erase one sector(4Kbyte).
 *
 * @param   Dst_Addr - 0 —— 2047
 *
 * @return  none
 */
void SPI_Flash_Erase_Sector(u32 Dst_Addr)
{
    Dst_Addr *= 4096;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    SF_CS_0();
    SPI_ReadWriteByte(W25X_SectorErase);
    SPI_ReadWriteByte((u8)((Dst_Addr) >> 16));
    SPI_ReadWriteByte((u8)((Dst_Addr) >> 8));
    SPI_ReadWriteByte((u8)Dst_Addr);
    SF_CS_1();
    SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_Erase_Block
 *
 * @brief   Erase one block(64Kbyte).
 *
 * @param   Dst_Addr - 0 —— 2047
 *
 * @return  none
 */
void SPI_Flash_Erase_Block(u32 Dst_Addr)
{
    Dst_Addr *= 65536;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    SF_CS_0();
    SPI_ReadWriteByte(W25X_BlockErase);
    SPI_ReadWriteByte((u8)((Dst_Addr) >> 16));
    SPI_ReadWriteByte((u8)((Dst_Addr) >> 8));
    SPI_ReadWriteByte((u8)Dst_Addr);
    SF_CS_1();
    SPI_Flash_Wait_Busy();
}
/*********************************************************************
 * @fn      SPI_Flash_Read
 *
 * @brief   Read data from flash.
 *
 * @param   pBuffer -
 *          ReadAddr -Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Read(u8 *pBuffer, u32 ReadAddr, u16 size)
{
    u16 i;

    SF_CS_0();
    SPI_ReadWriteByte(W25X_ReadData);
    SPI_ReadWriteByte((u8)((ReadAddr) >> 16));
    SPI_ReadWriteByte((u8)((ReadAddr) >> 8));
    SPI_ReadWriteByte((u8)ReadAddr);

    for (i = 0; i < size; i++)
    {
        pBuffer[i] = SPI_ReadWriteByte(0XFF);
    }

    SF_CS_1();
}

/****************************************************************************************
 * @fn      SPI_Flash_Write_Page
 *
 * @brief   Write data by one page.
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write_Page(u8 *pBuffer, u32 WriteAddr, u16 size)
{
    u16 i;

    SPI_FLASH_Write_Enable();
    SF_CS_0();
    SPI_ReadWriteByte(W25X_PageProgram);
    SPI_ReadWriteByte((u8)((WriteAddr) >> 16));
    SPI_ReadWriteByte((u8)((WriteAddr) >> 8));
    SPI_ReadWriteByte((u8)WriteAddr);

    for (i = 0; i < size; i++)
    {
        SPI_ReadWriteByte(pBuffer[i]);
    }

    SF_CS_1();
    SPI_Flash_Wait_Busy();
}

/****************************************************************************************
 * @fn      SPI_Flash_Write_NoCheck
 *
 * @brief   Write data to flash.(need Erase)
 *          All data in address rang is 0xFF.
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write_NoCheck(u8 *pBuffer, u32 WriteAddr, u16 size)
{
    u16 pageremain;

    pageremain = 256 - WriteAddr % 256;

    if (size <= pageremain)
        pageremain = size;

    while (1)
    {
        SPI_Flash_Write_Page(pBuffer, WriteAddr, pageremain);

        if (size == pageremain)
        {
            break;
        }
        else
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;
            size -= pageremain;

            if (size > 256)
                pageremain = 256;
            else
                pageremain = size;
        }
    }
}

/****************************************************************************************
 * @fn      SPI_Flash_Write
 *
 * @brief   Write data to flash.(no need Erase)
 *
 * @param   pBuffer -  write data.
 * @param   WriteAddr - Initial address(24bit).
 * @param   size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write(u8 *pBuffer, u32 WriteAddr, u16 size)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;

    secpos    = WriteAddr / 4096;
    secoff    = WriteAddr % 4096;
    secremain = 4096 - secoff;

    if (size <= secremain)
        secremain = size;

    while (1)
    {
        SPI_Flash_Read(USART_RX_BUF, secpos * 4096, 4096);

        for (i = 0; i < secremain; i++)
        {
            if (USART_RX_BUF[secoff + i] != 0XFF)
                break;
        }

        if (i < secremain)
        {
            SPI_Flash_Erase_Sector(secpos);

            for (i = 0; i < secremain; i++)
            {
                USART_RX_BUF[i + secoff] = pBuffer[i];
            }

            SPI_Flash_Write_NoCheck(USART_RX_BUF, secpos * 4096, 4096);
        }
        else
        {
            SPI_Flash_Write_NoCheck(pBuffer, WriteAddr, secremain);
        }

        if (size == secremain)
        {
            break;
        }
        else
        {
            secpos++;
            secoff = 0;

            pBuffer += secremain;
            WriteAddr += secremain;
            size -= secremain;

            if (size > 4096)
            {
                secremain = 4096;
            }
            else
            {
                secremain = size;
            }
        }
    }
}

/****************************************************************************************
 * @fn      SPI_Flash_Erase_Chip
 *
 * @brief   Erase all FLASH pages.
 *
 * @return  none
 */
void SPI_Flash_Erase_Chip(void)
{
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    SF_CS_0();
    SPI_ReadWriteByte(W25X_ChipErase);
    SF_CS_1();
    SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_PowerDown
 *
 * @brief   Enter power down mode.
 *
 * @return  none
 */
void SPI_Flash_PowerDown(void)
{
    SF_CS_0();
    SPI_ReadWriteByte(W25X_PowerDown);
    SF_CS_1();
    Delay_Us(3);
}

/*********************************************************************
 * @fn      SPI_Flash_WAKEUP
 *
 * @brief   Power down wake up.
 *
 * @return  none
 */
void SPI_Flash_WAKEUP(void)
{
    SF_CS_0();
    SPI_ReadWriteByte(W25X_ReleasePowerDown);
    SF_CS_1();
    Delay_Us(3);
}


/*********************************************************************
 * @fn      SPI_Flash_Test
 *
 * @brief   Flash读写测试
 *
 * @return  none
 */
void SPI_Flash_Test(void)
{
    u8 datap[SIZE];
    u16 Flash_Model;  
    uint8_t stringBUFF[26];

    Flash_Model = SPI_Flash_ReadID();

    switch (Flash_Model)
    {
        case W25Q80:
            sprintf((char *)stringBUFF, "W25Q80 OK!");
            break;

        case W25Q16:
            sprintf((char *)stringBUFF, "W25Q16 OK!");
            break;

        case W25Q32:
            sprintf((char *)stringBUFF, "W25Q32 OK!");
            break;

        case W25Q64:
            sprintf((char *)stringBUFF, "W25Q64 OK!");
            break;

        case W25Q128:
            sprintf((char *)stringBUFF, "W25Q128 OK!");
            break;

        default:
            sprintf((char *)stringBUFF, "Fail!");
            break;
    }
    PRINT("%s\r\n",stringBUFF);

    Delay_Ms(500);
    PRINT("Start Read W25Qxx....\r\n");
    SPI_Flash_Read(datap, 0x0, SIZE);
    for(int i = 0;i<SIZE;i++)
    PRINT("%X", *(datap+i));
    PRINT("\r\n");
    Delay_Ms(500);

    PRINT("Start Erase W25Qxx....\r\n");
    SPI_Flash_Erase_Sector(0);
    PRINT("W25Qxx Erase Finished!\r\n");
    Delay_Ms(500);

    PRINT("Start Read W25Qxx....\r\n");
    SPI_Flash_Read(datap, 0x0, SIZE);
    for(int i = 0;i<SIZE;i++)
    PRINT("%X", *(datap+i));
    PRINT("\r\n");

    Delay_Ms(500);
    PRINT("Start Write W25Qxx....\r\n");
    SPI_Flash_Write((u8 *)TEXT_Buf, 0, SIZE);
    PRINT("W25Qxx Write Finished!\r\n");
    Delay_Ms(500);
    PRINT("Start Read W25Qxx....\r\n");
    SPI_Flash_Read(datap, 0x0, SIZE);
    PRINT("%s\r\n", datap);

}

void SPI_FLASH_SAVE_BMP(u8 *pBuffer, u32 WriteAddr, u32 size)
{
    u32 totalsize  = size;
    u16 sectornum  = 0;
    u16 remainlen  = 0;
    u16 i          = 0;

    if (totalsize > 0xffff)
    {
        sectornum = totalsize / 0xffff;
        remainlen = totalsize % 0xffff;

        for (i = 0; i < sectornum; i++)
        {
            SPI_Flash_Write(pBuffer + i * 0xffff, WriteAddr + i * 0xffff, 0xffff);
        }

        SPI_Flash_Write(pBuffer + i * 0xffff, WriteAddr + i * 0xffff, remainlen);
    }
    else
    {
        SPI_Flash_Write(pBuffer + i * 0xffff, WriteAddr + i * 0xffff, remainlen);
    }
}

//SPI2 RX DMA中断
void DMA1_Channel4_IRQHandler(void)
{
    
    if (DMA_GetITStatus(DMA1_FLAG_TC4))
    {
        DMA_ClearFlag(DMA1_FLAG_TC4);

        if (imageBufFlag && FLAG_IMAGE_REFUSH_START)
        {
            if (DMA1_Channel4->MADDR == (uint32_t)imageBuf1)
            {
                nextImageBuf = FLAG_IMAGE_BUF1;
                imageBufFlag |= FLAG_IMAGE_BUF1_MAX;
            }
            else if (DMA1_Channel4->MADDR == (uint32_t)imageBuf2)
            {
                nextImageBuf = FLAG_IMAGE_BUF2;
                imageBufFlag |= FLAG_IMAGE_BUF2_MAX;
            }
        }
        else
        {
            imageBufFlag |= FLAG_IMAGE_HEADER;
        }
    }
}

void W25QXX_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure   = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12; // CS
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13; // CLK
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14; // MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15; // MOSI
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI2, &SPI_InitStructure);
    SPI2->HSCR |= 0x0001;
    SPI_Cmd(SPI2, ENABLE);                     //初始化SPI
    W25QXX_TYPE = SPI_Flash_ReadID(); //读取FLASH ID.
}

/**
 * @fn  SPI_Flash_Update
 * @brief 外置Flash数据更新。配合 W25Qxx串口下载助手.exe软件使用
 *        芯片上电或复位时按下User Key 进入FLASH UPDATE 模式
 */
void SPI_Flash_Update(void)
{
    uint8_t usbstatus = 0;

    char usbsend[64] = {0};



    PRINT("***************************************\r\n");
    PRINT("start flash Process...\r\n");
    u32 i                 = 0;
    uint32_t addr_start   = 0;
    uint32_t file_len     = 0;
    uint32_t receive_flag = 0;
    uint32_t start        = 0;
    uint32_t end          = 0;
    uint8_t receive_len   = 0;
    uint16_t screen_color[10] = {RED,BLUE,YELLOW,GREEN,WHITE,BLACK,GRAY,BROWN,PURPLE,PINK};
    uint8_t screen_color_idx = 0;
    uint8_t screen_color_flag = 1;

    W25QXX_Init(); // W25QXX初始化
    while (SPI_Flash_ReadID() != W25Q64) //检测不到W25Q64
    {
        i++;
        if (i >= 10000)
        {
        	PRINT("W25QXX Check Failed!\n");
            return;
        }
    }

    switch (W25QXX_TYPE)
    {
        case W25Q80:
        	PRINT("W25Q80 Successful");
            break;

        case W25Q16:
        	PRINT("W25Q16 Successful");
            break;

        case W25Q32:
        	PRINT("W25Q32 Successful");
            break;

        case W25Q64:
        	PRINT("W25Q64 Successful");
            break;

        case W25Q128:
        	PRINT("W25Q128 Successful");
            break;

        default:
        	PRINT("W25Qxx Fail!");
            break;
    }
    PRINT("!\r\n");

    USART_RX_BUF[0]  = 0; //上电清除指令接收缓冲区，防止内部数据不确定造成判断失误
    USART_RX_BUF[1]  = 0;
    USART_RX_BUF[10] = 0;
    while (1)
    {
        if (g_usbconnflage)
        {
            if (usbstatus != bDeviceState)
            {
                usbstatus = bDeviceState;

                if (usbstatus == CONFIGURED)
                {
                    g_usbconnflage = 0;
                }
                else
                {
                    g_usbconnflage++;
                    if (g_usbconnflage > 50000)
                    {
                        g_usbconnflage = 1;
                        PRINT("USB disConnected\r\n");
                    }
                }
            }
        }
        else
        {
            if (start_flag == 0)
            {
                if (USART_RX_BUF[0] == 0xFE && USART_RX_BUF[1] == 0xFF && USART_RX_BUF[10] == 0xFE) //等待接指令、写入地址和文件长度
                {

                    start_flag   = 1;         //清空串口接收长度，准备接收数据
                    data_stat    = 0;         //清空数据接收
                    receive_flag = data_stat; //

                    addr_start = 0;
                    addr_start |= USART_RX_BUF[2];
                    addr_start |= USART_RX_BUF[3] << 8;
                    addr_start |= USART_RX_BUF[4] << 16;
                    addr_start |= USART_RX_BUF[5] << 24; //获取存放数据的起始位置

                    file_len = 0;
                    file_len |= USART_RX_BUF[6];
                    file_len |= USART_RX_BUF[7] << 8;
                    file_len |= USART_RX_BUF[8] << 16;
                    file_len |= USART_RX_BUF[9] << 24; //获取存放文件长度
                    Delay_Ms(5);

                    while (!EP1CheckSendSta())
                        ;
                    sprintf(usbsend, "start---\r\naddr_start=%ld file_size=%ld\r\n", addr_start, file_len); //串口提示已经收到数据接收命令 准备接收数据
                    EP1SendData(strlen(usbsend), (uint8_t *)usbsend);                                       //从串口要写入文件的起始地址和要写入文件的大小
                    start = addr_start / 65536;                                                             // RECEIVE_DATA_LEN
                    end   = (file_len + addr_start + 65535) / 65536;                                        //+ (((file_len + addr_start) % 65536) > 0 ? 1 : 0)

                    while (!EP1CheckSendSta())
                        ;
                    sprintf(usbsend, "Start erasing Block\r\n"); //从串口提示要擦除需要写入的扇区
                    EP1SendData(strlen(usbsend), (uint8_t *)usbsend);
                    for (i = start; i < end; i++) //擦除需要些入文件的扇区
                    {
                        SPI_Flash_Erase_Block(i); //块擦除
                        while (!EP1CheckSendSta())
                            ;
                        sprintf(usbsend, "Erasing Block %ld\r\n", i);
                        EP1SendData(strlen(usbsend), (uint8_t *)usbsend);
                    }
//                    while (!EP1CheckSendSta())
//                        ;
//                    sprintf(usbsend, "Block erasing is complete\r\n");
//                    EP1SendData(strlen(usbsend), (uint8_t *)usbsend);
//                    while (!EP1CheckSendSta())
//                        ;
//                    sprintf(usbsend, "Start writing data\r\n");
//                    EP1SendData(strlen(usbsend), (uint8_t *)usbsend);
                    while (!EP1CheckSendSta())
                        ;
                    sprintf(usbsend, "%s", "NA"); //回应上位机已经接收到接收数据命令，已经准备好等待接收上位机发送的数据
                    EP1SendData(strlen(usbsend), (uint8_t *)usbsend);
                }
            }
            else if (start_flag == 1) //开始向W25Qxx中写入数据
            {
                if (data_stat % RECEIVE_DATA_LEN == 0 && receive_flag != data_stat) //从缓存区中读取接收到文件的数据块，写入W25Qxx中		注释：这里的一个文件块为4096Byte
                {
                    SPI_Flash_Write_NoCheck(USART_RX_BUF, addr_start + data_stat - RECEIVE_DATA_LEN, RECEIVE_DATA_LEN);
                    receive_flag = data_stat;
                    Delay_Ms(50);
                    while (!EP1CheckSendSta())
                        ;
                    sprintf(usbsend, " %ld%s", data_stat, "A"); //回应上位机已经将缓冲区中的数据块写入W25Qxx中等待接收下一个数据块
                    EP1SendData(strlen(usbsend), (uint8_t *)usbsend);
                }
                else if (data_stat >= file_len)
                {
                    SPI_Flash_Write_NoCheck(USART_RX_BUF, addr_start + data_stat - (file_len % RECEIVE_DATA_LEN), file_len % RECEIVE_DATA_LEN); //写入文件最后一个块 该数据块不是完整的数据块小于4096Byte
                    // PRINT("\r\n%s\r\n", "Write to file successfully-----");
                    while (!EP1CheckSendSta())
                        ;
                    sprintf(usbsend, "Write to file successfully-----");
                    EP1SendData(strlen(usbsend), (uint8_t *)usbsend);
                    while (!EP1CheckSendSta())
                        ;
                    sprintf(usbsend, "File Size %ld Byte\r\n", data_stat);
                    EP1SendData(strlen(usbsend), (uint8_t *)usbsend);
                    // PRINT("File Size %ld Byte\r\n", data_stat);
                    Delay_Ms(100);
                    NVIC_SystemReset(); //接收数据完成,复位重启
                    // while (1)
                    //     ;
                }
            }

            if (EP1CheckRecvSta()) //当前有接收数据
            {
                receive_len = EP1CheckRecvLen();                                            //读取接收到的数据
                EP1GetRecvData(receive_len, USART_RX_BUF + (data_stat % RECEIVE_DATA_LEN)); //将接收到的指令和数据放入缓冲区
                data_stat += receive_len;
            }
        }

        if (!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4) && screen_color_flag)
        {
            screen_color_flag = 0;
        }

        if (!screen_color_flag)
        {
            if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4))
            {
                screen_color_flag = 1;
                if (screen_color_idx > 9) screen_color_idx = 0;
                LCD_ClrScr(screen_color[screen_color_idx]);
                screen_color_idx++;
            }
        }
    }
}

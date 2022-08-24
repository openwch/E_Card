/*
 * Copyright (c) 2022, smartmx - smartmx@qq.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of the Tiny Flash DataBase Library.
 * 
 * Change Logs:
 * Date           Author       Notes
 * 2022-02-03     smartmx      the first version
 * 2022-02-08     smartmx      fix bugs
 * 2022-02-12     smartmx      fix bugs, add support for 2 byte write flash
 * 2022-03-15     smartmx      fix bugs, add support for stm32l4 flash
 * 2022-08-02     smartmx      add TFDB_VALUE_AFTER_ERASE_SIZE option
 *
 */
#include "tfdb_port.h"

/**
 * Read data from flash.
 * @note This operation's units is refer to TFDB_WRITE_UNIT_BYTES.
 *
 * @param addr flash address.
 * @param buf buffer to store read data.
 * @param size read bytes size.
 *
 * @return TFDB_Err_Code
 */
TFDB_Err_Code tfdb_port_read(tfdb_addr_t addr, uint8_t *buf, size_t size)
{
    TFDB_Err_Code result = TFDB_NO_ERR;
    /* You can add your code under here. */
    uint32_t *rbuf = (uint32_t *)addr;
    uint32_t *rbuf_to = (uint32_t *)buf;
    while(size)
    {
        *rbuf_to = *rbuf;
        rbuf++;
        rbuf_to++;
        size = size - 4;
    }
    return result;
}

/**
 * Erase flash.
 * @param addr flash address.
 * @param size erase bytes size.
 *
 * @return TFDB_Err_Code
 */
TFDB_Err_Code tfdb_port_erase(tfdb_addr_t addr, size_t size)
{
    TFDB_Err_Code result = TFDB_NO_ERR;
    /* You can add your code under here. */

    __disable_irq();
    FLASH_Unlock_Fast();

    while(size >= 256)
    {
        FLASH_ErasePage_Fast(addr);
        size = size - 256;
        addr = addr + 256;
    }

    FLASH_Lock_Fast();
    __enable_irq();

    return result;
}

/**
 * Write data to flash.
 * @note This operation's units is refer to TFDB_WRITE_UNIT_BYTES.
 * if you're using some flash like stm32L4xx, please add flash check
 * operations before write flash to ensure the write area is erased.
 * if the write area is not erased, please just return TFDB_NO_ERR.
 * TFDB will check data and retry at next address.
 *
 * @param addr flash address.
 * @param buf the write data buffer.
 * @param size write bytes size.
 *
 * @return result
 */
TFDB_Err_Code tfdb_port_write(tfdb_addr_t addr, const uint8_t *buf, size_t size)
{
    TFDB_Err_Code result = TFDB_NO_ERR;
    uint32_t *wbuf = (uint32_t *)buf;
    /* You can add your code under here. */
    __disable_irq();
    FLASH_Unlock_Fast();

    while(size)
    {
        if(FLASH_ProgramWord(addr, *wbuf) != FLASH_COMPLETE)
        {
            result = TFDB_WRITE_ERR;
            break;
        }
        wbuf++;
        size = size - 4;
        addr = addr + 4;
    }

    FLASH_Lock_Fast();
    __enable_irq();

    return result;
}




/***********************************************************************************************************************
 * File Name    : header.h
 * Description  : Contains application image header information and related function prototypes
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef HEADER_H_
#define HEADER_H_

#include <dsps_downloader_thread.h>
#include "qspi_ep.h"

#if 0
#define PRIMARY_IMAGE_START_ADDRESS      0x00010000
#define PRIMARY_IMAGE_END_ADDRESS        0x0007FFFF
#define SECONDARY_IMAGE_START_ADDRESS    0x00080000
#define SECONDARY_IMAGE_END_ADDRESS      0x000EFFFF

#define FLASH_PAGE_PROGRAMMING_SIZE (128)
#else
#define PRIMARY_IMAGE_START_ADDRESS      0x00018000
#define PRIMARY_IMAGE_END_ADDRESS        0x000F7FFF
#define SECONDARY_IMAGE_START_ADDRESS    0x60000000
#define SECONDARY_IMAGE_END_ADDRESS      0x600DFFFF

#define FLASH_PAGE_PROGRAMMING_SIZE (256)

#endif
#define FLASH_BLOCK_SIZE                    (32 * 1024)
#define SECONDARY_IMAGE_NUM_BLOCKS          ((SECONDARY_IMAGE_END_ADDRESS - SECONDARY_IMAGE_START_ADDRESS + 1) / FLASH_BLOCK_SIZE)
#define SECONDARY_IMAGE_NUM_SECTORS          ((SECONDARY_IMAGE_END_ADDRESS - SECONDARY_IMAGE_START_ADDRESS + 1) / SECTOR_SIZE)


#define IMAGE_HEADER                (0xA5A5A5A5)
#define IMAGE_HEADER_SIZE           (4)
#define IMAGE_LENGTH_SIZE           (4)
#define RING_BUFFER_LW_MARK         (RING_BUFFER_SIZE/4)


#endif /* HEADER_H_ */

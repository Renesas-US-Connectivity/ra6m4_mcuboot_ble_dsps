/***********************************************************************************************************************
 * File Name    : downloader_thread_entry.c
 * Description  : Contains Downloader Thread Implementation
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <dsps_downloader_thread.h>
#include "ring_buffer.h"
#include "header.h"
#include "uart_ble.h"
/* DSPS Downloader entry function */
/* pvParameters contains TaskHandle_t */
#include "qspi_operations.h"

TaskHandle_t s_dsps_downloader_task;
static uint8_t s_flash_write_buffer[FLASH_PAGE_PROGRAMMING_SIZE];
static uint32_t s_write_address;


static void fsp_error_check(fsp_err_t err)
{
    if(err != FSP_SUCCESS)
    {
        while(1){};
    }
}


static void nvm_erase(void)
{
    fsp_err_t err;

    uart_ble_printf("Erase Starting\r\n");

    err = qspi_op_flash_erase_sectors((uint8_t *)SECONDARY_IMAGE_START_ADDRESS, SECONDARY_IMAGE_NUM_SECTORS);
    fsp_error_check(err);

    uart_ble_printf("Erase Finished.\r\n");
}

static void nvm_write_page(void)
{

    fsp_err_t write_err;

    uart_ble_get_data(s_flash_write_buffer, FLASH_PAGE_PROGRAMMING_SIZE);

#if 0

    write_err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)s_flash_write_buffer, (uint32_t)s_write_address, FLASH_PAGE_PROGRAMMING_SIZE);
#else

    write_err = qspi_op_flash_write_data((uint8_t *)s_write_address, (uint8_t *)s_flash_write_buffer, FLASH_PAGE_PROGRAMMING_SIZE);
#endif
    s_write_address += FLASH_PAGE_PROGRAMMING_SIZE;

    uart_ble_data_consumed();

    fsp_error_check(write_err);
    uart_ble_printf("Write err: %d\r\n", write_err);

}

static void nvm_write_last_page(void)
{
    fsp_err_t write_err;
    size_t count = uart_ble_get_data_available();
    uart_ble_get_data(s_flash_write_buffer, count);

#if 0
    fsp_err_t write_err;
    write_err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)s_flash_write_buffer, (uint32_t)s_write_address, count);
#else
    write_err = qspi_op_flash_write_data((uint8_t *)s_write_address, (uint8_t *)s_flash_write_buffer, (uint32_t)count);
#endif

    fsp_error_check(write_err);
}

static void nvm_init(void)
{

#if 0
    fsp_err_t err =   R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);

    if(err != FSP_SUCCESS)
    {
        while(1){};
    }
#else
    qspi_op_init();
#endif
}


/* Downloader Thread entry function */
/* pvParameters contains TaskHandle_t */
void dsps_downloader_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    uart_ble_init();
    nvm_init();
    uart_ble_printf("UART DSPS MCU Boot Downloader Start\r\n");

    uint32_t notif;
    s_write_address = SECONDARY_IMAGE_START_ADDRESS;


    s_dsps_downloader_task = xTaskGetCurrentTaskHandle();

    while (1)
    {
        xTaskNotifyWait(0, 0xFFFFFFFF, &notif, portMAX_DELAY);

        if(notif & NOTIF_START_ERASE)
        {
            nvm_erase();
            uart_ble_flash_ready_for_data();
        }

        if(notif & NOTIF_FLASH_WRITE_READY)
        {

            nvm_write_page();

        }
        if(notif & NOTIF_FLASH_WRITE_LAST)
        {

            nvm_write_last_page();
            R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

            NVIC_SystemReset();
        }

    }
}


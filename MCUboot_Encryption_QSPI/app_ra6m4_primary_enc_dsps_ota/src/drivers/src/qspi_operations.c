/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include <dsps_downloader_thread.h>
#include "qspi_ep.h"
#include "r_qspi.h"
#include "qspi_operations.h"




static SemaphoreHandle_t    write_status_semaphore = NULL;
static bool                 flash_op_in_progress = false;
static spi_flash_status_t   flash_write_status;


static void wait_for_flash_op_complete(void)
{
    flash_op_in_progress = true;
    xSemaphoreTake(write_status_semaphore, portMAX_DELAY);
}


/*******************************************************************************************************************//**
 *  @brief       Close QSPI module
 *  @param[IN]   spi_protocol mode
 *  @retval      None
 **********************************************************************************************************************/
void qspi_op_deinit(const spi_flash_protocol_t spi_protocol_mode)
{
    fsp_err_t error = FSP_SUCCESS;

    /* if QPI is active mode then Exit QPI mode from flash device before QSPI close */
    if (SPI_FLASH_PROTOCOL_QPI == spi_protocol_mode)
    {
        uint8_t data_exit_qpi = QSPI_MX25L_CMD_EXIT_QPI_MODE;

        error = R_QSPI_DirectWrite(&g_qspi0_ctrl, &data_exit_qpi, ONE_BYTE, false);
        if (FSP_SUCCESS != error)
        {
            while(1);
        }
    }

       /* close QSPI module */
    error = R_QSPI_Close(&g_qspi0_ctrl);
    if (FSP_SUCCESS != error)
    {
        while(1);
    }


}
/*******************************************************************************************************************//**
 *  @brief       wait for QSPI flash device status register to get idle till operation is in progress
 *  @param[IN]   None
 *  @retval      FSP_SUCCESS or any other possible error codes
 **********************************************************************************************************************/
fsp_err_t qspi_op_get_flash_status(void)
{
    spi_flash_status_t status = {.write_in_progress = true};
    int32_t time_out          = (INT32_MAX);
    fsp_err_t err             = FSP_SUCCESS;

    do
    {
        /* Get status from QSPI flash device */
        err = R_QSPI_StatusGet(&g_qspi0_ctrl, &status);
        if (FSP_SUCCESS!= err)
        {
            return err;
        }

        /* Decrement time out to avoid infinite loop in case of consistent failure */
        --time_out;

        if ( 0 >= time_out)
        {
            return FSP_ERR_TIMEOUT;
        }

    }while (false != status.write_in_progress);

    return err;
}

SemaphoreHandle_t  qspi_op_init(void)
{
       fsp_err_t err                                  = FSP_SUCCESS;

       uint8_t   data_sreg[SREG_SIZE]                 = STATUS_REG_PAYLOAD;

       write_status_semaphore = xSemaphoreCreateBinary();

       if(write_status_semaphore == NULL)
       {
           while(1);
       }else
       {
          // xSemaphoreGive(write_status_semaphore);
           //xSemaphoreTake(write_status_semaphore, portMAX_DELAY);
       }


       /* open QSPI in extended SPI mode */
       err = R_QSPI_Open(&g_qspi0_ctrl, &g_qspi0_cfg);
       if (FSP_SUCCESS != err)
       {
           while(1);
       }

       /* write enable for further operations */
       err = R_QSPI_DirectWrite(&g_qspi0_ctrl, &(g_qspi0_cfg.write_enable_command), ONE_BYTE, false);
       if (FSP_SUCCESS != err)
       {

           /* close QSPI module which is currently in extended SPI mode only */
           qspi_op_deinit(SPI_FLASH_PROTOCOL_EXTENDED_SPI);

       }
       else
       {
           err = qspi_op_get_flash_status();
           if (FSP_SUCCESS != err)
           {

               /* close QSPI module which is currently in extended SPI mode only */
               qspi_op_deinit(SPI_FLASH_PROTOCOL_EXTENDED_SPI);

           }
       }

       /*
        * write QSPI flash status register
        * This is required to make sure the device is ready for general
        * read write operation,
        * This performs settings such as physical reset,WP hardware pin disable,
        * block protection lock bits clearing.
        * for more details please refer Mx25L data sheet.
        */
       err = R_QSPI_DirectWrite(&g_qspi0_ctrl, data_sreg, SREG_SIZE, false);
       if (FSP_SUCCESS != err)
       {

           /* close QSPI module which is currently in extended SPI mode only */
           qspi_op_deinit(SPI_FLASH_PROTOCOL_EXTENDED_SPI);

       }
       else
       {
           err = qspi_op_get_flash_status();
           if (FSP_SUCCESS != err)
           {

               /* close QSPI module which is currently in extended SPI mode only */
               qspi_op_deinit(SPI_FLASH_PROTOCOL_EXTENDED_SPI);

           }
       }

       return write_status_semaphore;

}

/**
 * @brief Perform a full chip erase on the QSPI flash.
 *
 * Issues the erase command to the QSPI flash device, clearing
 * the entire memory array by setting all bits to 1 (0xFF).
 * This operation is typically time-consuming compared to
 * sector or block erase commands.
 *
 * @retval FSP_SUCCESS          Chip erase command was successfully issued.
 * @retval FSP_ERR_UNSUPPORTED  The flash device does not support full chip erase.
 * @retval FSP_ERR_WRITE_FAILED Flash reported an error while erasing.
 * @retval FSP_ERR_ASSERTION    Invalid state or configuration detected.
 *
 * @note A full chip erase may take several seconds depending on
 *       the flash device size and specifications. During this time,
 *       the device will be busy and unavailable for other operations.
 *
 * @warning This operation erases the *entire* contents of the flash.
 *          Ensure this is the intended action, as all stored data
 *          will be permanently lost.
 *
 * @par Usage Example:
 * @code
 * // Erase the entire QSPI flash
 * fsp_err_t err = qspi_op_chip_erase();
 * if (FSP_SUCCESS != err)
 * {
 *     // Handle error (unsupported device, failure, etc.)
 * }
 * @endcode
 */

fsp_err_t qspi_op_chip_erase(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_QSPI_Erase(&g_qspi0_ctrl, (uint8_t *)0, SPI_FLASH_ERASE_SIZE_CHIP_ERASE);

    wait_for_flash_op_complete();

    return err;
}

fsp_err_t qspi_op_flash_erase_sectors(uint8_t *start_address, uint16_t num_sectors)
{

    fsp_err_t err = FSP_SUCCESS;

    for(uint32_t k=0; k<num_sectors;k++)
    {
         err = R_QSPI_Erase(&g_qspi0_ctrl, start_address+k*SECTOR_SIZE, SECTOR_SIZE);
         if (FSP_SUCCESS != err)
         {
             qspi_op_deinit(g_qspi0_cfg.spi_protocol);
         }
         else
         {
            err = qspi_op_get_flash_status();
            if (FSP_SUCCESS != err)
            {
                qspi_op_deinit(g_qspi0_cfg.spi_protocol);
            }
          }

         wait_for_flash_op_complete();

     }

    return err;

}

/**
 * @brief Write data to QSPI flash memory.
 *
 * Programs a block of data into the specified QSPI flash address.
 * The function handles writing up to the requested number of bytes,
 * typically aligned to the device's page boundaries.
 *
 * @param[in]  addr  Pointer to the destination address in QSPI flash
 *                   where data will be written.
 * @param[in]  data  Pointer to the source buffer containing data to write.
 * @param[in]  bytes Number of bytes to write from @p data into QSPI flash.
 *
 * @retval FSP_SUCCESS               Data was written successfully.
 * @retval FSP_ERR_WRITE_FAILED      Flash reported a write error.
 * @retval FSP_ERR_ASSERTION         One or more input parameters are invalid.
 * @retval FSP_ERR_UNSUPPORTED       Operation not supported by this device.
 *
 * @note Ensure the target flash sector has been erased before calling this
 *       function. Most QSPI flash devices do not allow overwriting bits
 *       from 0 to 1 without an erase.
 *
 * @par Usage Example:
 * @code
 * uint8_t buffer[256];
 * // Fill buffer with application data
 * memset(buffer, 0xA5, sizeof(buffer));
 *
 * // Erase flash sector first (not shown here)
 * flash_erase((uint8_t *)0x60000000, SECTOR_SIZE);
 *
 * // Write 256 bytes to flash at address 0x60000000
 * fsp_err_t err = qspi_op_flash_write_data((uint8_t *)0x60000000, buffer, sizeof(buffer));
 * if (FSP_SUCCESS != err)
 * {
 *     // Handle error
 * }
 * @endcode
 */
fsp_err_t qspi_op_flash_write_data(uint8_t *addr, uint8_t *data, uint32_t bytes)
{
    fsp_err_t err =     FSP_SUCCESS;

    err = R_QSPI_Write(&g_qspi0_ctrl, data, addr, bytes);

    wait_for_flash_op_complete();

    return err;
}


/*
 * FreeRTOS Idle Hook.  This is implemented here to allow for QSPI operations to suspend while checking the status bit.
 * The Idle task will yield and give the semaphore back to the application task to indicate the previous operation is complete.
 */
void vApplicationIdleHook(void)
{
    if(flash_op_in_progress)
    {
        R_QSPI_StatusGet(&g_qspi0_ctrl, &flash_write_status);

        if(!flash_write_status.write_in_progress)
        {
            flash_op_in_progress = false;
            xSemaphoreGive(write_status_semaphore);
        }
    }
}





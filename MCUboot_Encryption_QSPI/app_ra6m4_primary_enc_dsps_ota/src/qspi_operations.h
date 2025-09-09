/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef QSPI_OPERATIONS_H_
#define QSPI_OPERATIONS_H_

SemaphoreHandle_t  qspi_op_init(void);

void qspi_op_deinit(const spi_flash_protocol_t spi_protocol_mode);

fsp_err_t qspi_op_get_flash_status(void);

fsp_err_t qspi_op_write_flash(void);

fsp_err_t qspi_op_flash_erase_sectors(uint8_t * start_address, uint16_t num_sectors);

fsp_err_t qspi_op_chip_erase(void);

fsp_err_t qspi_op_flash_write_data(uint8_t *addr, uint8_t *data, uint32_t bytes);

#endif /* QSPI_OPERATIONS_H_ */

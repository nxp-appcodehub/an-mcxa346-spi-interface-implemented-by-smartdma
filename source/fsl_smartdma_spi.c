/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <fsl_smartdma_spi.h>
#include "fsl_smartdma.h"
#include <string.h>

/*******************************************************************************
 * Code
 ******************************************************************************/
/**
 * @brief SPI write operation
 * @param regAddress Register address
 * @param data Data buffer
 * @param dataSize Data size in bytes
 * @return Operation status
 */
spi_status_t SPI_Write(uint32_t regAddress, const uint8_t *data, uint32_t dataSize)
{
    if (data == NULL || dataSize == 0 || dataSize > SPI_MAX_DATA_SIZE) {
        return kSPI_STATUS_ERROR;
    }

    while (g_spi_complete_flag == 0);
    g_spi_complete_flag = 0;

    memcpy((void*)g_write_buffer, data, dataSize);
    g_register_address = regAddress;
    g_datalength = dataSize;

    SmartDMA_software_trigger_bit(kSMARTDMA_WRITE_WITH_ADDRESS);
    return kSPI_STATUS_SUCCESS;
}

/**
 * @brief SPI read operation
 * @param regAddress Register address
 * @param data Data buffer
 * @param dataSize Data size in bytes
 * @return Operation status
 */
spi_status_t SPI_Read(uint32_t regAddress, uint8_t *data, uint32_t dataSize)
{
    if (data == NULL || dataSize == 0 || dataSize > SPI_MAX_DATA_SIZE) {
        return kSPI_STATUS_ERROR;
    }

    while (g_spi_complete_flag == 0);
    g_spi_complete_flag = 0;

    g_register_address = regAddress;
    g_datalength = dataSize;

    SmartDMA_software_trigger_bit(kSMARTDMA_READ_WITH_ADDRESS);
    while (g_spi_complete_flag == 0);

    memcpy(data, (const void*)g_read_buffer, dataSize);
    return kSPI_STATUS_SUCCESS;
}




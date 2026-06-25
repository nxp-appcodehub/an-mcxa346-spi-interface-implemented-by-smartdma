/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_QSPI_H_
#define _FSL_QSPI_H_

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Maximum data size for SPI operations */
#define SPI_MAX_DATA_SIZE    (32U)

/**
 * @brief SPI Status Types
 */
typedef enum {
    kSPI_STATUS_SUCCESS = 0,    /**< Operation successful */
    kSPI_STATUS_ERROR   = 1,    /**< Operation error */
    kSPI_STATUS_BUSY    = 2,    /**< SPI is busy */
    kSPI_STATUS_TIMEOUT = 3     /**< Operation timeout */
} spi_status_t;

/**
 * @brief SmartDMA SPI Operation Types
 */
typedef enum {
    kSMARTDMA_WRITE_WITH_ADDRESS = 0,    /**< Write operation with address */
    kSMARTDMA_READ_WITH_ADDRESS  = 1,    /**< Read operation with address */
    kSMARTDMA_WRITE_WITHOUT_ADDRESS = 2, /**< Write operation without address */
    kSMARTDMA_READ_WITHOUT_ADDRESS  = 3, /**< Read operation without address */
} smartdma_operation_t;


/*******************************************************************************
 * Variables
 ******************************************************************************/
extern volatile uint32_t g_samrtdma_stack[16];
extern volatile uint32_t g_register_address;
extern volatile uint8_t  g_write_buffer[SPI_MAX_DATA_SIZE];
extern volatile uint8_t  g_read_buffer[SPI_MAX_DATA_SIZE];
extern volatile uint32_t g_datalength;
extern volatile uint32_t g_fre_divider;
extern volatile uint32_t g_spi_complete_flag;

/*******************************************************************************
 * API
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SPI write operation with register address
 * @param regAddress Register address to write to
 * @param data Pointer to data buffer to be written
 * @param dataSize Size of data to write (in bytes)
 * @return SPI operation status
 */
spi_status_t SPI_Write(uint32_t regAddress, const uint8_t *data, uint32_t dataSize);

/**
 * @brief SPI read operation with register address
 * @param regAddress Register address to read from
 * @param data Pointer to buffer where read data will be stored
 * @param dataSize Size of data to read (in bytes)
 * @return SPI operation status
 */
spi_status_t SPI_Read(uint32_t regAddress, uint8_t *data, uint32_t dataSize);

#ifdef __cplusplus
}
#endif

#endif /* _FSL_QSPI_H_ */

/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"
#include "fsl_inputmux.h"
#include "fsl_clock.h"
#include "fsl_smartdma.h"
#include "fsl_smartdma_mcxa.h"
#include "fsl_smartdma_spi.h"
#include "app.h"
#include "ICM42688.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DELAY_COUNT_PER_MS      22500U
#define SMARTDMA_STACK_SIZE     16
#define SPI_BUFFER_SIZE         16
// ICM42688 default scale factors
#define ICM42688_GYRO_SCALE_FACTOR    131.0f    // LSB/dps for ±2000dps range
#define ICM42688_ACCEL_SCALE_FACTOR   8192.0f   // LSB/g for ±4g range
/*******************************************************************************
 * Variables
 ******************************************************************************/
smartdma_spi_param_t smartdmaParam;
volatile uint32_t g_samrtdma_stack[SMARTDMA_STACK_SIZE];
volatile uint32_t g_register_address;
volatile uint8_t g_write_buffer[SPI_MAX_DATA_SIZE];
volatile uint8_t g_read_buffer[SPI_MAX_DATA_SIZE];
volatile uint32_t g_datalength;
volatile uint32_t g_fre_divider = 10;
volatile uint32_t g_spi_complete_flag = 0;
volatile bool g_ButtonPress = false;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void SmartDMA_spi_callback(void *param);
static void delay_ms_simple(uint32_t ms);
static void smartdma_init(void);
static void read_and_print_sensor_data(uint32_t *loop_count);

/*******************************************************************************
 * Code
 ******************************************************************************/
static void SmartDMA_spi_callback(void *param)
{
  (void)param;
  g_spi_complete_flag = 1;
}

static void delay_ms_simple(uint32_t ms)
{
  volatile uint32_t count = DELAY_COUNT_PER_MS * ms;
  while (count--) {
      __NOP();
  }
}


/**
 * @brief Initialize SmartDMA for SPI operations
 * Configures SmartDMA parameters, installs callback, boots API and enables interrupts
 */
static void smartdma_init(void)
{


    // Initialize SmartDMA without firmware
    SMARTDMA_InitWithoutFirmware();

    // Install firmware from array
    SMARTDMA_InstallFirmware(SMARTDMA_SPI_MEM_ADDR, s_smartdmaSPIFirmware,
                            SMARTDMA_SPI_FIRMWARE_SIZE);

    // Install callback and configure interrupt
    SMARTDMA_InstallCallback(SmartDMA_spi_callback, NULL);
    NVIC_EnableIRQ(SMARTDMA_IRQn);
    NVIC_SetPriority(SMARTDMA_IRQn, 3);

    // Configure SmartDMA parameters
    smartdmaParam.smartdma_stack = (uint32_t*)g_samrtdma_stack;
    smartdmaParam.p_reg_addr     = (uint32_t*)&g_register_address;
    smartdmaParam.p_write_buf    = (uint32_t*)g_write_buffer;
    smartdmaParam.p_read_buf     = (uint32_t*)g_read_buffer;
    smartdmaParam.p_data_length  = (uint32_t*)&g_datalength;
    smartdmaParam.p_frq_div      = (uint32_t*)&g_fre_divider;

    // Boot SmartDMA with firmware
    g_spi_complete_flag = 1;
    SMARTDMA_Boot(kSMARTDMA_spi, &smartdmaParam, 0x2);

}



static void read_and_print_sensor_data(uint32_t *loop_count)
{
  icm42688_accel_data_t accel_raw;
  icm42688_gyro_data_t gyro_raw;

  // Read raw sensor data
  if (ICM42688_ReadAccelData(&accel_raw) == kSPI_STATUS_SUCCESS &&
      ICM42688_ReadGyroData(&gyro_raw) == kSPI_STATUS_SUCCESS) {

      // Convert raw LSB values to physical units
      float accel_x_g = (float)accel_raw.x / ICM42688_ACCEL_SCALE_FACTOR;
      float accel_y_g = (float)accel_raw.y / ICM42688_ACCEL_SCALE_FACTOR;
      float accel_z_g = (float)accel_raw.z / ICM42688_ACCEL_SCALE_FACTOR;

      float gyro_x_dps = (float)gyro_raw.x / ICM42688_GYRO_SCALE_FACTOR;
      float gyro_y_dps = (float)gyro_raw.y / ICM42688_GYRO_SCALE_FACTOR;
      float gyro_z_dps = (float)gyro_raw.z / ICM42688_GYRO_SCALE_FACTOR;

      // Print converted sensor data
      PRINTF("[%04d] Accel(g): X=%6.2f Y=%6.2f Z=%6.2f | Gyro(dps): X=%7.2f Y=%7.2f Z=%7.2f\r\n",
             ++(*loop_count),
             accel_x_g, accel_y_g, accel_z_g,
             gyro_x_dps, gyro_y_dps, gyro_z_dps);
  } else {
      PRINTF("[%04d] Error reading sensor data\r\n", ++(*loop_count));
  }
}


int main(void)
{
  uint32_t loop_count = 0;

  BOARD_InitHardware();
  PRINTF("\r\n=== SmartDMA SPI ICM42688 Example ===\r\n");


  // Initialize SmartDMA
  smartdma_init();

  // Initialize ICM42688
  PRINTF("Initializing ICM42688...\r\n");
  if (ICM42688_Init() == kSPI_STATUS_SUCCESS) {
      PRINTF("ICM42688 initialized successfully!\r\n");
      PRINTF("Starting sensor data reading (1Hz)...\r\n");
      PRINTF("Format: [Count] Accel(g): X=xx.xx Y=xx.xx Z=xx.xx | Gyro(dps): X=xx.xx Y=xx.xx Z=xx.xx\r\n");
  } else {
      PRINTF("ICM42688 initialization failed!\r\n");
  }

  // Main loop with 1-second sensor reading
  while (1) {
      read_and_print_sensor_data(&loop_count);

      // 1 second delay
      delay_ms_simple(1000);
  }
}

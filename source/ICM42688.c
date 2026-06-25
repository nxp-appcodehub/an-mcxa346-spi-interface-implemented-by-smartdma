/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "ICM42688.h"

#ifdef __ARMCC_VERSION
  #include "cmsis_armcc.h"
#elif defined(__GNUC__)
  #include "cmsis_gcc.h"
#elif defined(__ICCARM__)
  #include "cmsis_iccarm.h"
#endif

// External SPI functions
extern spi_status_t SPI_Write(uint32_t regAddress, const uint8_t *data, uint32_t dataSize);
extern spi_status_t SPI_Read(uint32_t regAddress, uint8_t *data, uint32_t dataSize);

/* Simple delay function implementation */
static void delay_ms_simple(uint32_t ms)
{
  // Simple loop delay, adjust based on CPU frequency
  volatile uint32_t count;
  for (uint32_t i = 0; i < ms; i++) {
      for (count = 0; count < 15000; count++) {
#ifdef __NOP
          __NOP();
#else
          __asm volatile ("nop");
#endif
      }
  }
}

/* Write single register */
spi_status_t ICM42688_WriteReg(uint32_t regAddress, uint8_t regData)
{
  // Clear MSB (set to 0) for write operation
  uint32_t writeAddress = regAddress & 0x7F;
  return SPI_Write(writeAddress, &regData, 1);
}

/* Read single register */
spi_status_t ICM42688_ReadReg(uint32_t regAddress, uint8_t *regData)
{
  if (regData == NULL) {
      return kSPI_STATUS_ERROR;
  }
  // Set MSB to 1 for read operation
  uint32_t readAddress = regAddress | 0x80;
  return SPI_Read(readAddress, regData, 1);
}

/* Read multiple registers */
spi_status_t ICM42688_ReadMultipleReg(uint32_t regAddress, uint8_t *regData, uint32_t dataSize)
{
  if (regData == NULL || dataSize == 0) {
      return kSPI_STATUS_ERROR;
  }
  // Set MSB to 1 for read operation
  uint32_t readAddress = regAddress | 0x80;
  return SPI_Read(readAddress, regData, dataSize);
}

/* Initialize ICM42688 sensor */
spi_status_t ICM42688_Init(void)
{
  spi_status_t status;
  uint8_t regData;
  uint8_t whoami;

  // Step 1: Perform soft reset
  status = ICM42688_WriteReg(ICM42688_DEVICE_CONFIG, ICM42688_DEVICE_CONFIG_SOFT_RESET);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  // Wait for reset to complete
  delay_ms_simple(100);

  // Step 2: Check WHO_AM_I register
  status = ICM42688_ReadReg(ICM42688_WHO_AM_I, &whoami);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  if (whoami != ICM42688P_WHOAMI) {
      return kSPI_STATUS_ERROR;  // Wrong device ID
  }

  // Step 3: Configure power management - turn on gyro and accel in low noise mode
  regData = ICM42688_PWR_MGMT0_GYRO_MODE_LN | ICM42688_PWR_MGMT0_ACCEL_MODE_LN;
  status = ICM42688_WriteReg(ICM42688_PWR_MGMT0, regData);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  // Wait for sensors to start up
  delay_ms_simple(50);

  // Step 4: Configure default gyro settings (±2000dps, 1kHz ODR)
  regData = ICM42688_GYRO_FS_2000DPS | ICM42688_ODR_1KHZ;
  status = ICM42688_WriteReg(ICM42688_GYRO_CONFIG0, regData);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  // Step 5: Configure default accel settings (±16g, 1kHz ODR)
  regData = ICM42688_ACCEL_FS_16G | ICM42688_ODR_1KHZ;
  status = ICM42688_WriteReg(ICM42688_ACCEL_CONFIG0, regData);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  // Wait for configuration to take effect
  delay_ms_simple(50);

  return kSPI_STATUS_SUCCESS;
}

/* Reset ICM42688 sensor */
spi_status_t ICM42688_Reset(void)
{
  spi_status_t status;

  // Perform soft reset
  status = ICM42688_WriteReg(ICM42688_DEVICE_CONFIG, ICM42688_DEVICE_CONFIG_SOFT_RESET);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  // Wait for reset to complete
  delay_ms_simple(100);

  return kSPI_STATUS_SUCCESS;
}

/* Configure accelerometer */
spi_status_t ICM42688_SetAccelConfig(uint8_t fullScale, uint8_t outputDataRate)
{
  spi_status_t status;
  uint8_t regData;

  regData = fullScale | outputDataRate;
  status = ICM42688_WriteReg(ICM42688_ACCEL_CONFIG0, regData);

  if (status == kSPI_STATUS_SUCCESS) {
      delay_ms_simple(10);  // Allow configuration to take effect
  }

  return status;
}

/* Configure gyroscope */
spi_status_t ICM42688_SetGyroConfig(uint8_t fullScale, uint8_t outputDataRate)
{
  spi_status_t status;
  uint8_t regData;

  regData = fullScale | outputDataRate;
  status = ICM42688_WriteReg(ICM42688_GYRO_CONFIG0, regData);

  if (status == kSPI_STATUS_SUCCESS) {
      delay_ms_simple(10);  // Allow configuration to take effect
  }

  return status;
}

/* Read accelerometer data */
spi_status_t ICM42688_ReadAccelData(icm42688_accel_data_t *accelData)
{
  spi_status_t status;
  uint8_t rawData[6];

  if (accelData == NULL) {
      return kSPI_STATUS_ERROR;
  }

  // Read 6 bytes starting from ACCEL_DATA_X1
  status = ICM42688_ReadMultipleReg(ICM42688_ACCEL_DATA_X1, rawData, 6);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  // Convert raw data to signed 16-bit values (big-endian format)
  accelData->x = (int16_t)((rawData[0] << 8) | rawData[1]);
  accelData->y = (int16_t)((rawData[2] << 8) | rawData[3]);
  accelData->z = (int16_t)((rawData[4] << 8) | rawData[5]);

  return kSPI_STATUS_SUCCESS;
}

/* Read gyroscope data */
spi_status_t ICM42688_ReadGyroData(icm42688_gyro_data_t *gyroData)
{
  spi_status_t status;
  uint8_t rawData[6];

  if (gyroData == NULL) {
      return kSPI_STATUS_ERROR;
  }

  // Read 6 bytes starting from GYRO_DATA_X1
  status = ICM42688_ReadMultipleReg(ICM42688_GYRO_DATA_X1, rawData, 6);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  // Convert raw data to signed 16-bit values (big-endian format)
  gyroData->x = (int16_t)((rawData[0] << 8) | rawData[1]);
  gyroData->y = (int16_t)((rawData[2] << 8) | rawData[3]);
  gyroData->z = (int16_t)((rawData[4] << 8) | rawData[5]);
  return kSPI_STATUS_SUCCESS;
}

/* Read temperature data */
spi_status_t ICM42688_ReadTemperature(int16_t *temperature)
{
  spi_status_t status;
  uint8_t rawData[2];

  if (temperature == NULL) {
      return kSPI_STATUS_ERROR;
  }

  // Read 2 bytes starting from TEMP_DATA1
  status = ICM42688_ReadMultipleReg(ICM42688_TEMP_DATA1, rawData, 2);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  // Convert raw data to signed 16-bit value (big-endian format)
  *temperature = (int16_t)((rawData[0] << 8) | rawData[1]);

  return kSPI_STATUS_SUCCESS;
}

/* Read all sensor data */
spi_status_t ICM42688_ReadAllSensorData(icm42688_sensor_data_t *sensorData)
{
  spi_status_t status;
  uint8_t rawData[14];  // 2 bytes temp + 6 bytes accel + 6 bytes gyro

  if (sensorData == NULL) {
      return kSPI_STATUS_ERROR;
  }

  // Read all sensor data in one burst (from TEMP_DATA1 to GYRO_DATA_Z0)
  status = ICM42688_ReadMultipleReg(ICM42688_TEMP_DATA1, rawData, 14);
  if (status != kSPI_STATUS_SUCCESS) {
      return status;
  }

  // Parse temperature data (bytes 0-1)
  sensorData->temperature = (int16_t)((rawData[0] << 8) | rawData[1]);

  // Parse accelerometer data (bytes 2-7)
  sensorData->accel.x = (int16_t)((rawData[2] << 8) | rawData[3]);
  sensorData->accel.y = (int16_t)((rawData[4] << 8) | rawData[5]);
  sensorData->accel.z = (int16_t)((rawData[6] << 8) | rawData[7]);

  // Parse gyroscope data (bytes 8-13)
  sensorData->gyro.x = (int16_t)((rawData[8] << 8) | rawData[9]);
  sensorData->gyro.y = (int16_t)((rawData[10] << 8) | rawData[11]);
  sensorData->gyro.z = (int16_t)((rawData[12] << 8) | rawData[13]);

  return kSPI_STATUS_SUCCESS;
}

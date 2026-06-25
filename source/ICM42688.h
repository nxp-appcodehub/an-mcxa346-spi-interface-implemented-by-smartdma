/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ICM42688_H
#define ICM42688_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "fsl_smartdma_spi.h"


/* ICM42688P Register Addresses - Bank 0 */
#define ICM42688_DEVICE_CONFIG      0x11U
#define ICM42688_INT_CONFIG         0x14U
#define ICM42688_TEMP_DATA1         0x1DU
#define ICM42688_TEMP_DATA0         0x1EU
#define ICM42688_ACCEL_DATA_X1      0x1FU
#define ICM42688_ACCEL_DATA_X0      0x20U
#define ICM42688_ACCEL_DATA_Y1      0x21U
#define ICM42688_ACCEL_DATA_Y0      0x22U
#define ICM42688_ACCEL_DATA_Z1      0x23U
#define ICM42688_ACCEL_DATA_Z0      0x24U
#define ICM42688_GYRO_DATA_X1       0x25U
#define ICM42688_GYRO_DATA_X0       0x26U
#define ICM42688_GYRO_DATA_Y1       0x27U
#define ICM42688_GYRO_DATA_Y0       0x28U
#define ICM42688_GYRO_DATA_Z1       0x29U
#define ICM42688_GYRO_DATA_Z0       0x2AU
#define ICM42688_INT_STATUS         0x2DU
#define ICM42688_PWR_MGMT0          0x4EU
#define ICM42688_GYRO_CONFIG0       0x4FU
#define ICM42688_ACCEL_CONFIG0      0x50U
#define ICM42688_WHO_AM_I           0x75U
#define ICM42688_REG_BANK_SEL       0x76U

/* ICM42688P Expected WHO_AM_I Value */
#define ICM42688P_WHOAMI            0x47U

/* Power Management 0 Register Values */
#define ICM42688_PWR_MGMT0_GYRO_MODE_OFF    0x00U
#define ICM42688_PWR_MGMT0_GYRO_MODE_LN     0x0CU
#define ICM42688_PWR_MGMT0_ACCEL_MODE_OFF   0x00U
#define ICM42688_PWR_MGMT0_ACCEL_MODE_LN    0x03U

/* Device Config Register Values */
#define ICM42688_DEVICE_CONFIG_SOFT_RESET   0x01U

/* Gyro Full Scale Values */
#define ICM42688_GYRO_FS_2000DPS    0x00U
#define ICM42688_GYRO_FS_1000DPS    0x20U
#define ICM42688_GYRO_FS_500DPS     0x40U
#define ICM42688_GYRO_FS_250DPS     0x60U

/* Accel Full Scale Values */
#define ICM42688_ACCEL_FS_16G       0x00U
#define ICM42688_ACCEL_FS_8G        0x20U
#define ICM42688_ACCEL_FS_4G        0x40U
#define ICM42688_ACCEL_FS_2G        0x60U

/* Output Data Rate Values */
#define ICM42688_ODR_1KHZ           0x06U
#define ICM42688_ODR_500HZ          0x0FU
#define ICM42688_ODR_200HZ          0x07U
#define ICM42688_ODR_100HZ          0x08U
#define ICM42688_ODR_50HZ           0x09U

/* Data structures */
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} icm42688_accel_data_t;

typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} icm42688_gyro_data_t;

typedef struct {
  icm42688_accel_data_t accel;
  icm42688_gyro_data_t gyro;
  int16_t temperature;
} icm42688_sensor_data_t;

/* Function prototypes */
spi_status_t ICM42688_WriteReg(uint32_t regAddress, uint8_t regData);
spi_status_t ICM42688_ReadReg(uint32_t regAddress, uint8_t *regData);
spi_status_t ICM42688_ReadMultipleReg(uint32_t regAddress, uint8_t *regData, uint32_t dataSize);
spi_status_t ICM42688_Init(void);
spi_status_t ICM42688_Reset(void);
spi_status_t ICM42688_SetAccelConfig(uint8_t fullScale, uint8_t outputDataRate);
spi_status_t ICM42688_SetGyroConfig(uint8_t fullScale, uint8_t outputDataRate);
spi_status_t ICM42688_ReadAccelData(icm42688_accel_data_t *accelData);
spi_status_t ICM42688_ReadGyroData(icm42688_gyro_data_t *gyroData);
spi_status_t ICM42688_ReadAllSensorData(icm42688_sensor_data_t *sensorData);
spi_status_t ICM42688_ReadTemperature(int16_t *temperature);

#endif /* ICM42688_H */

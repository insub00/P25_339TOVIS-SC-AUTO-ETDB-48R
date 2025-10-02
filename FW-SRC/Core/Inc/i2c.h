/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_I2C2_Init(void);

/* USER CODE BEGIN Prototypes */
extern uint8_t gDispay_i2c_check_ok;

extern void I2C2_speed_ctrl(uint32_t Speed);
extern void Slave_Addr_Check_All_I2cLine(void);
extern void D_IC_I2C_check(void);
extern void wing_board_i2c_check(void);
extern void LED_i2c_write(uint8_t dev_addr, uint8_t reg, uint8_t data);
extern uint8_t LED_i2c_read(uint8_t dev_addr, uint8_t reg);
extern void LCM_EEP_i2c_write(uint8_t dev_addr, uint8_t reg, uint8_t *datas, uint16_t len);
extern uint8_t LCM_EEP_i2c_read(uint8_t dev_addr, uint8_t reg);
extern void LCM_EEP_i2c_sequential_read(uint8_t dev_addr, uint8_t *wBuf, uint16_t wLen, uint8_t *rBuf, uint16_t rLen);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */


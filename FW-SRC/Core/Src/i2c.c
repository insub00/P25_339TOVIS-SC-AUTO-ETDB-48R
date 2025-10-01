/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c2;

/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();

    /* I2C2 interrupt Init */
    HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);

    /* I2C2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void I2C2_speed_ctrl(uint32_t Speed)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = Speed*1000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}


void Slave_Addr_Check_All_I2cLine(void)
{
  uint8_t address;
  uint8_t check = 0;
  
  printf("I2C Line Slave address check start\r\n");
  
//  TCA9546APWR_Channel_Sel(eI2C_TOUCH);
//  for (address = 0x01; address <= (0xFF/2); address++) 
//  {
//    if (HAL_I2C_IsDeviceReady(&hi2c2, address << 1, 1, 100) == HAL_OK) 
//    {
//      if ((TCA9545_DEV_ID >> 1) != address)
//        printf("TOUCH i2c2 detect addr = 0x%X \r\n", address<<1);
//
//    }
//  }
//  HAL_Delay(5);
//  
//  TCA9546APWR_Channel_Sel(eI2C_TCON);
//  for (address = 0x01; address <= (0xFF/2); address++) 
//  {
//    if (HAL_I2C_IsDeviceReady(&hi2c2, address << 1, 1, 100) == HAL_OK) 
//    {
//      if ((TCA9545_DEV_ID >> 1) != address)
//        printf("TCON i2c2 detect addr = 0x%X \r\n", address<<1);
//    }
//  }
//  HAL_Delay(5);
  
  TCA9546APWR_Channel_Sel(eI2C_DISPLAY);
  for (address = 0x01; address <= (0xFF/2); address++) 
  {
    if (HAL_I2C_IsDeviceReady(&hi2c2, address << 1, 1, 100) == HAL_OK) 
    {
      if ((TCA9545_DEV_ID >> 1) != address)
        printf("Display detect addr = 0x%X \r\n", address<<1);
    }
  }
  HAL_Delay(5);
  
//  TCA9546APWR_Channel_Sel(eI2C_LED);
//  for (address = 0x01; address <= (0xFF/2); address++) 
//  {
//    if (HAL_I2C_IsDeviceReady(&hi2c2, address << 1, 1, 100) == HAL_OK) 
//    {
//      if ((TCA9545_DEV_ID >> 1) != address)
//        printf("LED i2c2 detect addr = 0x%X \r\n", address<<1);
//    }
//  }
//  HAL_Delay(5);
}
uint8_t gDispay_i2c_check_ok = 0;
void D_IC_I2C_check(void)
{
  gDispay_i2c_check_ok = 0;
  TCA9546APWR_Channel_Sel(eI2C_DISPLAY);
  if (HAL_I2C_IsDeviceReady(&hi2c2, HX8295_DEV_ID, 1, 100) == HAL_OK) 
  {
      printf("Display detect addr = 0x%X \r\n", HX8295_DEV_ID);
      gDispay_i2c_check_ok = 1;
  }
}

void wing_board_i2c_check(void)
{
  uint8_t address;
  uint8_t check = 0;
  
  printf("wing board I2C Line Slave address check start\r\n");
  
  TCA9546APWR_Channel_Sel(eI2C_TOUCH);
  if (HAL_I2C_IsDeviceReady(&hi2c2, 0x4A<<1, 1, 100) == HAL_OK) 
  {
      printf("TOUCH i2c2 detect addr = 0x%X \r\n", 0x4A<<1);
  }
  else
  {
    g_wing_test_err |= 0x01;
  }
  
  TCA9546APWR_Channel_Sel(eI2C_DISPLAY);
  if (HAL_I2C_IsDeviceReady(&hi2c2, 0x52<<1, 1, 100) == HAL_OK) 
  {
      printf("Display detect addr = 0x%X \r\n", 0x52<<1);
  }
  else
  {
    g_wing_test_err |= 0x02;
  }
  printf("g_wing_test_err = 0x%X \r\n", g_wing_test_err);
}

void LCM_EEP_i2c_write(uint8_t dev_addr, uint8_t reg, uint8_t *datas, uint16_t len)
{
  HAL_StatusTypeDef Status;
  uint8_t wBuf[257] = {0};
  
  memcpy(&wBuf[1], datas, len);
  wBuf[0] = reg;
  
  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, &wBuf[0], len+1, 500);
  if(Status != HAL_OK)
  {
    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
//  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, datas, len, 500);
//  if(Status != HAL_OK)
//  {
//    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
//  }
}

uint8_t LCM_EEP_i2c_read(uint8_t dev_addr, uint8_t reg)
{
  HAL_StatusTypeDef Status;
  uint8_t read_data;

  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, &reg, 1, 10);
  if(Status != HAL_OK)
  {
    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  Status = HAL_I2C_Master_Receive(&hi2c2, dev_addr | 0x01, &read_data, 1, 10);
  if(Status != HAL_OK)
  {
    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return read_data;
}

void LCM_EEP_i2c_sequential_read(uint8_t dev_addr, uint8_t *wBuf, uint16_t wLen, uint8_t *rBuf, uint16_t rLen)
{
  HAL_StatusTypeDef Status;

  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, wBuf, wLen, 500);
  if(Status != HAL_OK)
  {
    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  Status = HAL_I2C_Master_Receive(&hi2c2, dev_addr | 0x01, rBuf, rLen, 500);
  if(Status != HAL_OK)
  {
    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
  }
}

void LED_i2c_write(uint8_t dev_addr, uint8_t reg, uint8_t data)
{
  HAL_StatusTypeDef Status;
  uint8_t buf[2] = {0};
  
  TCA9546APWR_Channel_Sel(eI2C_LED);
    
  buf[0] = reg;
  buf[1] = data;
  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, buf, 2, 100);
  if(Status != HAL_OK)
  {
    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
  }
}

uint8_t LED_i2c_read(uint8_t dev_addr, uint8_t reg)
{
  HAL_StatusTypeDef Status;
  uint8_t read_data;
  
  TCA9546APWR_Channel_Sel(eI2C_LED);

  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, &reg, 1, 100);
  if(Status != HAL_OK)
  {
    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  Status = HAL_I2C_Master_Receive(&hi2c2, dev_addr | 0x01, &read_data, 1, 100);
  if(Status != HAL_OK)
  {
    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return read_data;
}
/* USER CODE END 1 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, TP_VDD_EN_Pin|ENVSP_Pin|ENVSN_Pin|ENOTP_Pin
                          |ADD_SET0_Pin|I2C_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, E_CS_Pin|E_SCL_Pin|E_MOSI_Pin|ADD_SET2_Pin
                          |ADD_SET1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED3_Pin|LED2_Pin|LED1_Pin|T_RST_Pin
                          |E_WP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, M95M04_HOLD_Pin|M95M04_WP_Pin|PON_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_FAIL_Pin LED_FAIL_Pin TCON_RST_Pin PWM_VBR_Pin */
  GPIO_InitStruct.Pin = LCD_FAIL_Pin|LED_FAIL_Pin|TCON_RST_Pin|PWM_VBR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : TP_FAIL_Pin TCON_ASIL_Pin E_MISO_Pin */
  GPIO_InitStruct.Pin = TP_FAIL_Pin|TCON_ASIL_Pin|E_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : TP_VDD_EN_Pin ENVSP_Pin ENVSN_Pin ENOTP_Pin
                           ADD_SET0_Pin I2C_RESET_Pin */
  GPIO_InitStruct.Pin = TP_VDD_EN_Pin|ENVSP_Pin|ENVSN_Pin|ENOTP_Pin
                          |ADD_SET0_Pin|I2C_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : E_CS_Pin E_SCL_Pin E_MOSI_Pin ADD_SET2_Pin
                           ADD_SET1_Pin */
  GPIO_InitStruct.Pin = E_CS_Pin|E_SCL_Pin|E_MOSI_Pin|ADD_SET2_Pin
                          |ADD_SET1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED3_Pin LED2_Pin LED1_Pin T_RST_Pin
                           E_WP_Pin */
  GPIO_InitStruct.Pin = LED3_Pin|LED2_Pin|LED1_Pin|T_RST_Pin
                          |E_WP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : TCH_EXT_Pin TEST_KEY_Pin LR_Pin SIGNAL_ON_Pin
                           BL_EN_Pin VLCD_ON_Pin */
  GPIO_InitStruct.Pin = TCH_EXT_Pin|TEST_KEY_Pin|LR_Pin|SIGNAL_ON_Pin
                          |BL_EN_Pin|VLCD_ON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : TCH_INT_Pin */
  GPIO_InitStruct.Pin = TCH_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TCH_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : M95M04_HOLD_Pin M95M04_WP_Pin PON_Pin */
  GPIO_InitStruct.Pin = M95M04_HOLD_Pin|M95M04_WP_Pin|PON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

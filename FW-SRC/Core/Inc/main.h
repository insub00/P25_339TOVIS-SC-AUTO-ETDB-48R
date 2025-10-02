/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <intrinsics.h>
#include <stdint.h>
  
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
  
#include "user.h"  
#include "packet.h"
#include "uart.h"
#include "mytest.h"
#include "vcom.h"   
  
/* ZLib */
#include "M95Mxx.h"
#include "93xx76x.h"
#include "HX8295.h"
#include "HX8882.h"
#include "TOUCH_ATMXT288M1T.h" 
#include "TCA9546APWR.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
typedef struct
{
  __IO uint32_t pwr_commrx;
  __IO uint32_t comm;
  __IO uint32_t drawing;
  __IO uint32_t mease_func;
  __IO uint32_t mease_func2;
//  __IO uint32_t t_time;
  
  
  __IO uint32_t mSocket;
  __IO uint32_t mCableCheck;
  __IO uint32_t mSocketRxTime;
  
  __IO uint32_t wingRxTime;
  
  __IO uint32_t wing_rx;
  
  __IO uint32_t input;
  __IO uint32_t output;
  __IO uint32_t gsptimer;
  __IO uint32_t timeout;
  __IO uint32_t touch_run_timer;
}UTickDef;

extern UTickDef Utick;
extern uint8_t uart_rxdata;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern void User_IncTick(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define LCD_FAIL_Pin GPIO_PIN_5
#define LCD_FAIL_GPIO_Port GPIOC
#define TP_FAIL_Pin GPIO_PIN_0
#define TP_FAIL_GPIO_Port GPIOB
#define TCON_ASIL_Pin GPIO_PIN_1
#define TCON_ASIL_GPIO_Port GPIOB
#define TP_VDD_EN_Pin GPIO_PIN_9
#define TP_VDD_EN_GPIO_Port GPIOE
#define ENVSP_Pin GPIO_PIN_10
#define ENVSP_GPIO_Port GPIOE
#define ENVSN_Pin GPIO_PIN_11
#define ENVSN_GPIO_Port GPIOE
#define ENOTP_Pin GPIO_PIN_12
#define ENOTP_GPIO_Port GPIOE
#define E_CS_Pin GPIO_PIN_12
#define E_CS_GPIO_Port GPIOB
#define E_SCL_Pin GPIO_PIN_13
#define E_SCL_GPIO_Port GPIOB
#define E_MOSI_Pin GPIO_PIN_14
#define E_MOSI_GPIO_Port GPIOB
#define E_MISO_Pin GPIO_PIN_15
#define E_MISO_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_8
#define LED3_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_9
#define LED2_GPIO_Port GPIOD
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOD
#define TCH_EXT_Pin GPIO_PIN_12
#define TCH_EXT_GPIO_Port GPIOD
#define TEST_KEY_Pin GPIO_PIN_13
#define TEST_KEY_GPIO_Port GPIOD
#define TCH_INT_Pin GPIO_PIN_14
#define TCH_INT_GPIO_Port GPIOD
#define TCH_INT_EXTI_IRQn EXTI15_10_IRQn
#define T_RST_Pin GPIO_PIN_15
#define T_RST_GPIO_Port GPIOD
#define LED_FAIL_Pin GPIO_PIN_7
#define LED_FAIL_GPIO_Port GPIOC
#define M95M04_HOLD_Pin GPIO_PIN_8
#define M95M04_HOLD_GPIO_Port GPIOC
#define M95M04_WP_Pin GPIO_PIN_9
#define M95M04_WP_GPIO_Port GPIOC
#define PON_Pin GPIO_PIN_10
#define PON_GPIO_Port GPIOC
#define TCON_RST_Pin GPIO_PIN_11
#define TCON_RST_GPIO_Port GPIOC
#define PWM_VBR_Pin GPIO_PIN_12
#define PWM_VBR_GPIO_Port GPIOC
#define LR_Pin GPIO_PIN_0
#define LR_GPIO_Port GPIOD
#define SIGNAL_ON_Pin GPIO_PIN_1
#define SIGNAL_ON_GPIO_Port GPIOD
#define E_WP_Pin GPIO_PIN_3
#define E_WP_GPIO_Port GPIOD
#define BL_EN_Pin GPIO_PIN_4
#define BL_EN_GPIO_Port GPIOD
#define VLCD_ON_Pin GPIO_PIN_7
#define VLCD_ON_GPIO_Port GPIOD
#define ADD_SET2_Pin GPIO_PIN_8
#define ADD_SET2_GPIO_Port GPIOB
#define ADD_SET1_Pin GPIO_PIN_9
#define ADD_SET1_GPIO_Port GPIOB
#define ADD_SET0_Pin GPIO_PIN_0
#define ADD_SET0_GPIO_Port GPIOE
#define I2C_RESET_Pin GPIO_PIN_1
#define I2C_RESET_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#define ANSI_RED		  "\x1b[31m"
#define ANSI_GREEN		"\x1b[32m"
#define ANSI_YELLOW		"\x1b[33m"
#define ANSI_BLUE		  "\x1b[34m"
#define ANSI_MAGENTA  "\x1b[35m"
#define ANSI_CYAN		  "\x1b[36m"
#define ANSI_RESET		"\x1b[0m"

#define PUTCHAR_PROTOTYPE int putchar(int ch)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

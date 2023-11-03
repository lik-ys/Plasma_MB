/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include  <stdint.h>
#include "arm_math.h"
#include "Debug.h"
#include "StateMashine.h"  
  
extern ADC_HandleTypeDef hadc1;
extern CAN_HandleTypeDef hcan1;
//extern ETH_HandleTypeDef heth;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern UART_HandleTypeDef* pUartWIFI  ;
extern UART_HandleTypeDef* pMBSlave   ;
extern UART_HandleTypeDef* pMBMaster  ;
extern UART_HandleTypeDef* pUartDBG   ;

extern TIM_HandleTypeDef* pMBTimMaster;
extern TIM_HandleTypeDef* pMBTimSlave;
extern TIM_HandleTypeDef* pExtSync   ; // Sync-external-out 12kHz 50%

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define COMM_START_Pin GPIO_PIN_2
#define COMM_START_GPIO_Port GPIOE
#define COMM_START_EXTI_IRQn EXTI2_IRQn
#define DI_METAL_CONTACT_Pin GPIO_PIN_3
#define DI_METAL_CONTACT_GPIO_Port GPIOE
#define DI_METAL_CONTACT_EXTI_IRQn EXTI3_IRQn
#define COMM_FIRE_Pin GPIO_PIN_4
#define COMM_FIRE_GPIO_Port GPIOE
#define COMM_FIRE_EXTI_IRQn EXTI4_IRQn
#define CNC_DI_3_Pin GPIO_PIN_6
#define CNC_DI_3_GPIO_Port GPIOE
#define CNC_DI_2_Pin GPIO_PIN_13
#define CNC_DI_2_GPIO_Port GPIOC
#define CNC_DI_1_Pin GPIO_PIN_14
#define CNC_DI_1_GPIO_Port GPIOC
#define CNC_DI_0_Pin GPIO_PIN_15
#define CNC_DI_0_GPIO_Port GPIOC
#define LED_WORK1_Pin GPIO_PIN_7
#define LED_WORK1_GPIO_Port GPIOE
#define LED_WORK2_Pin GPIO_PIN_8
#define LED_WORK2_GPIO_Port GPIOE
#define SYNC_POWER_Pin GPIO_PIN_9
#define SYNC_POWER_GPIO_Port GPIOE
#define CMD_FIRE_FIRE_Pin GPIO_PIN_10
#define CMD_FIRE_FIRE_GPIO_Port GPIOE
#define CMD_FIRE_LOCK_Pin GPIO_PIN_11
#define CMD_FIRE_LOCK_GPIO_Port GPIOE
#define CMD_FIRE_PWR_Pin GPIO_PIN_15
#define CMD_FIRE_PWR_GPIO_Port GPIOE
#define CNC_DO_0_Pin GPIO_PIN_12
#define CNC_DO_0_GPIO_Port GPIOD
#define CNC_DO_1_Pin GPIO_PIN_13
#define CNC_DO_1_GPIO_Port GPIOD
#define CNC_DO_2_Pin GPIO_PIN_14
#define CNC_DO_2_GPIO_Port GPIOD
#define CNC_DO_3_Pin GPIO_PIN_15
#define CNC_DO_3_GPIO_Port GPIOD
#define CMD_CHOPPER_FIRE_Pin GPIO_PIN_6
#define CMD_CHOPPER_FIRE_GPIO_Port GPIOC
#define CMD_CHOPPER_CUT_Pin GPIO_PIN_7
#define CMD_CHOPPER_CUT_GPIO_Port GPIOC
#define CMD_CHOPPER_EXTINCTION_Pin GPIO_PIN_8
#define CMD_CHOPPER_EXTINCTION_GPIO_Port GPIOC
#define CMD_CHOPPER_WAITING_Pin GPIO_PIN_9
#define CMD_CHOPPER_WAITING_GPIO_Port GPIOC
#define DIR_RS1_Pin GPIO_PIN_8
#define DIR_RS1_GPIO_Port GPIOA
#define DIR_RS2_Pin GPIO_PIN_4
#define DIR_RS2_GPIO_Port GPIOD
#define CMD_GAS_Waiting_Pin GPIO_PIN_5
#define CMD_GAS_Waiting_GPIO_Port GPIOB
#define CMD_GAS_Extinction_Pin GPIO_PIN_6
#define CMD_GAS_Extinction_GPIO_Port GPIOB
#define CMD_GAS_Out_Pin GPIO_PIN_7
#define CMD_GAS_Out_GPIO_Port GPIOB
#define CMD_GAS_Fire_Pin GPIO_PIN_9
#define CMD_GAS_Fire_GPIO_Port GPIOB
#define FIRE_DI_0_Pin GPIO_PIN_1
#define FIRE_DI_0_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

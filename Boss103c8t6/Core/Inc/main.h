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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Led_Red_Pin GPIO_PIN_13
#define Led_Red_GPIO_Port GPIOC
#define Led_Green_Pin GPIO_PIN_14
#define Led_Green_GPIO_Port GPIOC
#define Led_Blue_Pin GPIO_PIN_15
#define Led_Blue_GPIO_Port GPIOC
#define Out_Led_Green_Pin GPIO_PIN_0
#define Out_Led_Green_GPIO_Port GPIOA
#define Out_Led_Red_Pin GPIO_PIN_1
#define Out_Led_Red_GPIO_Port GPIOA
#define Out_Midi_Pin GPIO_PIN_2
#define Out_Midi_GPIO_Port GPIOA
#define In_Midi_Pin GPIO_PIN_3
#define In_Midi_GPIO_Port GPIOA
#define ADC_0_Pin GPIO_PIN_4
#define ADC_0_GPIO_Port GPIOA
#define ADC_1_Pin GPIO_PIN_5
#define ADC_1_GPIO_Port GPIOA
#define ADC_3_Pin GPIO_PIN_7
#define ADC_3_GPIO_Port GPIOA
#define ADC_IN_RED_Pin GPIO_PIN_0
#define ADC_IN_RED_GPIO_Port GPIOB
#define ADC_IN_GREEN_Pin GPIO_PIN_1
#define ADC_IN_GREEN_GPIO_Port GPIOB
#define RST_Pin GPIO_PIN_2
#define RST_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_10
#define SCL_GPIO_Port GPIOB
#define USB_VBUS_Pin GPIO_PIN_8
#define USB_VBUS_GPIO_Port GPIOA
#define USB_PULLUP_Pin GPIO_PIN_10
#define USB_PULLUP_GPIO_Port GPIOA
#define swdio_Pin GPIO_PIN_13
#define swdio_GPIO_Port GPIOA
#define swdclk_Pin GPIO_PIN_14
#define swdclk_GPIO_Port GPIOA
#define SW_1_Pin GPIO_PIN_15
#define SW_1_GPIO_Port GPIOA
#define SW_2_Pin GPIO_PIN_3
#define SW_2_GPIO_Port GPIOB
#define In_Tip_Pin GPIO_PIN_4
#define In_Tip_GPIO_Port GPIOB
#define In_Ring_Pin GPIO_PIN_5
#define In_Ring_GPIO_Port GPIOB
#define Out_Tip_Pin GPIO_PIN_6
#define Out_Tip_GPIO_Port GPIOB
#define Out_Ring_Pin GPIO_PIN_7
#define Out_Ring_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

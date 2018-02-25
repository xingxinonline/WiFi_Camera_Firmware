/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "wifi_task.h"
      

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define DCMI_POWN_Pin GPIO_PIN_2
#define DCMI_POWN_GPIO_Port GPIOE
#define DCMI_RESET_Pin GPIO_PIN_3
#define DCMI_RESET_GPIO_Port GPIOE
#define DCMI_XCLK_Pin GPIO_PIN_5
#define DCMI_XCLK_GPIO_Port GPIOE
#define RTC_SDA_Pin GPIO_PIN_0
#define RTC_SDA_GPIO_Port GPIOF
#define RTC_SCL_Pin GPIO_PIN_1
#define RTC_SCL_GPIO_Port GPIOF
#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOF
#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOF
#define KEY3_Pin GPIO_PIN_4
#define KEY3_GPIO_Port GPIOF
#define KEY4_Pin GPIO_PIN_5
#define KEY4_GPIO_Port GPIOF
#define LCD_DIM_Pin GPIO_PIN_9
#define LCD_DIM_GPIO_Port GPIOF
#define MOTOR_STEP3_Pin GPIO_PIN_0
#define MOTOR_STEP3_GPIO_Port GPIOA
#define MOTOR_DIR3_Pin GPIO_PIN_1
#define MOTOR_DIR3_GPIO_Port GPIOA
#define MCU_PC_TX_Pin GPIO_PIN_2
#define MCU_PC_TX_GPIO_Port GPIOA
#define MCU_PC_RX_Pin GPIO_PIN_3
#define MCU_PC_RX_GPIO_Port GPIOA
#define LCD_D0_Pin GPIO_PIN_0
#define LCD_D0_GPIO_Port GPIOG
#define LCD_D1_Pin GPIO_PIN_1
#define LCD_D1_GPIO_Port GPIOG
#define MOTOR_STEP1_Pin GPIO_PIN_9
#define MOTOR_STEP1_GPIO_Port GPIOE
#define MOTOR_DIR1_Pin GPIO_PIN_11
#define MOTOR_DIR1_GPIO_Port GPIOE
#define MOTOR_STEP2_Pin GPIO_PIN_13
#define MOTOR_STEP2_GPIO_Port GPIOE
#define MOTOR_DIR2_Pin GPIO_PIN_14
#define MOTOR_DIR2_GPIO_Port GPIOE
#define MOTOR_STEP4_Pin GPIO_PIN_10
#define MOTOR_STEP4_GPIO_Port GPIOB
#define MOTOR_DIR4_Pin GPIO_PIN_11
#define MOTOR_DIR4_GPIO_Port GPIOB
#define MOTOR_EN2_Pin GPIO_PIN_12
#define MOTOR_EN2_GPIO_Port GPIOB
#define MOTOR_EN3_Pin GPIO_PIN_13
#define MOTOR_EN3_GPIO_Port GPIOB
#define MOTOR_EN4_Pin GPIO_PIN_14
#define MOTOR_EN4_GPIO_Port GPIOB
#define MOTOR_EN5_Pin GPIO_PIN_15
#define MOTOR_EN5_GPIO_Port GPIOB
#define MOTOR_EN1_Pin GPIO_PIN_8
#define MOTOR_EN1_GPIO_Port GPIOD
#define MOTOR_STEP5_Pin GPIO_PIN_12
#define MOTOR_STEP5_GPIO_Port GPIOD
#define MOTOR_DIR5_Pin GPIO_PIN_13
#define MOTOR_DIR5_GPIO_Port GPIOD
#define LCD_RS_Pin GPIO_PIN_14
#define LCD_RS_GPIO_Port GPIOD
#define LCD_RW_Pin GPIO_PIN_15
#define LCD_RW_GPIO_Port GPIOD
#define LCD_D2_Pin GPIO_PIN_2
#define LCD_D2_GPIO_Port GPIOG
#define LCD_D3_Pin GPIO_PIN_3
#define LCD_D3_GPIO_Port GPIOG
#define LCD_D4_Pin GPIO_PIN_4
#define LCD_D4_GPIO_Port GPIOG
#define LCD_D5_Pin GPIO_PIN_5
#define LCD_D5_GPIO_Port GPIOG
#define LCD_D6_Pin GPIO_PIN_6
#define LCD_D6_GPIO_Port GPIOG
#define LCD_D7_Pin GPIO_PIN_7
#define LCD_D7_GPIO_Port GPIOG
#define LCD_EN_Pin GPIO_PIN_8
#define LCD_EN_GPIO_Port GPIOG
#define SD_DETECT_Pin GPIO_PIN_8
#define SD_DETECT_GPIO_Port GPIOA
#define MCU_WIFI_TX_Pin GPIO_PIN_9
#define MCU_WIFI_TX_GPIO_Port GPIOA
#define MCU_WIFI_RX_Pin GPIO_PIN_10
#define MCU_WIFI_RX_GPIO_Port GPIOA
#define MOTOR_LED_Pin GPIO_PIN_11
#define MOTOR_LED_GPIO_Port GPIOA
#define TEST_LED_Pin GPIO_PIN_12
#define TEST_LED_GPIO_Port GPIOA
#define DCMI_SCL_Pin GPIO_PIN_6
#define DCMI_SCL_GPIO_Port GPIOB
#define DCMI_SDA_Pin GPIO_PIN_7
#define DCMI_SDA_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

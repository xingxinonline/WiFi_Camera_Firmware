/*
***************************************************************************************************
*                               LCD 1602 Driver API
*
* File   : lcd_driver.h
* Author : Douglas Xie
* Date   : 2017.11.07
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

/* Includes -------------------------------------------------------------------------------------*/
#include "global_config.h"

/* Macro defines --------------------------------------------------------------------------------*/
/* LCD1602 control pins signal */
#define LCD_RS_1()      HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET)
#define LCD_RS_0()      HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET)
#define LCD_RW_1()      HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_SET)
#define LCD_RW_0()      HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET)
#define LCD_EN_1()      HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET)
#define LCD_EN_0()      HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET)
/* LCD1602 data pins and port */
#define LCD_DATA_PINS   (LCD_D0_Pin|LCD_D1_Pin|LCD_D2_Pin|LCD_D3_Pin|LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin)
#define LCD_DATA_PORT   LCD_D0_GPIO_Port
/* LCD1602 data input & output register bit 0~7 */
#define LCD_DATA_IN     LCD_DATA_PORT->IDR
#define LCD_DATA_OUT    LCD_DATA_PORT->ODR



/* LCD1602 line number */
#define LCD_LINE_1      1
#define LCD_LINE_2      2

/* Data Type Define -----------------------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------------------------*/

/* Function declaration -------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   LCD Busy Waiting
* @Param   
* @Note    read lcd busy state and return
* @Return  true-busy, false-not busy
*******************************************************************************/
bool LCD_Busy(void);

/*******************************************************************************
* @Brief   LCD Wirte Command
* @Param   
* @Note    write command byte to lcd
* @Return  
*******************************************************************************/
void LCD_WriteCommand(uint8_t cmd);

/*******************************************************************************
* @Brief   LCD Wirte Data
* @Param   
* @Note    write data byte to lcd
* @Return  
*******************************************************************************/
void LCD_WriteData(uint8_t data);

/*******************************************************************************
* @Brief   LCD Set Position
* @Param   line[in]:  display line      1, 2 or LCD_LINE_1, LCD_LINE_2
*          index[in]: cursor position   0 ~ 15
*               line1:  0x00------------0x0f(16 symble)  0x10-------0x27
*               line2:  0x40------------0x4f(16 symble)  0x50-------0x67
* @Note    write command to set cursor position
* @Return  
*******************************************************************************/
void LCD_SetPosition(uint8_t line, uint8_t index);

/*******************************************************************************
* @Brief   LCD Display One Char
* @Param   ch[in]: char data input
* @Note    display char in target position
* @Return  
*******************************************************************************/
void LCD_DisplayChar(uint8_t ch);

/*******************************************************************************
* @Brief   LCD Display String
* @Param   str[in]: string data pointer
* @Note    display string in target position
* @Return  
*******************************************************************************/
void LCD_DisplayString(uint8_t *str);

/*******************************************************************************
* @Brief   LCD All Shift Left
* @Param   
* @Note    display string all shift left one step for rolling display
* @Return  
*******************************************************************************/
void LCD_AllShiftLeft(void);

/*******************************************************************************
* @Brief   LCD All Shift Right
* @Param   
* @Note    display string all shift right one step for rolling display
* @Return  
*******************************************************************************/
void LCD_AllShiftRight(void);

/*******************************************************************************
* @Brief   LCD Clear Display
* @Param   
* @Note    clear all display information
* @Return  
*******************************************************************************/
void LCD_ClearDisplay(void);

/*******************************************************************************
* @Brief   LCD Cursor Return
* @Param   
* @Note    cursor return to start position
* @Return  
*******************************************************************************/
void LCD_CursorReturn(void);

/*******************************************************************************
* @Brief   LCD Close
* @Param   
* @Note    close lcd
* @Return  
*******************************************************************************/
void LCD_Close(void);

/*******************************************************************************
* @Brief   LCD Open
* @Param   
* @Note    open lcd
* @Return  
*******************************************************************************/
void LCD_Open(void);

/*******************************************************************************
* @Brief   LCD Flicker Char
* @Param   line[in]:  display line      1, 2 or LCD_LINE_1, LCD_LINE_2
*          index[in]: cursor position   0 ~ 15
* @Note    flicker char in target position
*           0x0D cursor not display
*           0x0F cursor display and flicker
* @Return  
*******************************************************************************/
void LCD_FlickerChar(uint8_t line, uint8_t index);

/*******************************************************************************
* @Brief   LCD Disable Flicker
* @Param   
* @Note    disable flicker function
* @Return  
*******************************************************************************/
void LCD_DisableFlicker(void);

/*******************************************************************************
* @Brief   LCD Disable Flicker
* @Param   
* @Note    disable flicker function
* @Return  
*******************************************************************************/
void LCD_Init(void);



#endif /* LCD_DRIVER_H */


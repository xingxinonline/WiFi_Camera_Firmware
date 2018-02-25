/*
***************************************************************************************************
*                               LCD 1602 Driver API
*
* Command List: 
*           0x38: 16x2 display mode, 5x8 symbol, 8bit data, excute command time 40us
*           0x0C: display on, cursor off, 40us
*           0x08: display off, cursor off, 40us
*           0x0D: symbol flicker, cursor off
*           0x0F: symbol flicker, cursor on
*           0x06: auto move cursor right after write data  40us
*           0x04: auto move cursor left after write data   40us
*           0x01: clear display     1.64ms
*           0x02: cursor return to start position   1.64ms
*           0x18: display string all left shift one step for rolling display    40us
*           0x1C: display string all right shift one step for rolling display   40us
*
* File   : lcd_driver.c
* Author : Douglas Xie
* Date   : 2017.11.07
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/


/* Includes -------------------------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "string.h"

#include "global_config.h"
#include "main.h"
#include "lcd_driver.h"

/* Private variables ----------------------------------------------------------------------------*/


/* Function declaration -------------------------------------------------------------------------*/


/* Function implement ---------------------------------------------------------------------------*/
#define LCD_CMD_DELAY_MS        5 

/*******************************************************************************
* @Brief   LCD Small Delay US
* @Param   
* @Note    delay us
* @Return  
*******************************************************************************/
void LCD_SmallDelay(void)
{
    //uint32_t i = 0;
    //for(i = 0; i < 180; i++);
}

/*******************************************************************************
* @Brief   LCD Small Delay US
* @Param   
* @Note    delay us
* @Return  
*******************************************************************************/
void LCD_SetDataInput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* Config lcd data pins to be input mode */
    GPIO_InitStruct.Pin = LCD_DATA_PINS;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/*******************************************************************************
* @Brief   LCD Small Delay US
* @Param   
* @Note    delay us
* @Return  
*******************************************************************************/
void LCD_SetDataOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* Config lcd data pins to be output mode */
    GPIO_InitStruct.Pin = LCD_DATA_PINS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/*******************************************************************************
* @Brief   LCD Busy Waiting
* @Param   
* @Note    read lcd busy state and return
* @Return  true-busy, false-not busy
*******************************************************************************/
bool LCD_Busy(void)
{          
    bool busy_state = false;
    
    LCD_SetDataInput();
    LCD_RS_0();
    LCD_RW_1();
    LCD_EN_1();
    LCD_SmallDelay();
    if((LCD_DATA_IN & 0x80) != 0)
    {
        busy_state = true;
    }
    LCD_EN_0();
    LCD_SetDataOutput();
    LCD_SmallDelay();

    return busy_state; 
}

/*******************************************************************************
* @Brief   LCD Wirte Command
* @Param   
* @Note    write command byte to lcd
* @Return  
*******************************************************************************/
void LCD_WriteCommand(uint8_t cmd)
{          
    uint32_t io_state = 0;
    
    while(LCD_Busy() == true)
    {
        vTaskDelay(1);
    }
    
    /* Write command byte */
    LCD_RS_0();
    LCD_RW_0();
    io_state = LCD_DATA_OUT & 0xFFFFFF00; 
    io_state |= cmd;
    LCD_DATA_OUT = io_state;
    
    /* EN pin fall edge to excute command */
    LCD_SmallDelay();
    LCD_EN_1();
    LCD_SmallDelay();
    LCD_EN_0();
}

/*******************************************************************************
* @Brief   LCD Wirte Data
* @Param   
* @Note    write data byte to lcd
* @Return  
*******************************************************************************/
void LCD_WriteData(uint8_t data) 
{                         
    uint32_t io_state = 0;

    while(LCD_Busy() == true)
    {
        vTaskDelay(1);
    }
    
    /* Write data byte */
    LCD_RS_1();
    LCD_RW_0();
    io_state = LCD_DATA_OUT & 0xFFFFFF00; 
    io_state |= data;
    LCD_DATA_OUT = io_state;

    /* EN pin fall edge to excute command */
    LCD_SmallDelay();
    LCD_EN_1();
    LCD_SmallDelay();
    LCD_EN_0();
}

/*******************************************************************************
* @Brief   LCD Set Position
* @Param   line[in]:  display line      1, 2 or LCD_LINE_1, LCD_LINE_2
*          index[in]: cursor position   0 ~ 15
*               line1:  0x00------------0x0f(16 symble)  0x10-------0x27
*               line2:  0x40------------0x4f(16 symble)  0x50-------0x67
* @Note    write command to set cursor position
* @Return  
*******************************************************************************/
void LCD_SetPosition(uint8_t line, uint8_t index)
{                         
    if(line == LCD_LINE_1)
    {
        LCD_WriteCommand(index | 0x80);
    }
    else
    {
        LCD_WriteCommand(index | 0xC0);
    }
}

/*******************************************************************************
* @Brief   LCD Display One Char
* @Param   ch[in]: char data input
* @Note    display char in target position
* @Return  
*******************************************************************************/
void LCD_DisplayChar(uint8_t ch)
{
    LCD_WriteData(ch);
}

/*******************************************************************************
* @Brief   LCD Display String
* @Param   str[in]: string data pointer
* @Note    display string in target position
* @Return  
*******************************************************************************/
void LCD_DisplayString(uint8_t *str)
{
    uint8_t length = 0;
    uint8_t *symbol = str;
    
    while((*symbol != '\0') && (length < 16))
    {
        LCD_WriteData(*symbol);
        symbol++;
        length++;
    }
    while(length < 16)
    {
        LCD_WriteData(' ');
        symbol++;
        length++;
    }
}

/*******************************************************************************
* @Brief   LCD All Shift Left
* @Param   
* @Note    display string all shift left one step for rolling display
* @Return  
*******************************************************************************/
void LCD_AllShiftLeft(void)
{
    LCD_WriteCommand(0x18);
}

/*******************************************************************************
* @Brief   LCD All Shift Right
* @Param   
* @Note    display string all shift right one step for rolling display
* @Return  
*******************************************************************************/
void LCD_AllShiftRight(void)
{
    LCD_WriteCommand(0x1C);
}

/*******************************************************************************
* @Brief   LCD Clear Display
* @Param   
* @Note    clear all display information
* @Return  
*******************************************************************************/
void LCD_ClearDisplay(void)
{
    LCD_WriteCommand(0x01);
}

/*******************************************************************************
* @Brief   LCD Cursor Return
* @Param   
* @Note    cursor return to start position
* @Return  
*******************************************************************************/
void LCD_CursorReturn(void)
{
    LCD_WriteCommand(0x02);
}

/*******************************************************************************
* @Brief   LCD Close
* @Param   
* @Note    close lcd
* @Return  
*******************************************************************************/
void LCD_Close(void)
{
    LCD_WriteCommand(0x08);
}

/*******************************************************************************
* @Brief   LCD Open
* @Param   
* @Note    open lcd
* @Return  
*******************************************************************************/
void LCD_Open(void)
{
    LCD_WriteCommand(0x0C);
}

/*******************************************************************************
* @Brief   LCD Flicker Char
* @Param   line[in]:  display line      1, 2 or LCD_LINE_1, LCD_LINE_2
*          index[in]: cursor position   0 ~ 15
* @Note    flicker char in target position
*           0x0D cursor not display
*           0x0F cursor display and flicker
* @Return  
*******************************************************************************/
void LCD_FlickerChar(uint8_t line, uint8_t index)
{
    LCD_WriteCommand(0x0D);
    LCD_SetPosition(line, index);
}

/*******************************************************************************
* @Brief   LCD Disable Flicker
* @Param   
* @Note    disable flicker function
* @Return  
*******************************************************************************/
void LCD_DisableFlicker(void)
{
    LCD_WriteCommand(0x0C);
}

/*******************************************************************************
* @Brief   LCD Disable Flicker
* @Param   
* @Note    disable flicker function
* @Return  
*******************************************************************************/
void LCD_Init(void)
{   
    LCD_WriteCommand(0x38);  /* 16x2 display, 5x7 symbol, 8bit data */
    vTaskDelay(LCD_CMD_DELAY_MS);
    LCD_WriteCommand(0x08);  /* lcd off */
    vTaskDelay(LCD_CMD_DELAY_MS);
    LCD_WriteCommand(0x06);  /* cursor auto move right */
    vTaskDelay(LCD_CMD_DELAY_MS);
    LCD_WriteCommand(0x01);  /* clear dipslay */
    vTaskDelay(LCD_CMD_DELAY_MS);
    LCD_WriteCommand(0x0c);  /* lcd on, cursor off */
    vTaskDelay(LCD_CMD_DELAY_MS);
}



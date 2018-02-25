/*
***************************************************************************************************
*                                     LCD Display Task
*
* File   : display_task.c
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
#include "display_task.h"
#include "debug_task.h"

/* Private variables ----------------------------------------------------------------------------*/
QueueHandle_t display_queue;

/* Function declaration -------------------------------------------------------------------------*/


/* Task Function implement ----------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Display Show Task 
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Disp_ShowTask(void * argument)
{    
    Disp_Request_t  display;
    
    /* Create display queue and register queue for debug */
    if( display_queue == NULL )
    {
        display_queue = xQueueCreate(DISP_QUEUE_LENGTH, DISP_QUEUE_ITEM_SIZE);
        vQueueAddToRegistry( display_queue, "Display Queue" );
    }
    
    DBG_SendMessage(DBG_MSG_TASK_STATE, "Display Task Start\r\n");
    
    /* Display initial and show launch information */
    LCD_Init();
    LCD_SetPosition(1, 0);
    LCD_DisplayString(APP_NAME_DISP);
    LCD_SetPosition(2, 0);
    LCD_DisplayString(APP_VER_DISP);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    /* Infinite loop */
    for(;;)
    {    
        /* Receive a display request */
        if( xQueueReceive(display_queue, &display, (TickType_t) DISP_RX_TICK_WAIT))
		{
            switch(display.source)
            {
            /* release information */
            case DISP_RELEASE:

            /* debug information */
            case DISP_DBG_WIFI:
            case DISP_DBG_CLIENT:
            case DISP_DBG_MOTOR:
            case DISP_DBG_BUTTON:
                if(display.show_line1 == true)
                {
                    LCD_SetPosition(1, 0);
                    LCD_DisplayString(display.message1);
                }
                if(display.show_line2 == true)
                {
                    LCD_SetPosition(2, 0);
                    LCD_DisplayString(display.message2);
                }
                break;
            default:
                break;
            }
                                   
		}
    }
}
                                   
                                   
                                   

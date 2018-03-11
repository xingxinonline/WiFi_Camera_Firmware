/*
***************************************************************************************************
*                                     Micro SD Operate Task
*
* File   : sd_task.c
* Author : Douglas Xie
* Date   : 2018.03.11
***************************************************************************************************
* Copyright (C) 2018 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

/* Includes -------------------------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "string.h"

#include "global_config.h"
#include "main.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "sd_task.h"
#include "debug_task.h"


/* Private variables ----------------------------------------------------------------------------*/

/* Function declaration -------------------------------------------------------------------------*/

/* Task Function implement ----------------------------------------------------------------------*/


/*******************************************************************************
* @Brief   Micro SD Manage Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void SD_ManageTask(void * argument)
{
    DBG_SendMessage(DBG_MSG_TASK_STATE, "Micro SD Task Start\r\n");
    
        
    /* Infinite loop */
    for(;;)
    {    
        
        vTaskDelay(10);
    }
}



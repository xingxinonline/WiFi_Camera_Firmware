/*
***************************************************************************************************
*                                     LCD Display Task
*
* File   : display_task.h
* Author : Douglas Xie
* Date   : 2017.11.07
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifndef DISP_TASK_H
#define DISP_TASK_H

/* Includes -------------------------------------------------------------------------------------*/
#include "global_config.h"

/* Macro defines --------------------------------------------------------------------------------*/
/* Task period */
#define DISP_TASK_PERIOD        (10 / portTICK_PERIOD_MS)

/* Display queue parameter */

#define DISP_RX_TICK_WAIT       (1000 / portTICK_PERIOD_MS)
#define DISP_QUEUE_LENGTH       5                           /* Queue max item number */
#define DISP_QUEUE_ITEM_SIZE    (sizeof(Disp_Request_t))    /* Item size is Disp_Request_t type */      


/* Data Type Define -----------------------------------------------------------------------------*/
/* Display request source enum list */
typedef enum
{
    /* Release display information */
    DISP_RELEASE,

    /* Debug display information */
    DISP_DBG_WIFI,
    DISP_DBG_CLIENT,
    DISP_DBG_MOTOR,
    DISP_DBG_BUTTON,

} Disp_Source_t;

/* Display request queue item struct */
typedef struct
{
    Disp_Source_t source;
    bool show_line1;
    bool show_line2;
    uint8_t message1[17];
    uint8_t message2[17];
} Disp_Request_t;

/* Public variables ----------------------------------------------------------------------------*/
extern QueueHandle_t display_queue;


/* Function declaration -------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Display Show Task 
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Disp_ShowTask(void * argument);


#endif /* DISP_TASK_H */


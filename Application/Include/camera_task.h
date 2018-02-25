/*
***************************************************************************************************
*                                     Camera Photo Task
*
* File   : camera_task.h
* Author : Douglas Xie
* Date   : 2017.11.10
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/


#ifndef CAMERA_TASK_H
#define CAMERA_TASK_H

/* Includes -------------------------------------------------------------------------------------*/
#include "global_config.h"

/* Macro defines --------------------------------------------------------------------------------*/
/* Task period */
#define CAMERA_TASK_PERIOD      (100 / portTICK_PERIOD_MS)

/* Camera interface define */
#define CAMERA_CLOCK_TIMER      TIM9
#define hcamera_clock_timer     htim9

#define CAMERA_DELAY_TIMER      TIM14
#define hcamera_delay_timer     htim14

#define hcamera_dcmi            hdcmi             
#define hcamera_dma             hdma_dcmi

/* Camera file buffer size define */
#define CAMERA_BUFF_SIZE            40960       /* 40kByte */
#define CAMERA_FILENAME_SIZE        18          /* eg: 20180214112456.jpg */

/* Camera event group single event */
#define CAMERA_EVENT_PHOTO_START    (1 << 0)
#define CAMERA_EVENT_PHOTO_DONE     (1 << 1)
#define CAMERA_EVENT_SAVE_START     (1 << 2)
#define CAMERA_EVENT_SAVE_DONE      (1 << 3)
#define CAMERA_EVENT_POST_START     (1 << 4)
#define CAMERA_EVENT_POST_DONE      (1 << 5)

/* Camera event group max waiting time */
#define CAMERA_EVENT_WAITING        (1000 / portTICK_PERIOD_MS)

/* Data Type Define -----------------------------------------------------------------------------*/
/* Camera photo task state */
typedef enum
{
    CAMERA_CONFIG = 1,
    CAMERA_START,
    CAMERA_RUNNING,
    CAMERA_SAVE,
    CAMERA_IDLE
    
} Camera_State_t;

typedef struct
{
    uint8_t  data[CAMERA_BUFF_SIZE];
    uint32_t length;
    uint8_t  filename[CAMERA_FILENAME_SIZE+2];
} Camera_FiFo_t;

/* Camera buffer struct */
typedef struct
{
    Camera_FiFo_t fifo_buffer[2];
    uint32_t fifo_input      : 1;
    uint32_t fifo_output     : 1;
    uint32_t fifo_reserved   : 30;
} Camera_Buffer_t;

/* Public variables ----------------------------------------------------------------------------*/
/* FreeRTOS event group handle */
extern EventGroupHandle_t  camera_event_group;

/* Camera fifo buffer */
extern Camera_Buffer_t     camera_info;

/* Function declaration -------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Camera Photo Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Camera_PhotoTask(void * argument);

/*******************************************************************************
* @Brief   Camera Save Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Camera_SaveTask(void * argument);


#endif /* CAMERA_TASK_H */


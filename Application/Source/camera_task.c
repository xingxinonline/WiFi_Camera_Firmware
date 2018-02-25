/*
***************************************************************************************************
*                                     Camera Photo Task
*
* File   : camera_task.c
* Author : Douglas Xie
* Date   : 2017.11.10
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
#include "wifi_task.h"
#include "client_task.h"
#include "display_task.h"
#include "camera_task.h"
#include "debug_task.h"

#include "ov7670.h"
#include "sccb.h"
#include "delay.h"

/* Private variables ----------------------------------------------------------------------------*/
extern TIM_HandleTypeDef    htim9;
extern TIM_HandleTypeDef    htim14;
extern DCMI_HandleTypeDef   hdcmi;
extern DMA_HandleTypeDef    hdma_dcmi;

#ifdef PRINT_JPG_DATA
extern UART_HandleTypeDef   huart2;
bool tx_finish = false;
#endif

/* Camera fifo buffer */
Camera_Buffer_t     camera_info;

/* FreeRTOS event group handle */
EventGroupHandle_t  camera_event_group;

/* Camera task debug message */
DBG_MsgBuf_t camera_dbg;

/* Function declaration -------------------------------------------------------------------------*/
void Camera_DCMI_Init(void);

/* Task Function implement ----------------------------------------------------------------------*/


/*******************************************************************************
* @Brief   Camera Photo Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Camera_PhotoTask(void * argument)
{
    uint32_t pdma_buff;
    EventBits_t event_bits;
    Camera_State_t camera_state = CAMERA_IDLE;
    
    /* Create FreeRTOS event group */
    camera_event_group = xEventGroupCreate();        
    memset(&camera_info, 0, sizeof(Camera_Buffer_t));
    
    vTaskDelay(1000);
    DBG_SendMessage(DBG_MSG_TASK_STATE, "Camera Photo Task Start\r\n");
    
    /* Infinite loop */
    for(;;)
    {    
        switch(camera_state)
        {
        case CAMERA_CONFIG:
            camera_state = CAMERA_START;
            DBG_SendMessage( DBG_MSG_CAMERA, "Camera: Start Task\r\n" );
            
        case CAMERA_START:
            /* Start when last fifo buffer has save */
            if(camera_info.fifo_input == camera_info.fifo_output)
            {
                /* Recieve data in new buffer */
                camera_info.fifo_input = ~camera_info.fifo_input;
                memset(&(camera_info.fifo_buffer[camera_info.fifo_input]), 0, sizeof(Camera_FiFo_t));
                pdma_buff = (uint32_t)camera_info.fifo_buffer[camera_info.fifo_input].data;
                
                /* OV7670 clock provided by pwm timer */
                HAL_TIM_Base_Start_IT(&hcamera_delay_timer);
                HAL_TIM_PWM_Start(&hcamera_clock_timer,TIM_CHANNEL_1);
            
                /* OV7670 picture size and parameter config */
                OV2640_JPEGConfig(JPEG_640x480);
                DBG_SendMessage( DBG_MSG_CAMERA, "Camera: Config OK\r\n" );
                
                /* Reset DCMI and start DMA receive */
                Camera_DCMI_Init();
                HAL_DCMI_Start_DMA(&hcamera_dcmi, DCMI_MODE_SNAPSHOT, pdma_buff, (CAMERA_BUFF_SIZE >> 2));
                
                camera_state = CAMERA_RUNNING;
                DBG_SendMessage( DBG_MSG_CAMERA, "Camera: Photo Start\r\n" );
            }
            break;
            
        case CAMERA_RUNNING:
            /* Wait for camera photo done event */
            event_bits = xEventGroupWaitBits(camera_event_group,
                                             CAMERA_EVENT_PHOTO_DONE,
                                             pdTRUE,
                                             pdTRUE,
                                             CAMERA_EVENT_WAITING );
            
            if(( event_bits & CAMERA_EVENT_PHOTO_DONE ) == CAMERA_EVENT_PHOTO_DONE )
            {
                xEventGroupClearBits(camera_event_group, CAMERA_EVENT_PHOTO_DONE);
               
                /* Stop camera photo */
                HAL_DCMI_Stop(&hcamera_dcmi);
                HAL_DCMI_DeInit(&hcamera_dcmi);
                HAL_TIM_Base_Stop_IT(&hcamera_delay_timer);
                HAL_TIM_PWM_Stop(&hcamera_clock_timer,TIM_CHANNEL_1);   
                
                if(camera_info.fifo_buffer[camera_info.fifo_input].length == 0)
                {
                    camera_state = CAMERA_IDLE;
                    DBG_SendMessage( DBG_MSG_CAMERA, "Camera: Photo Error\r\n" );
                }
                else
                {
                    camera_state = CAMERA_SAVE;
                    DBG_SendMessage( DBG_MSG_CAMERA, "Camera: Photo Done\r\n" );
                }
            }
            break;
            
        case CAMERA_SAVE:                                        
            /* Post save event to save task */
            xEventGroupSetBits( camera_event_group, CAMERA_EVENT_SAVE_START);
            
            camera_state = CAMERA_IDLE;
            DBG_SendMessage( DBG_MSG_CAMERA, "Camera: Photo Save\r\n" );
            break;
            
        case CAMERA_IDLE:
            vTaskDelay(1000);
            event_bits = xEventGroupWaitBits(camera_event_group,
                                             CAMERA_EVENT_PHOTO_START,
                                             pdTRUE,
                                             pdTRUE,
                                             0 );
            
            if(( event_bits & CAMERA_EVENT_PHOTO_START ) == CAMERA_EVENT_PHOTO_START )
            {
                xEventGroupClearBits(camera_event_group, CAMERA_EVENT_PHOTO_START);
                camera_state = CAMERA_CONFIG;
            }
            break;
            
        default:
            break;
        }
        
        vTaskDelay(1);
    }
}


/*******************************************************************************
* @Brief   Camera Save Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Camera_SaveTask(void * argument)
{
    uint32_t i = 0;
    uint32_t fifo_index = 0;
    EventBits_t event_bits;
    
    DBG_SendMessage(DBG_MSG_TASK_STATE, "Camera Save Task Start\r\n");
    
    /* Infinite loop */
    for(;;)
    {    
        /* Wait for camera save event */
        event_bits = xEventGroupWaitBits(camera_event_group,
                                         CAMERA_EVENT_SAVE_START,
                                         pdTRUE,
                                         pdTRUE,
                                         CAMERA_EVENT_WAITING );
        
        if(( event_bits & CAMERA_EVENT_SAVE_START ) == CAMERA_EVENT_SAVE_START )
        {
            xEventGroupClearBits(camera_event_group, CAMERA_EVENT_SAVE_START);
            
            fifo_index = camera_info.fifo_input;
            
            /* Data buffer has valid data */
            if((camera_info.fifo_buffer[fifo_index].data[0] == 0xFF) && (camera_info.fifo_buffer[fifo_index].data[1] == 0xD8))
            {        
                /* TODO: ADD FILENAME HERE, REPLACE THE FILENAME STRING TO DATETIME */
                /* Fill filename to fifo */
                sprintf(camera_info.fifo_buffer[fifo_index].filename, "%s.jpg", "20180214005632");
                
                /* Post wifi send event to wifi task */
                xEventGroupSetBits( camera_event_group, CAMERA_EVENT_POST_START);
                
#ifdef EN_DEBUG
                DBG_Sprintf(camera_dbg.buf, "File:%s\r\nSize:%d\r\n", 
                            camera_info.fifo_buffer[fifo_index].filename, 
                            camera_info.fifo_buffer[fifo_index].length);
                DBG_SendMessage( DBG_MSG_CAMERA, camera_dbg.buf );
#endif
                
#ifdef PRINT_JPG_DATA
                for(i = 0; i < camera_info.fifo_buffer[fifo_index].length; i += 100)
                {
                    tx_finish = false;
                    HAL_UART_Transmit_IT(&huart2, &(camera_info.fifo_buffer[fifo_index].data[i]), 100);
                    
                    while(tx_finish == false)
                    {
                        vTaskDelay(1);
                    }
                }
#endif
            }     
            /* Error image data */
            else
            {
                DBG_Sprintf(camera_dbg.buf, "Error Image Data\r\n", fifo_index);
                DBG_SendMessage( DBG_MSG_CAMERA, camera_dbg.buf );
            }
                      
            camera_info.fifo_output = camera_info.fifo_input;
        }
        vTaskDelay(10);
    }
}


/*******************************************************************************
* @Brief   Camera DCMI Initial
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Camera_DCMI_Init(void)
{
    HAL_DCMI_DeInit(&hcamera_dcmi);
    
    hcamera_dcmi.Instance = DCMI;
    hcamera_dcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
    hcamera_dcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
    hcamera_dcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
    hcamera_dcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
    hcamera_dcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
    hcamera_dcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
    hcamera_dcmi.Init.JPEGMode = DCMI_JPEG_ENABLE;
    if (HAL_DCMI_Init(&hcamera_dcmi) != HAL_OK)
    {
        Error_Handler();
    }
}

/*******************************************************************************
* @Brief   Camera DCMI Photo Finish Interrupt
* @Param   
* @Note    stop dcmi dma and save fifo length
* @Return  
*******************************************************************************/
void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *phdcmi)
{
    uint32_t image_length = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if(&hcamera_dcmi == phdcmi)
    {
        /* Save fifo length to global struct */
        //camera_info.fifo_buffer[camera_info.fifo_input].length = CAMERA_BUFF_SIZE - (4 * __HAL_DMA_GET_COUNTER(&hcamera_dma));
        image_length = CAMERA_BUFF_SIZE - (4 * __HAL_DMA_GET_COUNTER(&hcamera_dma));
        while(camera_info.fifo_buffer[camera_info.fifo_input].data[image_length - 1] == 0)
        {
            image_length--;
            if(image_length == 0)
            {
                break;
            }
        }
        camera_info.fifo_buffer[camera_info.fifo_input].length = image_length;
        
        /* Post photo done event by set evnet bit */
        xEventGroupSetBitsFromISR( camera_event_group, CAMERA_EVENT_PHOTO_DONE, &xHigherPriorityTaskWoken );
        
        /* Task yield if necessary */
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}

#ifdef PRINT_JPG_DATA
/*******************************************************************************
* @Brief   Debug print uart complete callback
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *phuart)
{
    if(phuart == &huart2)
    {
        tx_finish = true;
    }
}
#endif
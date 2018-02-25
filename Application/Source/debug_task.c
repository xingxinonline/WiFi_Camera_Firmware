/*
***************************************************************************************************
*                                   Debug Message Print Task
*
* File   : debug_task.c
* Author : Douglas Xie
* Date   : 2017.10.13
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifdef EN_DEBUG

/* Include Head Files ---------------------------------------------------------------------------*/
/* Driver include */
#include "main.h"
#include "debug_task.h"

/* Application include */
#include "global_config.h"
//@TODO: ADD MONITOR-->>>  #include "monitor_task.h"

/* FreeRTOS Kernel include */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"


/* Global Variable ------------------------------------------------------------------------------*/
QueueHandle_t debug_queue;
TimerHandle_t debug_timer;

volatile uint32_t cpu_load = 0;
volatile uint32_t cpu_load_max = 0;

extern UART_HandleTypeDef huart2;

/* Function Declaration -------------------------------------------------------------------------*/
static void prvDebugTimerCallback( TimerHandle_t xTimer );
void DBG_PrintFreeHeapSize(void);
void DBG_PrintTaskStatus(void);
void DBG_PrintMessage(uint8_t *message, uint8_t max_size);

/* Function Implement ---------------------------------------------------------------------------*/

/*******************************************************************************
* @brief  Debug Print Task
* @param  
* @note   Receive one print message from queue and send out from uart every loop
* @retval  
*******************************************************************************/
void DBG_PrintTask(void *pvParameters)
{
    uint8_t message[DBG_QUEUE_ITEM_SIZE];
    
    pvParameters = pvParameters;

    /* Create debug queue and register queue for debug */
    if( debug_queue == NULL )
    {
        debug_queue = xQueueCreate(DBG_QUEUE_LENGTH, DBG_QUEUE_ITEM_SIZE);
        vQueueAddToRegistry( debug_queue, "Debug Queue" );
    }
    
    /* Create debug timer for monitor free heap size */
    debug_timer = xTimerCreate ("Debug Timer",                              /* A text name, purely to help debugging. */
                                 (DBG_TIMER_PERIOD / portTICK_PERIOD_MS),	/* The timer period */
                                 pdTRUE,									/* Set enable or disable xAutoReload. */
                                 (void *) 0,                                /* The timer ID */
                                 prvDebugTimerCallback);					/* The callback function that inspects the status of all the other tasks. */
        
    /* Start debug timer */
    xTimerStart(debug_timer, (DBG_TIMER_PERIOD / portTICK_PERIOD_MS)); 
    
    DBG_SendMessage(DBG_MSG_TASK_STATE, "Debug Task Start\r\n");
    DBG_PrintFreeHeapSize();
    
    /* Task entity */
    while(1)
    {
        /* Receive a print message from debug queue */
        if( xQueueReceive(debug_queue, message, (TickType_t) DBG_RX_TICK_WAIT))
		{
            /* Receive message success, print from uart */
            DBG_PrintMessage(message, DBG_QUEUE_ITEM_SIZE);
		}

        /* Push monitor event */
//@TODO: ADD MONITOR-->>>          Monitor_PushEvent( MONITOR_EVENT_RUN_DEBUG );
    }
}

/*******************************************************************************
* @brief  Debug Timer Timeout Callback
* @param  
* @note   
* @retval 
*******************************************************************************/
static void prvDebugTimerCallback( TimerHandle_t xTimer )
{
    DBG_PrintFreeHeapSize(); 
}


/*******************************************************************************
* @brief  Debug Print Free Heap Size When Changed
* @param  
* @note   call xPortGetFreeHeapSize() & xPortGetMinimumEverFreeHeapSize() to 
*         get free heap and print
* @retval 
*******************************************************************************/
void DBG_PrintFreeHeapSize(void)
{
    uint8_t message[DBG_QUEUE_ITEM_SIZE];
    uint32_t free_size = 0;
    uint32_t mini_size = 0;  
    static uint32_t pre_free_size = 0;
    static uint32_t pre_mini_size = 0;
    
    /* Get free heap size */
    free_size = xPortGetFreeHeapSize();
    mini_size = xPortGetMinimumEverFreeHeapSize();
    
    /* Print message when free heap size changed */
    if(( free_size != pre_free_size )||( mini_size != pre_mini_size ))
    {
        /* Update record */
        pre_free_size = free_size;
        pre_mini_size = mini_size;
        
        /* Build report message */
        DBG_Sprintf((char *)message, "(Now)%d, (Mini)%d \r\n", free_size, mini_size);
        
        /* Send message to print task */
        DBG_SendMessage(DBG_MSG_FREE_HEAP, "Free Heap Size Change:");
        DBG_SendMessage(DBG_MSG_FREE_HEAP, message);
    }
}

/*******************************************************************************
* @brief  Build Debug Message String
* @param  [in]msg_body: original message string
*         [in]msg_append: new string that need to append to message body
* @note   build a new message string that is msg_body + msg_append
* @retval  
*******************************************************************************/
void DBG_BuildMessage(uint8_t* msg_body, uint8_t *msg_append)
{
    DBG_Sprintf((char *)msg_body, "%s%s", msg_body, msg_append);
}


/*******************************************************************************
* @brief  Send Debug Message to Queue
* @param  [in]msg_type: message type which define in head file
*         [in]message: input message string(max size is DBG_QUEUE_ITEM_SIZE)
* @note   send debug message to queue and wait for debug task to print
* @retval  
*******************************************************************************/
void DBG_SendMessage(DBG_MsgType_t msg_type, uint8_t *message)
{
    /* Create debug queue and register queue for debug */
    if( debug_queue == NULL )
    {
        debug_queue = xQueueCreate(DBG_QUEUE_LENGTH, DBG_QUEUE_ITEM_SIZE);
        vQueueAddToRegistry( debug_queue, "Debug Queue" );
    }
    
    /* Print debug message according to message type */
    switch(msg_type)
    {
    case DBG_MSG_COMMON:
    case DBG_MSG_TASK_STATE:
//    case DBG_MSG_FREE_HEAP:        
//    case DBG_MSG_FREE_STACK:
//    case DBG_MSG_MONITOR:
    case DBG_MSG_WIFI_CTRL:
    case DBG_MSG_WIFI_RX:
    case DBG_MSG_CLIENT:
    case DBG_MSG_MOTOR:
    case DBG_MSG_CAMERA:
        /* Send message to queue */
        xQueueSend(debug_queue, message, (TickType_t) 0);
        break;
    default:
        break;
    }
    
    /* Clear message */
    *message = '\0';
}


/*******************************************************************************
* @Brief   DBG Print Message
* @Param   [in]message  - pointer to the print message string
*          [in]max_size  - max size of the massage
* @Note    send print string to PC
* @Return  
*******************************************************************************/
void DBG_PrintMessage(uint8_t *message, uint8_t max_size)
{    
    uint8_t tx_byte = 0;
    
    /* send out string until the end */
    while((*message != '\0') && (tx_byte < max_size))
    {
        HAL_UART_Transmit(&hdbg_uart, message, 1, DBG_TX_TICK_WAIT);
        message++;
        tx_byte++;
    }
}

#endif /* EN_DEBUG */


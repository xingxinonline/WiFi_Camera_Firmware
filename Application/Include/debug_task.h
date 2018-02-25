/*
***************************************************************************************************
*                                   Debug Message Print Task
*
* File   : debug_task.h
* Author : Douglas Xie
* Date   : 2017.10.13
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifndef  DEBUG_TASK
#define  DEBUG_TASK

/* Include Head Files ---------------------------------------------------------------------------*/
#include "global_config.h"

/* Macro Define ---------------------------------------------------------------------------------*/
/* Debug print free heap size period */
#define DBG_TIMER_PERIOD        (500) 

/* Queue send & receive waiting ticks: for xTicksToWait */
#define DBG_RX_TICK_WAIT        (configTICK_RATE_HZ)        /* 1 second */
#define DBG_TX_TICK_WAIT        (10 / portTICK_PERIOD_MS)   /* 10 ms */

/* Debug queue parameter */
#define DBG_QUEUE_LENGTH        (10)    /* Queue max item number */
#define DBG_QUEUE_ITEM_SIZE     (50)    /* Item size is 50 bytes */

/* Debug interface define */
#define DBG_USART               USART2
#define hdbg_uart               huart2

/* Data Type Define -----------------------------------------------------------------------------*/
/* Debug message type define */
typedef enum
{ 
    DBG_MSG_COMMON = 0,
    DBG_MSG_TASK_STATE,     /* Task create, delete, suspend, resume ... state */
    DBG_MSG_FREE_HEAP,
    DBG_MSG_FREE_STACK,
    DBG_MSG_WIFI_CTRL,
    DBG_MSG_WIFI_RX,
    DBG_MSG_CLIENT,
    DBG_MSG_MOTOR,
    DBG_MSG_CAMERA,
    DBG_MSG_MONITOR
} DBG_MsgType_t;

/* Debug message buffer type define */
typedef struct
{
    uint8_t  buf[DBG_QUEUE_ITEM_SIZE];   
} DBG_MsgBuf_t;


#ifdef EN_DEBUG
/*=================================== ENABLE DEBUG FUNCTION =====================================*/

/*******************************************************************************
* @brief  Debug Print Task
* @param  
* @note   Receive one print message from queue and send out from uart every loop
* @retval  
*******************************************************************************/
void DBG_PrintTask(void *pvParameters);

/*******************************************************************************
* @brief  Build Debug Message String
* @param  [in]msg_body: original message string
*         [in]msg_append: new string that need to append to message body
* @note   build a new message string that is msg_body + msg_append
* @retval  
*******************************************************************************/
void DBG_BuildMessage(uint8_t* msg_body, uint8_t *msg_append);

/*******************************************************************************
* @brief  Send Debug Message to Queue
* @param  [in]msg_type: message type which define in head file
*         [in]message: input message string(max size is DBG_QUEUE_ITEM_SIZE)
* @note   send debug message to queue and wait for debug task to print
* @retval  
*******************************************************************************/
void DBG_SendMessage(DBG_MsgType_t msg_type, uint8_t *message);

/* Debug sprintf function replace */
#define DBG_Sprintf                 sprintf

#else  
/*=================================== DISABLE DEBUG FUNCTION ====================================*/
/* Define blank function for compiler */

/* Debug function */
#define DBG_BuildMessage(a, b)
#define DBG_SendMessage(a, b)

/* Debug sprintf function is empty */
#define DBG_Sprintf

#endif /* EN_DEBUG */
#endif /* DEBUG_TASK */

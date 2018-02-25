/*
***************************************************************************************************
*                            Client Communication Task
*
* File   : client_task.h
* Author : Douglas Xie
* Date   : 2018.02.04
***************************************************************************************************
* Copyright (C) 2017-2018 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/


#ifdef USE_DEMO_VERSION

#include "client_task_demo.h"

#else

#ifndef  CLIENT_TASK
#define  CLIENT_TASK

/* Include Head Files ---------------------------------------------------------------------------*/
#include "global_config.h"

/* Macro Define ---------------------------------------------------------------------------------*/
/*** Message Format:
 *  StartCode(5) + Command(1) + Index(2) + Length(2) + Payload(0~1024) + Checksum(1) + EndCode(5)
 ***/
#define MSG_CMD_SIZE            16      /* message command size without payload */
#define MSG_MAX_TX_PAYLOAD      1000
#define MSG_MAX_RX_PAYLOAD      1000
#define MSG_BUFFER_SIZE         (MSG_MAX_RX_PAYLOAD + MSG_CMD_SIZE)
/* Recognize code */
#define MSG_RECOGNIZE_CODE_LEN  5       /* 5 */
#define MSG_START_CODE          0x7B    /* 123 */
#define MSG_END_CODE            0xA8    /* -88 */
/* App get command code */
#define MSG_GET_BASE            0x10
#define MSG_GET_MAC             (MSG_GET_BASE + 1)
#define MSG_GET_IMAGE           (MSG_GET_BASE + 2)
#define MSG_GET_STATE           (MSG_GET_BASE + 3)
#define MSG_GET_VERSION         (MSG_GET_BASE + 4)
/* App set command code */
#define MSG_SET_BASE            0x20
#define MSG_SET_ACCOUNT         (MSG_SET_BASE + 1)
#define MSG_SET_WIFI            (MSG_SET_BASE + 2)
#define MSG_SET_MOTOR           (MSG_SET_BASE + 3)
#define MSG_SET_TIME            (MSG_SET_BASE + 4)
#define MSG_SET_SCH             (MSG_SET_BASE + 5)
/* Device push command code */
#define MSG_PUSH_BASE           0x30
#define MSG_PUSH_IMAGE          (MSG_PUSH_BASE + 1)
#define MSG_PUSH_ACCOUNT        (MSG_PUSH_BASE + 2)
#define MSG_PUSH_ALARM          (MSG_PUSH_BASE + 3)
/* App ota update code */
#define MSG_OTA_BASE            0x40
#define MSG_OTA_REQUEST         (MSG_OTA_BASE + 1)
#define MSG_OTA_BIN             (MSG_OTA_BASE + 2)
#define MSG_OTA_VERIFY          (MSG_OTA_BASE + 3)
/* Factory new magic code */
#define MSG_FACTORY_NEW         0xA5
/* Feedback state code */
#define MSG_FB_BASE             0xF0
#define MSG_FB_OK               (MSG_FB_BASE + 0)
#define MSG_FB_UPDATED          (MSG_FB_BASE + 1)   /* For ota request */
#define MSG_FB_ERROR            (MSG_FB_BASE + 2)

/* WiFi queue parameter */
#define CLIENT_QUEUE_LENGTH     (3)                        /* Queue max item number */
#define CLIENT_QUEUE_ITEM_SIZE  (sizeof(Client_Message_t)) /* Item size is Client_Message_t type */
#define CLIENT_QUEUE_TIMEUOT    (1000 / portTICK_PERIOD_MS)


/* Data Type Define -----------------------------------------------------------------------------*/
#pragma  pack(1)
typedef struct
{
    uint8_t  client_id;
    uint8_t  command;
    uint16_t index;
    uint16_t length;
    uint8_t *payload;
    uint8_t  checksum;

} Client_Message_t;

typedef struct
{
    uint16_t fw_version;
    uint16_t fw_crc16;
    uint32_t fw_size;
    uint32_t write_length;
} Client_Ota_t;
#pragma  pack()

/* Function Declaration -------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Client Communication Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Client_CommTask(void * argument);

/*******************************************************************************
* @Brief   Receive Data Analyzer
* @Param   data[in]: receive data from app
* @Note    analyze data and fill message struct
* @Return  true: analyze success
*          false: analyze error
*******************************************************************************/
bool Client_DataAnalyzer(uint8_t *data, Client_Message_t *msg);

/*******************************************************************************
* @Brief   Command Request Handler
* @Param   request[in]: request string from client
* @Note    process client request and respond 
* @Return  
*******************************************************************************/
void Client_RequestHandler(void);

/*******************************************************************************
* @Brief   Command Respond Handler
* @Param   state[in]: respond state
* @Note    build repond message and send back to client
* @Return  
*******************************************************************************/
void Client_RespondHandler(uint8_t state);

#endif /* CLIENT_TASK */
#endif /* USE_DEMO_VERSION */

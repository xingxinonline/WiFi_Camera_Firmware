/*
***************************************************************************************************
*                            Client Communication Task
*       this is demo version which use demo protocol and PC software TCP_Client V1.00.exe
*
* File   : client_task.h
* Author : Douglas Xie
* Date   : 2017.10.24
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifdef USE_DEMO_VERSION

#ifndef  CLIENT_TASK_DEMO
#define  CLIENT_TASK_DEMO

/* Include Head Files ---------------------------------------------------------------------------*/
#include "global_config.h"

/* Macro Define ---------------------------------------------------------------------------------*/
#define CLIENT_COMMAND_SIZE     8
#define CLIENT_TARGET_SIZE      15
#define CLIENT_CHANNEL_SIZE     4
#define CLIENT_PAYLOAD_SIZE     65
#define CLIENT_BUFFER_SIZE      100  // > (6 + CLIENT_COMMAND_SIZE + CLIENT_TARGET_SIZE + CLIENT_CHANNEL_SIZE + CLIENT_PAYLOAD_SIZE) /* Format: A,B,C,D\r\n */

/* WiFi queue parameter */
#define CLIENT_QUEUE_LENGTH     (3)                        /* Queue max item number */
#define CLIENT_QUEUE_ITEM_SIZE  (sizeof(Client_Command_t)) /* Item size is Client_Command_t type */
#define CLIENT_QUEUE_TIMEUOT    (1000 / portTICK_PERIOD_MS)

/* Data Type Define -----------------------------------------------------------------------------*/
typedef struct
{
    uint8_t client_id;
    uint8_t command[CLIENT_COMMAND_SIZE];  /* GET, SET */
    uint8_t target[CLIENT_TARGET_SIZE];    /* ADC, SPEED, SSID, PWD */
    uint8_t channel[CLIENT_CHANNEL_SIZE];  /* 1, 2, 3, 4, 5 ... */
    uint8_t payload[CLIENT_PAYLOAD_SIZE];  /* Payload data array */
} Client_Command_t;

typedef enum
{
    CLIENT_GET_OK,
    CLIENT_SET_OK,
    
    CLIENT_ERR_COMMAND,
    CLIENT_ERR_TARGET,
    CLIENT_ERR_CHANNEL,
    CLIENT_ERR_PAYLOAD,
    CLIENT_ERR_UNKNOWN,
} Client_State_t;

/* Function Declaration -------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Client Communication Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Client_CommTask(void * argument);

/*******************************************************************************
* @Brief   Command Request Analyzer
* @Param   request[in]: request string from client
* @Note    split request string to command struct
* @Return  true: analyze success
*          false: analyze error
*******************************************************************************/
bool Client_RequestAnalyzer(uint8_t *request, Client_Command_t *command);

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
void Client_RespondHandler(Client_State_t state);

#endif /* CLIENT_TASK_DEMO */
#endif /* USE_DEMO_VERSION */
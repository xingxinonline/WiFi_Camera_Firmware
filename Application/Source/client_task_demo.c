/*
***************************************************************************************************
*                            Client Communication Task
*       this is demo version which use demo protocol and PC software TCP_Client V1.00.exe
*
* File   : client_task.c
* Author : Douglas Xie
* Date   : 2017.10.24
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifdef USE_DEMO_VERSION

/* Include Head Files ---------------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "string.h"

#include "global_config.h"
#include "client_task.h"
#include "wifi_task.h"
#include "motor_task.h"
#include "display_task.h"
#include "camera_task.h"
#include "debug_task.h"

/* Global Variable ------------------------------------------------------------------------------*/
Client_Command_t g_request;     /* client request struct */
Client_Command_t g_respond;     /* client respond struct */

QueueHandle_t request_queue;
QueueHandle_t respond_queue;


/* Function Declaration -------------------------------------------------------------------------*/
void Client_GetFirmwareVersion(void);
void Client_GetMacAddress(void);
void Client_GetSerialNumber(void);
void Client_GetSSID(void);
void Client_GetPassword(void);
void Client_GetMotorFrequency(void);
void Client_GetMotorStep(void);
void Client_GetMotorDirection(void);
void Client_GetCameraImage(void);
void Client_SetMotorFrequency(void);
void Client_SetMotorStep(void);
void Client_SetMotorDirection(void);

/* Command Handler Implement -----------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Client Communication Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Client_CommTask(void * argument)
{
    /* Create client queue and register queue for debug */
    request_queue = xQueueCreate(CLIENT_QUEUE_LENGTH, CLIENT_QUEUE_ITEM_SIZE);
    vQueueAddToRegistry( request_queue, "Request Queue" );
    
    respond_queue = xQueueCreate(CLIENT_QUEUE_LENGTH, CLIENT_QUEUE_ITEM_SIZE);
    vQueueAddToRegistry( respond_queue, "Respond Queue" );
    
    DBG_SendMessage(DBG_MSG_TASK_STATE, "Client Task Start\r\n");
    
    for(;;)
    {
        /* Receive rx_state until get result state or timeout */
        if( xQueueReceive(request_queue, &g_request, (TickType_t) CLIENT_QUEUE_TIMEUOT))
        {
            /* Process request */
            Client_RequestHandler();
        }
    }
}

/*******************************************************************************
* @Brief   Command Request Analyzer
* @Param   request[in]: request string from client
* @Note    split request string to command struct
* @Return  true: analyze success
*          false: analyze error
*******************************************************************************/
bool Client_RequestAnalyzer(uint8_t *request, Client_Command_t *command)
{
    uint8_t i = 0;
    uint8_t req_index = 0;
    
    memset(command, 0, sizeof(Client_Command_t));
    
    /* Get command string */
    for(i = 0; request[req_index] != ','; req_index++, i++)
    {
        if(i < (CLIENT_COMMAND_SIZE - 1))
        {
            (*command).command[i] = request[req_index];
        }
        else
        {
            (*command).command[CLIENT_COMMAND_SIZE - 2] = '*';
            break;
        }
    }
    (*command).command[i] = '\0';
    
    /* Get target string */
    for(i = 0, req_index++; request[req_index] != ','; req_index++, i++)
    {
        if(i < (CLIENT_TARGET_SIZE - 1))
        {
            (*command).target[i] = request[req_index];
        }
        else
        {
            (*command).target[CLIENT_TARGET_SIZE - 2] = '*';
            break;
        }
    }
    (*command).target[i] = '\0';
    
    /* Get channel string */
    for(i = 0, req_index++; request[req_index] != ','; req_index++, i++)
    {
        if(i < (CLIENT_CHANNEL_SIZE - 1))
        {
            (*command).channel[i] = request[req_index];
        }
        else
        {
            (*command).channel[CLIENT_CHANNEL_SIZE - 2] = '*';
            break;
        }
    } 
    (*command).channel[i] = '\0';
    
    /* Get payload string */
    for(i = 0, req_index++; request[req_index] != '\0'; req_index++, i++)
    {
        if(i < (CLIENT_PAYLOAD_SIZE - 1))
        {
            (*command).payload[i] = request[req_index];
        }
        else
        {
            (*command).payload[CLIENT_PAYLOAD_SIZE - 2] = '*';
            break;
        }
    } 
    
    /* Get ride of new line tail */
    while(((*command).payload[i-1] == '\r') || ((*command).payload[i-1] == '\n'))
    {
        i--;
    }
    
    (*command).payload[i] = '\0';
    return true;
}

/*******************************************************************************
* @Brief   Command Request Handler
* @Param   request[in]: request string from client
* @Note    process client request and respond 
* @Return  
*******************************************************************************/
void Client_RequestHandler(void)
{           
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Request Receive\r\n");
    
    /* GET command process */
    if(strcmp(g_request.command, "GET") == 0)
    {
        if(strcmp(g_request.target, "MAC") == 0)
        {
            Client_GetMacAddress();
        }
        else if(strcmp(g_request.target, "FW_VER") == 0)
        {
            Client_GetFirmwareVersion();
        }
        else if(strcmp(g_request.target, "SN") == 0)
        {
            Client_GetSerialNumber();
        }
        else if(strcmp(g_request.target, "SSID") == 0)
        {
            Client_GetSSID();
        }
        else if(strcmp(g_request.target, "PWD") == 0)
        {
            Client_GetPassword();
        }
        else if(strcmp(g_request.target, "FREQ") == 0)
        {
            Client_GetMotorFrequency();
        }
        else if(strcmp(g_request.target, "STEP") == 0)
        {
            Client_GetMotorStep();
        }        
        else if(strcmp(g_request.target, "DIR") == 0)
        {
            Client_GetMotorDirection();
        }
        else if(strcmp(g_request.target, "IMAGE") == 0)
        {
            Client_GetCameraImage();
        }
        else
        {
            Client_RespondHandler( CLIENT_ERR_TARGET );
        }
    }
    /* SET command process */
    else if(strcmp(g_request.command, "SET") == 0)
    {
        if(strcmp(g_request.target, "SN") == 0)
        {
            //Client_SetSerialNumber();
        }
        else if(strcmp(g_request.target, "SSID") == 0)
        {
            //Client_SetSSID();
        }
        else if(strcmp(g_request.target, "PWD") == 0)
        {
            //Client_SetPassword();
        }
        else if(strcmp(g_request.target, "FREQ") == 0)
        {
            Client_SetMotorFrequency();
        }
        else if(strcmp(g_request.target, "STEP") == 0)
        {
            Client_SetMotorStep();
        }        
        else if(strcmp(g_request.target, "DIR") == 0)
        {
            Client_SetMotorDirection();
        }
        else
        {
            Client_RespondHandler( CLIENT_ERR_TARGET );
        }
    }
    else 
    {
        Client_RespondHandler( CLIENT_ERR_COMMAND );
    }
}

/*******************************************************************************
* @Brief   Command Respond Handler
* @Param   state[in]: respond state
* @Note    build repond message and send back to client
* @Return  
*******************************************************************************/
void Client_RespondHandler(Client_State_t state)
{                
    Disp_Request_t disp_req;
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Respond Post\r\n");
        
    switch(state)
    {
    case CLIENT_GET_OK:
        strcpy(g_respond.command, "GET_OK");
        strcpy(g_respond.target, g_request.target);
        strcpy(g_respond.channel, g_request.channel);
        strcpy(g_respond.payload, g_request.payload);
        sprintf(disp_req.message2, "GET %s OK", g_request.target);
        break;
        
    case CLIENT_SET_OK:
        strcpy(g_respond.command, "SET_OK");
        strcpy(g_respond.target, g_request.target);
        strcpy(g_respond.channel, g_request.channel);
        strcpy(g_respond.payload, g_request.payload);
        sprintf(disp_req.message2, "SET %s OK", g_request.target);
        break;
        
    case CLIENT_ERR_COMMAND:
        strcpy(g_respond.command, "ERROR");
        strcpy(g_respond.target,  "COMMAND");
        strcpy(g_respond.channel, "*");
        strcpy(g_respond.payload, g_request.command);   //"*");
        sprintf(disp_req.message2, "Error Command");
        break;
        
    case CLIENT_ERR_TARGET:
        strcpy(g_respond.command, "ERROR");
        strcpy(g_respond.target,  "TARGET");
        strcpy(g_respond.channel, "*");
        strcpy(g_respond.payload, g_request.target);    //"*");
        sprintf(disp_req.message2, "Error Target");
        break;
        
    case CLIENT_ERR_CHANNEL:
        strcpy(g_respond.command, "ERROR");
        strcpy(g_respond.target,  "CHANNEL");
        strcpy(g_respond.channel, "*");
        strcpy(g_respond.payload, g_request.channel);   //"*");
        sprintf(disp_req.message2, "ERROR CHANNEL");
        break;
        
    case CLIENT_ERR_PAYLOAD:
        strcpy(g_respond.command, "ERROR");
        strcpy(g_respond.target,  "PAYLOAD");
        strcpy(g_respond.channel, "*");
        strcpy(g_respond.payload, g_request.payload);   //"*");
        sprintf(disp_req.message2, "ERROR PAYLOAD");
        break;
        
    case CLIENT_ERR_UNKNOWN:
    default:
        strcpy(g_respond.command, "ERROR");
        strcpy(g_respond.target,  "UNKNOWN");
        strcpy(g_respond.channel, "*");
        strcpy(g_respond.payload, "*");
        sprintf(disp_req.message2, "ERROR UNKNOWN");
        break;
    }
    
    /* Send respond message to client */
    g_respond.client_id = g_request.client_id;
    xQueueSend(respond_queue, &g_respond, 0 ); 
    
    /* Send message to lcd display task */
    disp_req.source = DISP_FROM_CLIENT;
    disp_req.show_line1 = true;
    disp_req.show_line2 = true;      
    sprintf(disp_req.message1, "DBG: TCP Client");
    xQueueSend(display_queue, &disp_req, 0 ); 
}


/*******************************************************************************/
void Client_GetFirmwareVersion(void)
{
    strcpy(g_request.payload, FW_VERSION);
    strcpy(g_request.channel, "*");
    Client_RespondHandler( CLIENT_GET_OK );
}

/*******************************************************************************/
void Client_GetMacAddress(void)
{
//    strcpy(g_request.payload, HW_VERSION);
//    strcpy(g_request.channel, "*");
    Client_RespondHandler( CLIENT_GET_OK );
}

/*******************************************************************************/
void Client_GetSerialNumber(void)
{
    strcpy(g_request.payload, SN_CODE);
    strcpy(g_request.channel, "*");
    Client_RespondHandler( CLIENT_GET_OK );
}

/*******************************************************************************/
void Client_GetSSID(void)
{
    strcpy(g_request.payload, WIFI_SSID);
    strcpy(g_request.channel, "*");
    Client_RespondHandler( CLIENT_GET_OK );
}

/*******************************************************************************/
void Client_GetPassword(void)
{
    strcpy(g_request.payload, WIFI_PASSWORD);
    strcpy(g_request.channel, "*");
    Client_RespondHandler( CLIENT_GET_OK );
}

/*******************************************************************************/
void Client_GetMotorFrequency(void)
{
    uint32_t freq = 0;
        
    if(((g_request.channel[0] >= '1') && (g_request.channel[0] <= '5')) && (g_request.channel[1] == '\0'))
    {
        freq = Motor_GetFrequency(g_request.channel[0]);
        
        memset(g_request.payload, 0, CLIENT_PAYLOAD_SIZE);
        sprintf(g_request.payload, "%d", freq);
        Client_RespondHandler( CLIENT_GET_OK );
    }
    else
    {
        Client_RespondHandler( CLIENT_ERR_CHANNEL );
    }
}

/*******************************************************************************/
void Client_GetMotorStep(void)
{
    uint32_t step = 0;
        
    if(((g_request.channel[0] >= '1') && (g_request.channel[0] <= '5')) && (g_request.channel[1] == '\0'))
    {
        step = Motor_GetStep(g_request.channel[0]);
        
        memset(g_request.payload, 0, CLIENT_PAYLOAD_SIZE);
        sprintf(g_request.payload, "%d", step);
        Client_RespondHandler( CLIENT_GET_OK );
    }
    else
    {
        Client_RespondHandler( CLIENT_ERR_CHANNEL );
    }
}

/*******************************************************************************/
void Client_GetMotorDirection(void)
{
    uint8_t direction = 0;
        
    if(((g_request.channel[0] >= '1') && (g_request.channel[0] <= '5')) && (g_request.channel[1] == '\0'))
    {
        direction = Motor_GetDirection(g_request.channel[0]);
        
        memset(g_request.payload, 0, CLIENT_PAYLOAD_SIZE);
        g_request.payload[0] = direction;
        Client_RespondHandler( CLIENT_GET_OK );
    }
    else
    {
        Client_RespondHandler( CLIENT_ERR_CHANNEL );
    }
}

/*******************************************************************************/
void Client_GetCameraImage(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Get Camera Image\r\n");
    Client_RespondHandler( CLIENT_GET_OK );
    xEventGroupSetBits( camera_event_group, CAMERA_EVENT_PHOTO_START);
}


/*******************************************************************************/
void Client_SetMotorFrequency(void)
{
    uint8_t  i = 0;
    uint32_t freq = 0;
    bool payload_error = false;
    
    memset((uint8_t*)(&g_respond), 0, sizeof(Client_Command_t));
    
    /* transform period string to uint16_t */
    while(g_request.payload[i] != '\0')
    {
        if((g_request.payload[i] >= '0') && (g_request.payload[i] <= '9'))
        {
            freq = freq * 10 + (g_request.payload[i] - '0');
            i++;
        }
        else
        {
            payload_error = true;
            break;
        }
    }    
    
    if(((g_request.channel[0] >= '1') && (g_request.channel[0] <= '5')) && (g_request.channel[1] == '\0'))
    {
        if(payload_error == false)
        {
            Motor_SetFrequency(g_request.channel[0], freq);
            Client_RespondHandler( CLIENT_SET_OK );
        }
        else
        {
            Client_RespondHandler( CLIENT_ERR_PAYLOAD );
        }
    }
    else
    {
        Client_RespondHandler( CLIENT_ERR_CHANNEL );
    }
}

/*******************************************************************************/
void Client_SetMotorStep(void)
{
    uint8_t  i = 0;
    uint16_t step = 0;
    bool payload_error = false;

    memset((uint8_t*)(&g_respond), 0, sizeof(Client_Command_t));
    
    /* transform step string to uint16_t */
    while(g_request.payload[i] != '\0')
    {
        if((g_request.payload[i] >= '0') && (g_request.payload[i] <= '9'))
        {
            step = step * 10 + (g_request.payload[i] - '0');
            i++;
        }
        else
        {
            payload_error = true;
            break;
        }
    }    
    
    if(((g_request.channel[0] >= '1') && (g_request.channel[0] <= '5')) && (g_request.channel[1] == '\0'))
    {
        if(payload_error == false)
        {
            Motor_SetStep(g_request.channel[0], step);
            Client_RespondHandler( CLIENT_SET_OK );
        }
        else
        {
            Client_RespondHandler( CLIENT_ERR_PAYLOAD );
        }
        
    }
    else
    {
        Client_RespondHandler( CLIENT_ERR_CHANNEL );
    }
}

/*******************************************************************************/
void Client_SetMotorDirection(void)
{
    memset((uint8_t*)(&g_respond), 0, sizeof(Client_Command_t));
    
    /* validate payload */
    if((g_request.payload[0] == 'L') || (g_request.payload[0] == 'R'))
    {
        if(((g_request.channel[0] >= '1') && (g_request.channel[0] <= '5')) && (g_request.channel[1] == '\0'))
        {
            Motor_SetDirection(g_request.channel[0], g_request.payload[0]);
            Client_RespondHandler( CLIENT_SET_OK );
        }
        else
        {
            Client_RespondHandler( CLIENT_ERR_CHANNEL );
        }
    }
    else
    {
        Client_RespondHandler( CLIENT_ERR_PAYLOAD );
    }
}

#endif /* USE_DEMO_VERSION */


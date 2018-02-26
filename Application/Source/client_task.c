/*
***************************************************************************************************
*                            Client Communication Task
*
* File   : client_task.c
* Author : Douglas Xie
* Date   : 2018.02.04
***************************************************************************************************
* Copyright (C) 2017-2018 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifndef USE_DEMO_VERSION

/* Include Head Files ---------------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "string.h"
#include "util.h"
#include "global_config.h"
#include "memory.h"
#include "client_task.h"
#include "wifi_task.h"
#include "motor_task.h"
#include "display_task.h"
#include "camera_task.h"
#include "debug_task.h"

/* Global Variable ------------------------------------------------------------------------------*/
Client_Message_t message;           /* client message struct */
Client_Message_t feedback;         

QueueHandle_t request_queue;
QueueHandle_t respond_queue;

/* Private Variable -----------------------------------------------------------------------------*/
Client_Ota_t ota_info;

/* Function Declaration -------------------------------------------------------------------------*/
void Client_GetMacAddress(void);
void Client_GetCameraImage(void);
void Client_GetState(void);
void Client_GetFirmwareVersion(void);
void Client_SetWebAccount(void);
void Client_SetWifi(void);
void Client_SetMotor(void);
void Client_SetTime(void);
void Client_SetSchedule(void);
void Client_PushImage(void);
void Client_PushWebAccount(void);
void Client_PushAlarm(void);
void Client_OtaUpdateRequest(void);
void Client_OtaBinData(void);
void Client_OtaVerify(void);
void Client_FactoryNew(void);

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
        if( xQueueReceive(request_queue, &message, (TickType_t) CLIENT_QUEUE_TIMEUOT))
        {
            /* Process request */
            Client_RequestHandler();
        }
    }
}

/*******************************************************************************
* @Brief   Receive Data Analyzer
* @Param   data[in]: receive data from app
* @Note    analyze data and fill message struct
* @Return  true: analyze success
*          false: analyze error
*******************************************************************************/
bool Client_DataAnalyzer(uint8_t *data, Client_Message_t *msg)
{
    bool ret = false;
    uint16_t i = 0;
    uint16_t temp = 0;
    uint8_t checksum = 0;
    
    memset(msg, 0, sizeof(Client_Message_t));
    
    /* Find start code */
    for(i = 0; i < MSG_BUFFER_SIZE; i++)
    {
        (data[i] == MSG_START_CODE) ? (temp++) : (temp = 0);
        
        /* Find start code success */  
        if(temp >= MSG_RECOGNIZE_CODE_LEN)
        {
            i += 1;
            break;
        }
    }
        
    if(i < MSG_BUFFER_SIZE)
    {
        /* Load data */
        msg->command = data[i];
        msg->index = data[i+1] + (data[i+2] << 8);
        msg->length = data[i+3] + (data[i+4] << 8);
        //msg->payload = &data[i+5];
        msg->checksum = data[i+5+msg->length];
        
        /* Validate checksum */
        checksum = Mem_GetChecksum8(0, &data[i], msg->length + 5);
        if(checksum == msg->checksum) 
        {
            if(msg->length == 0)
            {
                msg->payload = NULL;
                ret = true;
            }
            else if((msg->length > 0) && (msg->length <= MSG_MAX_RX_PAYLOAD))
            {
                msg->payload = (uint8_t *)pvPortMalloc(msg->length);
                memcpy(msg->payload, &data[i+5], msg->length);
                ret = true;
            }
        }
    }
        
    if(ret == false)
    {
        msg->command = MSG_FB_ERROR;
        msg->payload = NULL;
    }
    
    return ret;
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
    memset(&feedback, 0, sizeof(Client_Message_t));

    switch(message.command)
    {
    case MSG_GET_MAC:       //------------------------- Get command
        Client_GetMacAddress();
        break;      
    case MSG_GET_IMAGE:
        Client_GetCameraImage();
        break;        
    case MSG_GET_STATE:
        Client_GetState();
        break;
    case MSG_GET_VERSION:
        Client_GetFirmwareVersion();
        break;
        
    case MSG_SET_ACCOUNT:   //------------------------- Set command
        Client_SetWebAccount();
        break;
    case MSG_SET_WIFI:
        Client_SetWifi();
        break;
    case MSG_SET_MOTOR:
        Client_SetMotor();
        break;
    case MSG_SET_TIME:
        Client_SetTime();
        break;
    case MSG_SET_SCH:
        Client_SetSchedule();
        break;
        
    case MSG_PUSH_IMAGE:    //------------------------- Push command
        Client_PushImage();
        break;
    case MSG_PUSH_ACCOUNT: 
        Client_PushWebAccount();
        break;
    case MSG_PUSH_ALARM:
        Client_PushAlarm();
        break;
        
    case MSG_OTA_REQUEST:   //------------------------- OTA command
        Client_OtaUpdateRequest();
        break;
    case MSG_OTA_BIN:
        Client_OtaBinData();
        break;
    case MSG_OTA_VERIFY:
        Client_OtaVerify();
        break;
        
    case MSG_FACTORY_NEW:   //------------------------- Factory New command
        Client_FactoryNew();
        break;
        
    default:
        Client_RespondHandler( MSG_FB_ERROR );
        break;
    }
    
    /* Free payload memory */
    vPortFree(message.payload);
}

/*******************************************************************************
* @Brief   Command Respond Handler
* @Param   state[in]: respond state
* @Note    build repond message and send back to client
* @Return  
*******************************************************************************/
void Client_RespondHandler(uint8_t state)
{                
    Disp_Request_t disp_req;
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Respond Post\r\n");
        
    if(state == MSG_FB_OK)
    {
        sprintf(disp_req.message2, "CMD:%02X OK", message.command);
    }
    else  if(state == MSG_FB_UPDATED)
    {
        sprintf(disp_req.message2, "CMD:%02X Updated", message.command);
    }
    else  /* if(state == MSG_FB_ERROR) */
    {
        vPortFree(feedback.payload);
        feedback.index = 0;
        feedback.length = 0;
        feedback.payload = NULL;
        sprintf(disp_req.message2, "CMD:%02X Error", message.command);
    }
        
    /* Send respond message to client */
    feedback.client_id = message.client_id;
    feedback.command = state;
    /* index, length, payload fill by handler */
    feedback.checksum = Mem_GetChecksum8(0, (uint8_t *)&feedback.command, 5);
    if(feedback.payload != NULL)
    {
        feedback.checksum = Mem_GetChecksum8(feedback.checksum, feedback.payload, feedback.length);
    }
    xQueueSend(respond_queue, &feedback, 0 ); 
    
    /* Send message to lcd display task */
    disp_req.source = DISP_DBG_CLIENT;
    disp_req.show_line1 = true;
    disp_req.show_line2 = true;      
    sprintf(disp_req.message1, "DBG: TCP");
    xQueueSend(display_queue, &disp_req, 0 ); 
}

/*******************************************************************************/
void Client_GetMacAddress(void)
{
    uint8_t i;
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Get MAC\r\n");

    vPortFree(feedback.payload);
    feedback.index = 0;
    feedback.length = 18;
    feedback.payload = (uint8_t *)pvPortMalloc(18);
    for(i = 0; i < 18; i++)
    {
        feedback.payload[i] = wifi_mac_string[i];
    }
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_GetCameraImage(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Get Camera Image\r\n");
    Client_RespondHandler( MSG_FB_OK );
    xEventGroupSetBits( camera_event_group, CAMERA_EVENT_PHOTO_START);
}

/*******************************************************************************/
void Client_GetState(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Get State\r\n");
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_GetFirmwareVersion(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Get Version\r\n");
    feedback.index = 0;
    feedback.length = 2;
    feedback.payload = (uint8_t *)pvPortMalloc(2);
    feedback.payload[0] = (uint8_t)(APP_VERSION & 0xFF);
    feedback.payload[1] = (uint8_t)((APP_VERSION >> 8) & 0xFF);
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_SetWebAccount(void)
{
    uint16_t i = 0;
    uint8_t server_len = 0;
    uint8_t port_len = 0;
    uint8_t id_len = 0;
    uint8_t passwd_len = 0;
    
    server_len = message.payload[0];
    port_len = message.payload[1];
    id_len = message.payload[2];
    passwd_len = message.payload[3];
    if((message.length == (4 + server_len + port_len + id_len + passwd_len)) && 
        (server_len <= 64) && (port_len <= 2) && (id_len <= 32) && (passwd_len <= 32))
    {       
        memset(app_config.cloud_server, 0, 64);
        app_config.cloud_port = 0;
        memset(app_config.account_id, 0, 32);
        memset(app_config.account_passwd, 0, 32);
        
        for(i = 0; i < server_len; i++)
        {
            app_config.cloud_server[i] = message.payload[i + 4];
        }
        app_config.cloud_port = message.payload[server_len + 4] + (message.payload[server_len + 5] << 8);
        for(i = 0; i < id_len; i++)
        {
            app_config.account_id[i] = message.payload[i + server_len + port_len + 4];
        }
        for(i = 0; i < passwd_len; i++)
        {
            app_config.account_passwd[i] = message.payload[i + server_len + port_len + id_len + 4];
        }
        Mem_WriteConfig();
        DBG_SendMessage(DBG_MSG_CLIENT, "Client: Set Account OK\r\n");
        Client_RespondHandler( MSG_FB_OK );
    }
    else
    {
        DBG_SendMessage(DBG_MSG_CLIENT, "Client: Set Account Error\r\n");
        Client_RespondHandler( MSG_FB_ERROR );
    }
}

/*******************************************************************************/
void Client_SetWifi(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Set WiFi\r\n");
    
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_SetMotor(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Set Motor\r\n");
//  Motor_SetFrequency(msg.channel[0], freq);
//  Motor_SetStep(msg.channel[0], step);  
//  Motor_SetDirection(msg.channel[0], msg.payload[0]);
    
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_SetTime(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Set RTC\r\n");
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_SetSchedule(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Set Feed Schedule\r\n");
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_PushImage(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Push Image\r\n");
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_PushWebAccount(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Push Web Account\r\n");
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_PushAlarm(void)
{
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Push Alarm\r\n");
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_OtaUpdateRequest(void)
{
    uint16_t fw_ver = 0;
    memset(&ota_info, 0, sizeof(ota_info));
    vPortFree(feedback.payload);
    memset(&feedback, 0, sizeof(feedback));
    
    if((message.length == 2) && (message.payload != NULL))
    {
        fw_ver = message.payload[0] + (message.payload[1] << 8);
        if(fw_ver > APP_VERSION)
        {
            ota_info.fw_version = fw_ver;
            app_info.ota_version = 0;
            app_info.ota_length  = 0;
            app_info.ota_crc = 0;
            Mem_WriteInfo();

            /* erase ota flash */
            Mem_EraseApp(OTA_ADDR_START, OTA_ADDR_END);
                         
            Client_RespondHandler( MSG_FB_OK );
            DBG_SendMessage(DBG_MSG_CLIENT, "Client: OTA Request - OK\r\n");
        }
        else
        {
            Client_RespondHandler( MSG_FB_UPDATED );
            DBG_SendMessage(DBG_MSG_CLIENT, "Client: OTA Request - Updated\r\n");
        }
    }
    else
    {
        Client_RespondHandler( MSG_FB_ERROR );
        DBG_SendMessage(DBG_MSG_CLIENT, "Client: OTA Request Error\r\n");
    }
}

/*******************************************************************************/
void Client_OtaBinData(void)
{
    DBG_MsgBuf_t dbg;
        
    DBG_Sprintf(dbg.buf, "Client: OTA Bin Packet %d\r\n", message.index);    
    DBG_SendMessage(DBG_MSG_CLIENT, dbg.buf);

    /* packet 0 include firmware version, crc and size */
    if(message.index == 0)
    {
        /* extract firmware information */
        ota_info.fw_version = message.payload[0] + (message.payload[1] << 8);
        ota_info.fw_crc16 = message.payload[2] + (message.payload[3] << 8);
        ota_info.fw_size = message.payload[4] + (message.payload[5] << 8) + (message.payload[6] << 16) + (message.payload[7] << 24);
        ota_info.write_length = message.length - 8;
        
        /* write bin data */
        Mem_WriteApp(OTA_ADDR_START, (uint8_t *)&message.payload[8], message.length - 8);       
        
    }
    else
    {
        Mem_WriteApp(OTA_ADDR_START + ota_info.write_length, (uint8_t *)message.payload, message.length);        
        ota_info.write_length += message.length;
    }
    
    Client_RespondHandler( MSG_FB_OK );
}

/*******************************************************************************/
void Client_OtaVerify(void)
{
    bool ota_success = false;
    uint16_t crc = 0;
    
    /* check firmware size */
    if(ota_info.write_length == ota_info.fw_size)
    {
        /* check crc */
        crc = CRC16_CCITT((uint8_t *)OTA_ADDR_START, ota_info.fw_size);
        if(crc == ota_info.fw_crc16)
        {
            /* ota success, update app info and reboot */
            ota_success = true;
        }
    }
    
    if(ota_success == true)
    {
        DBG_SendMessage(DBG_MSG_CLIENT, "Client: OTA Verify Success\r\n");
        app_info.ota_version = ota_info.fw_version;
        app_info.ota_length = ota_info.fw_size;
        app_info.ota_crc = ota_info.fw_crc16;
        Mem_WriteInfo();
        Client_RespondHandler( MSG_FB_OK );
        
        /* delay 1 second and reboot to excute new app */
        DBG_SendMessage(DBG_MSG_CLIENT, "=== Device Reset After 3s ===\r\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        HAL_NVIC_SystemReset();
    }
    else
    {
        DBG_SendMessage(DBG_MSG_CLIENT, "Client: OTA Verify Failed\r\n");
        Client_RespondHandler( MSG_FB_ERROR );
    }
}

void Client_FactoryNew(void)
{
    uint8_t i = 0;
    DBG_SendMessage(DBG_MSG_CLIENT, "Client: Factory New\r\n");
    
    /* reset default value */
    memset(&app_config, 0, sizeof(app_config));   
    /* write default motor config */
    for(i = 0; i < 5; i++)
    {
        app_config.motor_cfg.m_dir[i] = MOTOR_DEFAULT_DIR;
        app_config.motor_cfg.m_freq[i] = MOTOR_DEFAULT_FREQ;
        app_config.motor_cfg.m_step[i] = MOTOR_DEFAULT_STEP;
    }
    Mem_WriteConfig();
    Client_RespondHandler( MSG_FB_OK );
    
    /* delay 1 second and reboot */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    HAL_NVIC_SystemReset();
}


#endif /* USE_DEMO_VERSION */



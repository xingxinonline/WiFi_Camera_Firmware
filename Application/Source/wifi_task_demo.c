/*
***************************************************************************************************
*                               WiFi Module Control Task
*       this is demo version which use demo protocol and PC software TCP_Client V1.00.exe
*
* File   : wifi_task.c
* Author : Douglas Xie
* Date   : 2017.10.09
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifdef USE_DEMO_VERSION

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


#define APP_TEST_FORMAT

/* Private variables ----------------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern QueueHandle_t request_queue;
extern QueueHandle_t respond_queue;

/* WiFi output buffer */
uint8_t tx_buffer[WIFI_TX_BUF_SIZE];
bool wifi_uart_tx_done = false;

/* WiFi input buffer */
uint8_t  rx_buffer[WIFI_RX_BUF_SIZE];
uint16_t rx_index = 0;
uint16_t line_start = 0;

/* Client input data buffer */
uint8_t  client_data[WIFI_DATA_BUF_SIZE];
uint16_t client_data_index = 0;
uint16_t client_data_size = 0;
uint8_t  client_id_active = 0xFF;
bool push_event = false;

/* WiFi mac and ip address */
uint8_t wifi_mac_string[18];    /* string format: AA:BB:CC:DD:EE:FF\0 */
uint8_t wifi_ip_string[16];     /* string format: 192.168.100.123\0 */

/* Client management */
uint8_t client_list[5] = {0,0,0,0,0};

/* WiFi state variable */
WiFi_RxState_t wifi_rx_state     = WIFI_RX_NONE;
WiFi_CtrlState_t wifi_ctrl_state = WIFI_CTRL_ECHO;

/* WiFi queue */
QueueHandle_t receive_queue;
QueueHandle_t wifi_tx_queue;

/* WiFi debug message */
DBG_MsgBuf_t wifi_message;

int8_t start_code[5] = {123,123,123,123,123};
int8_t end_code[5] = {-88,-88,-88,-88,-88};
uint8_t packet_buf[WIFI_PACKET_SIZE+13];
uint8_t packet_id = 0;

/* Function declaration -------------------------------------------------------------------------*/
/* Control task branch handler */
bool WiFi_Ctrl_Echo(void);
bool WiFi_Ctrl_SetupUART(void);
bool WiFi_Ctrl_SetupAP(void);
bool WiFi_Ctrl_GetMac(void);
bool WiFi_Ctrl_StartServer(void);
bool WiFi_Ctrl_ClientManage(void);
bool WiFi_Ctrl_ReceRequest(void);
bool WiFi_Ctrl_SendImage(void);
bool WiFi_Ctrl_SendImageStart(void);
bool WiFi_Ctrl_SendImageEnd(void);
bool WiFi_Ctrl_SendRespond(void);
WiFi_CtrlState_t WiFi_Ctrl_Idle(void);

/* UART function */
void WiFi_SetUartBaudrate(uint32_t baudrate);
bool WiFi_SendCommand(uint8_t *cmd);
bool WiFi_SendData(uint8_t *data, uint16_t length);
void WiFi_ResetRxBuffer(void);

/* Task Function implement ----------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   WiFi Control Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void WiFi_ControlTask(void * argument)
{
    uint8_t error_counter = 0;
    uint16_t idle_counter = 0;
    
    /* Power on delay for WiFi module initial */
    vTaskDelay(WIFI_RESET_DELAY);
    
    /* Create wifi queue and register queue for debug */
    if( receive_queue == NULL )
    {
        receive_queue = xQueueCreate(WIFI_QUEUE_LENGTH, WIFI_QUEUE_ITEM_SIZE);
        vQueueAddToRegistry( receive_queue, "WiFi Queue" );
    }
    
    WiFi_ResetRxBuffer();
    
    DBG_SendMessage(DBG_MSG_TASK_STATE, "WiFi Task Start\r\n");
    
    /* Infinite loop */
    for(;;)
    {    
        switch(wifi_ctrl_state)
        {
        case WIFI_CTRL_ECHO:
            /* Test echo function to verify baudrate */
            DBG_SendMessage(DBG_MSG_WIFI_CTRL, "WiFi: Test Echo\r\n");
            if(WiFi_Ctrl_Echo() == true)
            {
                wifi_ctrl_state = WIFI_CTRL_START_SERVER;
            }
            else
            {
                wifi_ctrl_state = WIFI_CTRL_SETUP_UART;
            }
            break;
            
        case WIFI_CTRL_SETUP_UART:
            /* Setup wifi module to high speed baudrate */
            DBG_SendMessage(DBG_MSG_WIFI_CTRL, "WiFi: Setup WiFi UART\r\n");
            WiFi_Ctrl_SetupUART();
            wifi_ctrl_state = WIFI_CTRL_GET_MAC;
            break;
                        
        case WIFI_CTRL_GET_MAC:
            /* Get ESP8266 station mac address */
            DBG_SendMessage(DBG_MSG_WIFI_CTRL, "WiFi: Get Station MAC\r\n");
            if(WiFi_Ctrl_GetMac() == true)
            {
                wifi_ctrl_state = WIFI_CTRL_SETUP_AP;
            }
            else
            {
                wifi_ctrl_state = WIFI_CTRL_SETUP_UART;
            }
            break;
            
        case WIFI_CTRL_SETUP_AP:
            /* Setup wifi AP mode */
            DBG_SendMessage(DBG_MSG_WIFI_CTRL, "WiFi: Setup WiFi AP\r\n");
            if(WiFi_Ctrl_SetupAP() == true)
            {
                wifi_ctrl_state = WIFI_CTRL_ECHO;
            }
            else
            {
                wifi_ctrl_state = WIFI_CTRL_SETUP_UART;
            }
            break;
            
        case WIFI_CTRL_START_SERVER:
            /* Start TCP server */
            DBG_SendMessage(DBG_MSG_WIFI_CTRL, "WiFi: Start TCP Server\r\n");
            if(WiFi_Ctrl_StartServer() == true)
            {
                wifi_ctrl_state = WIFI_CTRL_IDLE;
            }
            else
            {
                wifi_ctrl_state = WIFI_CTRL_SETUP_AP;
            }
            break;
            
        case WIFI_CTRL_CLIENT_MANAGE:
            /* Manage client connect or closed */
            DBG_SendMessage(DBG_MSG_WIFI_CTRL, "WiFi: Client List Update\r\n");
            if(WiFi_Ctrl_ClientManage() == true)
            {
                wifi_ctrl_state = WIFI_CTRL_IDLE;
            }
            break;
            
        case WIFI_CTRL_RECE_REQUEST:
            /* Receive request from client */
            if(WiFi_Ctrl_ReceRequest() == true)
            {
                wifi_ctrl_state = WIFI_CTRL_IDLE;
                error_counter = 0;
            }
            else
            {
                error_counter++;
                if(error_counter >= WIFI_ERROR_REPEAT)
                {
                    wifi_ctrl_state = WIFI_CTRL_IDLE;
                }
            }
            break;
            
        case WIFI_CTRL_SEND_RESPOND:
            /* Send respond to client */
            if(WiFi_Ctrl_SendRespond() == true)
            {
                wifi_ctrl_state = WIFI_CTRL_IDLE;
                error_counter = 0;
            }
            else
            {
                error_counter++;
                if(error_counter >= WIFI_ERROR_REPEAT)
                {
                    wifi_ctrl_state = WIFI_CTRL_IDLE;
                }
            }
            break;
            
        case WIFI_CTRL_SEND_IMAGE:
            /* Send data to client */
#ifdef APP_TEST_FORMAT
            WiFi_Ctrl_SendImage();
#else
            WiFi_Ctrl_SendImageStart();
            WiFi_Ctrl_SendImage();
            WiFi_Ctrl_SendImageEnd();     
#endif
            wifi_ctrl_state = WIFI_CTRL_IDLE;
            break;
            
        case WIFI_CTRL_IDLE:
            /* Idle and waiting for event */
            wifi_ctrl_state = WiFi_Ctrl_Idle();
            if(wifi_ctrl_state == WIFI_CTRL_IDLE)
            {
                idle_counter++;
                if(idle_counter >= 500)
                {
                    idle_counter = 0;
                    wifi_ctrl_state = WIFI_CTRL_ALIVE_TEST;
                }
            }
            else
            {
                idle_counter = 0;
            }
            break;
            
        case WIFI_CTRL_ALIVE_TEST:
            if(WiFi_Ctrl_Echo() == true)
            {
                wifi_ctrl_state = WIFI_CTRL_IDLE;
            }
            else
            {                      
                DBG_SendMessage(DBG_MSG_WIFI_CTRL, "WiFi: Alive Test\r\n");
                wifi_ctrl_state = WIFI_CTRL_SETUP_AP;
            }
            break;
            
        default:
            break;
        }
        
        vTaskDelay(WIFI_TASK_PERIOD);
    }
}

bool WiFi_Ctrl_Echo(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    
    /* Enable echo feedback */
    sprintf((char*)tx_buffer, "ATE1\r\n");
    WiFi_SendCommand(tx_buffer);  
    
    /* Receive rx_state until get result state or timeout */
    if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
    {
        if(receive.rx_state == WIFI_RX_ATFB_OK)
        {
            //DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Echo OK\r\n");
            rtn_state = true;
        }
        else
        {
            //DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Echo Failed\r\n");
            rtn_state = false;
        }
    }
    
    return rtn_state;
}

bool WiFi_Ctrl_SetupUART(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    
    /*--------------- Set WiFi UART Baudrate -----------------*/
    /* Set mcu wifi uart to default baudrate */
    WiFi_SetUartBaudrate(WIFI_BAUDRATE_DEFAULT);
    
    /* Set wifi module uart baudrate to high speed mode */
    sprintf((char*)tx_buffer, "AT+CIOBAUD=%d\r\n", WIFI_BAUDRATE_RUNNING);
    WiFi_SendCommand(tx_buffer);
    
    /* Receive rx_state until get result state or timeout */
    if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
    {
        if(receive.rx_state == WIFI_RX_ATFB_OK)
        {
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Setup Baudrate OK\r\n");
            rtn_state = true;
        }
        else
        {        
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Setup Baudrate Failed\r\n");
            rtn_state = false;
        }
    }
    
    /* Set mcu wifi uart to running baudrate */
    WiFi_SetUartBaudrate(WIFI_BAUDRATE_RUNNING);
    
    return rtn_state;
}

bool WiFi_Ctrl_SetupAP(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    
    /*-------------- Setup WiFi Transmit Mode -----------------*/
    /* Setup wifi module to softAP+station mode */
    sprintf((char*)tx_buffer, "AT+CWMODE=3\r\n");
    WiFi_SendCommand(tx_buffer);
    
    /* Receive rx_state until get result state or timeout */
    if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
    {
        if(receive.rx_state == WIFI_RX_ATFB_OK)
        {
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Setup WiFi Mode OK\r\n");
            rtn_state = true;
        }
        else
        {        
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Setup WiFi Mode Failed\r\n");
            rtn_state = false;
        }
    }
    
    /*-------------- Setup WiFi AP Mode Parameter -----------------*/
    if(rtn_state == true)
    {    
        /* Setup AP mode */
        sprintf((char*)tx_buffer, "AT+CWSAP=\"%s%c%c%c%c%c%c\",\"%s\",%d,%d\r\n", 
                WIFI_SSID, wifi_mac_string[9], wifi_mac_string[10], wifi_mac_string[12], wifi_mac_string[13], wifi_mac_string[15], wifi_mac_string[16], 
                WIFI_PASSWORD, WIFI_CHANNEL, (uint8_t)WIFI_ENCRYPT);
        WiFi_SendCommand(tx_buffer);
        
        /* Receive rx_state until get result state or timeout */
        if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
        {
            if(receive.rx_state == WIFI_RX_ATFB_OK)
            {
                DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Setup WiFi AP OK\r\n");
                rtn_state = true;
            }
            else
            {        
                DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Setup WiFi AP Failed\r\n");
                rtn_state = false;
            }
        }
    }
    
    /*-------------- Reset WiFi Module -----------------*/
    if(rtn_state == true)
    {    
        sprintf((char*)tx_buffer, "AT+RST\r\n");
        WiFi_SendCommand(tx_buffer);
        
        /* Receive rx_state until get result state or timeout */
        if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
        {
            if(receive.rx_state == WIFI_RX_ATFB_OK)
            {
                vTaskDelay(WIFI_RESET_DELAY);
                DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Reset WiFi OK\r\n");
                rtn_state = true;
            }
            else
            {        
                DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Reset WiFi Failed\r\n");
                rtn_state = false;
            }
        }
    }
    
    WiFi_ResetRxBuffer();
    return rtn_state;
}

bool WiFi_Ctrl_GetMac(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    
    /*-------------- Enable Multi-Client Connect -----------------*/
    sprintf((char*)tx_buffer, "AT+CIPSTAMAC?\r\n");
    WiFi_SendCommand(tx_buffer);  
    
    /* Receive rx_state until get result state or timeout */
    if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
    {
        if(receive.rx_state == WIFI_RX_ATFB_OK)
        {
            for(int i = 0; i < 18; i++)
            {
                if((wifi_mac_string[i] >= 'a') && (wifi_mac_string[i] <= 'z'))
                {
                    wifi_mac_string[i] -= 'a' - 'A';
                }
            }
            
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Get MAC OK\r\n");
            rtn_state = true;
        }
        else
        {        
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Get MAC Failed\r\n");
            rtn_state = false;
        }
    }
    
    WiFi_ResetRxBuffer();
    return rtn_state;
}

bool WiFi_Ctrl_StartServer(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    
    /*-------------- Enable Multi-Client Connect -----------------*/
    sprintf((char*)tx_buffer, "AT+CIPMUX=1\r\n");
    WiFi_SendCommand(tx_buffer);  
    
    /* Receive rx_state until get result state or timeout */
    if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
    {
        if(receive.rx_state == WIFI_RX_ATFB_OK)
        {
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Enable Multi-Client OK\r\n");
            rtn_state = true;
        }
        else
        {        
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Enable Multi-Client Failed\r\n");
            rtn_state = false;
        }
    }
    
    /*-------------- Create TCP Server -----------------*/
    if(rtn_state == true)
    {
        /* Set wifi module uart baudrate to high speed mode */
        sprintf((char*)tx_buffer, "AT+CIPSERVER=1,%d\r\n", WIFI_TCP_SERVER_PORT);
        WiFi_SendCommand(tx_buffer);
        
        /* Receive rx_state until get result state or timeout */
        if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
        {
            if(receive.rx_state == WIFI_RX_ATFB_OK)
            {
                DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Start TCP Server OK\r\n");
                rtn_state = true;
            }
            else
            {        
                DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Start TCP Server Failed\r\n");
                rtn_state = false;
            }
        }
    }
    
    /*-------------- Set Server Timeout -----------------*/
    if(rtn_state == true)
    {
        /* Set server timeout time */
        sprintf((char*)tx_buffer, "AT+CIPSTO=%d\r\n", WIFI_TCP_SERVER_TIMEOUT);
        WiFi_SendCommand(tx_buffer);
        
        /* Receive rx_state until get result state or timeout */
        if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
        {
            if(receive.rx_state == WIFI_RX_ATFB_OK)
            {
                DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Set Server Timeout OK\r\n");
                rtn_state = true;
            }
            else
            {        
                DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Set Server Timeout Failed\r\n");
                rtn_state = false;
            }
        }
    }
    
    WiFi_ResetRxBuffer();
    return rtn_state;
}

bool WiFi_Ctrl_ClientManage(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    Disp_Request_t disp_req;
    
    /* Receive rx_state until get result state or timeout */
    if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
    {
        if(receive.rx_state == WIFI_RX_ID_CONNECT)
        {
            DBG_Sprintf(wifi_message.buf, "\tWiFi Rx: Client %d Connect\r\n", receive.client_id);
            DBG_SendMessage(DBG_MSG_WIFI_RX, wifi_message.buf);
            
            sprintf(disp_req.message2, "Client%d-Connect", receive.client_id);
        }
        else
        {        
            DBG_Sprintf(wifi_message.buf, "\tWiFi Rx: Client %d Closed\r\n", receive.client_id);
            DBG_SendMessage(DBG_MSG_WIFI_RX, wifi_message.buf);
            
            sprintf(disp_req.message2, "Client%d-Closed", receive.client_id);
        }
        
        disp_req.source = DISP_FROM_WIFI;
        disp_req.show_line1 = true;
        disp_req.show_line2 = true;      
        sprintf(disp_req.message1, "DBG: WiFi");
        xQueueSend(display_queue, &disp_req, 0 ); 
        rtn_state = true;
    }
    
    WiFi_ResetRxBuffer();
    return rtn_state;
}

bool WiFi_Ctrl_ReceRequest(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    Client_Command_t request;
    
    /* Receive rx_state until get result state or timeout */
    if( xQueueReceive(receive_queue, &receive, (TickType_t) 0))
    {
        if(receive.rx_state == WIFI_RX_IPD_OK)
        {            
            DBG_SendMessage(DBG_MSG_WIFI_RX, "WiFi: Receive Request OK\r\n");
            rtn_state = true;
            
            /* Analyze client request and post to client queue */
            Client_RequestAnalyzer(client_data, &request);
            request.client_id = receive.client_id;
            xQueueSend(request_queue, &request, 0 ); 
            
            WiFi_ResetRxBuffer();
        }
        else
        {        
            rtn_state = false;
        }
    }
    else
    {        
        rtn_state = false;
    }
    
    return rtn_state;
}

bool WiFi_Ctrl_SendRespond(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    Client_Command_t respond;
    uint8_t length = 0;   
    
    DBG_SendMessage(DBG_MSG_WIFI_RX, "WiFi: Sending Respond\r\n");
    
    if(xQueueReceive(respond_queue, &respond, (TickType_t) 10))
    {
        length = strlen(respond.command) + strlen(respond.target) + strlen(respond.channel) + strlen(respond.payload);
        length += 5; /* three split symbol ',' and new line '\r\n' */
        
        //example: AT+CIPSEND=0,14
        sprintf((char*)tx_buffer, "AT+CIPSEND=%d,%d\r\n", respond.client_id, length);
        WiFi_SendCommand(tx_buffer);
        
        if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
        {
            if(receive.rx_state == WIFI_RX_ATFB_OK)
            {
                if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
                {
                    if(receive.rx_state == WIFI_RX_SEND_READY)
                    {
                        rtn_state = true;
                    }
                }
            }
        }
        
        if(rtn_state == true)
        {
            sprintf((char*)tx_buffer, "%s,%s,%s,%s\r\n", respond.command, respond.target, respond.channel, respond.payload);
            WiFi_SendData(tx_buffer, strlen(tx_buffer));
            
            if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
            {
                if(receive.rx_state == WIFI_RX_SEND_OK)
                {
                    rtn_state = true;
                }
                else
                {        
                    rtn_state = false;
                }
            }
        }
        
        if(rtn_state == true)
        {
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Send Respond OK\r\n");
        }
        else
        {
            DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Send Respond Failed\r\n");
        }
    }
    
    return rtn_state;
}

bool WiFi_Ctrl_SendImage(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    uint32_t data_length = 0;
    uint32_t i = 0;   
    
    DBG_SendMessage(DBG_MSG_WIFI_RX, "WiFi: Send Image Data\r\n");   
    
    packet_id = 0;
    data_length = camera_info.fifo_buffer[camera_info.fifo_input].length;
    for(i = 0; i < camera_info.fifo_buffer[camera_info.fifo_input].length; i += WIFI_PACKET_SIZE)
    {
        DBG_SendMessage(DBG_MSG_WIFI_RX, ">");
        
        //example: AT+CIPSEND=0,14
        if(data_length >= WIFI_PACKET_SIZE)
        {
            sprintf((char*)tx_buffer, "AT+CIPSEND=%d,%d\r\n", client_id_active, WIFI_PACKET_SIZE+10);
        }
        else
        {
            sprintf((char*)tx_buffer, "AT+CIPSEND=%d,%d\r\n", client_id_active, data_length+10);
        }
        WiFi_SendCommand(tx_buffer);
        
        if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
        {
            if(receive.rx_state == WIFI_RX_ATFB_OK)
            {
                if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
                {
                    if(receive.rx_state == WIFI_RX_SEND_READY)
                    {
                        rtn_state = true;
                    }
                }
            }
        }
        
        if(rtn_state == true)
        {
            wifi_uart_tx_done = false;
#ifdef APP_TEST_FORMAT
            if(data_length >= WIFI_PACKET_SIZE)
            {
                memcpy(&packet_buf[0], start_code, 5);
                packet_buf[5] = packet_id++;
                packet_buf[6] = WIFI_PACKET_SIZE & 0xFF;
                packet_buf[7] = WIFI_PACKET_SIZE >> 8;
                memcpy(&packet_buf[8], &(camera_info.fifo_buffer[camera_info.fifo_input].data[i]), WIFI_PACKET_SIZE);
                memcpy(&packet_buf[8+WIFI_PACKET_SIZE], end_code, 5);

                WiFi_SendData(packet_buf, WIFI_PACKET_SIZE+13);
                data_length -= WIFI_PACKET_SIZE;
            }
            else
            {
                memcpy(&packet_buf[0], start_code, 5);
                packet_buf[5] = packet_id++;
                packet_buf[6] = data_length & 0xFF;
                packet_buf[7] = data_length >> 8;
                memcpy(&packet_buf[8], &(camera_info.fifo_buffer[camera_info.fifo_input].data[i]), data_length);
                memcpy(&packet_buf[8+data_length], end_code, 5);

                WiFi_SendData(packet_buf, data_length+13);
                data_length = 0;
            }
#else
            if(data_length >= WIFI_PACKET_SIZE)
            {
                WiFi_SendData(&(camera_info.fifo_buffer[camera_info.fifo_input].data[i]), WIFI_PACKET_SIZE);
                data_length -= WIFI_PACKET_SIZE;
            }
            else
            {
                WiFi_SendData(&(camera_info.fifo_buffer[camera_info.fifo_input].data[i]), data_length);
                data_length = 0;
            }
#endif
            while(wifi_uart_tx_done == false)
            {
                vTaskDelay(5);
            }
            
            if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
            {
                if(receive.rx_state == WIFI_RX_SEND_OK)
                {
                    rtn_state = true;
                }
                else
                {        
                    rtn_state = false;
                }
            }
        }
        
        if(rtn_state == true)
        {
            //vTaskDelay(1);
        }
        else
        {
            break;
        }
    }
    
    DBG_SendMessage(DBG_MSG_WIFI_RX, "\r\n");
    if(rtn_state == true)
    {
        DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Send Image Data OK\r\n");
    }
    else
    {
        DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Send Image Data Failed\r\n");
    }
    
    return rtn_state;
}

bool WiFi_Ctrl_SendImageStart(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    uint32_t i = 0;   
    uint8_t str_length[10];
    
    DBG_SendMessage(DBG_MSG_WIFI_RX, "WiFi: Send Image Start\r\n");    
    
    //example: AT+CIPSEND=0,14
    sprintf((char*)str_length, "START:%d", camera_info.fifo_buffer[camera_info.fifo_input].length);
    sprintf((char*)tx_buffer, "AT+CIPSEND=%d,%d\r\n", client_id_active, strlen(str_length));
    WiFi_SendCommand(tx_buffer);
    
    if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
    {
        if(receive.rx_state == WIFI_RX_ATFB_OK)
        {
            if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
            {
                if(receive.rx_state == WIFI_RX_SEND_READY)
                {
                    rtn_state = true;
                }
            }
        }
    }
    
    if(rtn_state == true)
    {
        WiFi_SendData(str_length, strlen(str_length));
        
        if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
        {
            if(receive.rx_state == WIFI_RX_SEND_OK)
            {
                rtn_state = true;
            }
            else
            {        
                rtn_state = false;
            }
        }
    }
    
    vTaskDelay(5);
    if(rtn_state == true)
    {
        DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Send Image Start OK\r\n");
    }
    else
    {
        DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Send Image Start Failed\r\n");
    }
}


bool WiFi_Ctrl_SendImageEnd(void)
{
    bool rtn_state = false;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    uint32_t i = 0;   
    uint8_t str_length[10];
    
    DBG_SendMessage(DBG_MSG_WIFI_RX, "WiFi: Send Image End\r\n");    
    
    //example: AT+CIPSEND=0,14
    sprintf((char*)str_length, "END");
    sprintf((char*)tx_buffer, "AT+CIPSEND=%d,%d\r\n", client_id_active, strlen(str_length));;
    WiFi_SendCommand(tx_buffer);
    
    if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
    {
        if(receive.rx_state == WIFI_RX_ATFB_OK)
        {
            if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
            {
                if(receive.rx_state == WIFI_RX_SEND_READY)
                {
                    rtn_state = true;
                }
            }
        }
    }
    
    if(rtn_state == true)
    {
        WiFi_SendData(str_length, strlen(str_length));
        
        if( xQueueReceive(receive_queue, &receive, (TickType_t) WIFI_RX_FB_TIMEOUT))
        {
            if(receive.rx_state == WIFI_RX_SEND_OK)
            {
                rtn_state = true;
            }
            else
            {        
                rtn_state = false;
            }
        }
    }
    
    vTaskDelay(5);
    if(rtn_state == true)
    {
        DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Send Image End OK\r\n");
    }
    else
    {
        DBG_SendMessage(DBG_MSG_WIFI_RX, "\tWiFi Rx: Send Image End Failed\r\n");
    }
}

WiFi_CtrlState_t WiFi_Ctrl_Idle(void)
{
    EventBits_t event_bits; 
    WiFi_CtrlState_t next_state = WIFI_CTRL_IDLE;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    Client_Command_t respond;
    
    /* Peek a queue item and check it's type (get item but not remove from queue) */
    if(xQueuePeek(receive_queue, &receive, (TickType_t) 0))
    {
        switch(receive.rx_state)
        {
        case WIFI_RX_ID_CONNECT:
        case WIFI_RX_ID_CLOSED:
            next_state = WIFI_CTRL_CLIENT_MANAGE;
            break;
            
        case WIFI_RX_IPD_RECEVING:
        case WIFI_RX_IPD_OK:
            next_state = WIFI_CTRL_RECE_REQUEST;
            break;
            
        case WIFI_RX_OVERFLOW:
        case WIFI_RX_IPD_ERROR:
        default:
            DBG_SendMessage(DBG_MSG_WIFI_RX, "WiFi: Error Data\r\n");
            WiFi_ResetRxBuffer();
            next_state = WIFI_CTRL_IDLE;
            break;
            
        }
    }
    else if(xQueuePeek(respond_queue, &respond, (TickType_t) 0))
    {
        next_state = WIFI_CTRL_SEND_RESPOND;
    }
    else
    {
        if(client_id_active != 0xFF)
        {
            /* Get camera post event */
            event_bits = xEventGroupWaitBits(camera_event_group,
                                             CAMERA_EVENT_PUSH_IMAGE,
                                             pdTRUE,
                                             pdTRUE,
                                             0 );
            
            if(( event_bits & CAMERA_EVENT_PUSH_IMAGE ) == CAMERA_EVENT_PUSH_IMAGE )
            {
                xEventGroupClearBits(camera_event_group, CAMERA_EVENT_PUSH_IMAGE);
                
                next_state = WIFI_CTRL_SEND_IMAGE;
            }
        }
    }
    
    return next_state;
}


/* UART Function implement ----------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Set UART Baudrate 
* @Param   
* @Note    Set uart baudrate that connect to wifi module
* @Return  
*******************************************************************************/
void WiFi_SetUartBaudrate(uint32_t baudrate)
{
    HAL_UART_DeInit(&hwifi_uart);
    
    hwifi_uart.Instance = WIFI_USART;
    hwifi_uart.Init.BaudRate = baudrate;
    hwifi_uart.Init.WordLength = UART_WORDLENGTH_8B;
    hwifi_uart.Init.StopBits = UART_STOPBITS_1;
    hwifi_uart.Init.Parity = UART_PARITY_NONE;
    hwifi_uart.Init.Mode = UART_MODE_TX_RX;
    hwifi_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hwifi_uart.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&hwifi_uart) != HAL_OK)
    {
        Error_Handler();
    }
    
    WiFi_ResetRxBuffer();
}

/*******************************************************************************
* @Brief   Send Command to WiFi Module
* @Param   
* @Note    Send AT commandto wifi module
* @Return  
*******************************************************************************/
bool WiFi_SendCommand(uint8_t *cmd)
{
    WiFi_ResetRxBuffer();
    
    if( HAL_UART_Transmit_DMA(&hwifi_uart, cmd, strlen((const char*)cmd)) == HAL_OK )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
* @Brief   Send Data to WiFi Module
* @Param   
* @Note    Send image data in raw byte format
* @Return  
*******************************************************************************/
bool WiFi_SendData(uint8_t *data, uint16_t length)
{
    if( HAL_UART_Transmit_DMA(&hwifi_uart, data, length) == HAL_OK )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*******************************************************************************
* @Brief   Reset Rx Buffer
* @Param   
* @Note    Reset all rx buffer
* @Return  
*******************************************************************************/
void WiFi_ResetRxBuffer(void)
{
    HAL_UART_AbortReceive(&hwifi_uart);
    
    memset(rx_buffer, 0, WIFI_RX_BUF_SIZE);
    memset(client_data, 0, WIFI_DATA_BUF_SIZE);
    
    rx_index = 0;
    line_start = 0;
    //client_id_active = 0xFF;    
    client_data_size = 0;
    client_data_index = 0;
    
    xQueueReset(receive_queue);
    wifi_rx_state = WIFI_RX_NONE;
    HAL_UART_Receive_DMA(&hwifi_uart, rx_buffer, 1);
}

/*******************************************************************************
* @Brief   UART Receive Complete Callback
* @Param   
* @Note    Update receive index
* @Return  
*******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint16_t i = 0;
    BaseType_t xHigherPriorityTaskWoken;
    WiFi_Receive_t receive = {.client_id = 0xFF, .rx_state = WIFI_RX_NONE};
    
    /* We have not woken a task at the start of the ISR. */
	xHigherPriorityTaskWoken = pdFALSE;
    push_event = false;
    
    if(huart == &hwifi_uart)
    {
        /* one byte received, update the index */
        rx_index++;
        if(rx_index >= WIFI_RX_BUF_SIZE)
        {
            rx_index = 0;
            line_start = 0;
            wifi_rx_state = WIFI_RX_OVERFLOW;
            receive.rx_state = WIFI_RX_OVERFLOW;
            push_event = true;
        }
        
        /* Check packet header type ------------------------------------------*/
        if(rx_index == 1)
        {
            if(rx_buffer[0] == 'A')
            {
                /* A */
                wifi_rx_state = WIFI_RX_ATFB;
            }
            else if((rx_buffer[0] == '+'))
            {
                /* + */
                wifi_rx_state = WIFI_RX_IPD;
                client_data_index = 0;
                client_data_size = 0;
            }
            else if((rx_buffer[0] >= '0') && (rx_buffer[0] <= '4'))
            {
                /* n */
                wifi_rx_state = WIFI_RX_ID;
            }
            else if(rx_buffer[0] == 'R')
            {
                /* R */
                wifi_rx_state = WIFI_RX_RECV;
            }
            else if(rx_buffer[0] == 'E')
            {
                /* E */
                wifi_rx_state = WIFI_RX_ERROR;
            }
            else if(rx_buffer[0] == '>')
            {
                /* > */
                wifi_rx_state = WIFI_RX_SEND_READY;
                receive.rx_state = WIFI_RX_SEND_READY;
                push_event = true;
            }
            else
            {
                wifi_rx_state = WIFI_RX_NONE;
                rx_buffer[0] = 0;
                rx_index = 0;
                line_start = 0;         
            }
        }
        /* Recevive all packet according to header type ----------------------*/
        else
        {
            switch(wifi_rx_state)
            {
            case WIFI_RX_ATFB:
                if(rx_buffer[rx_index - 1] == '\n')
                {
                    /* OK\r\n(4bytes)  --  ERROR\r\n(7bytes) */
                    if(((rx_index - line_start) >= 4) && ((rx_index - line_start) <= 7))
                    {
                        if(memcmp(&(rx_buffer[rx_index - 4]), "OK", 2) == 0)
                            //if((rx_buffer[rx_index - 4] == 'O') && (rx_buffer[rx_index - 3] == 'K'))
                        {
                            if(memcmp(&(rx_buffer[0]), "AT+CIPSTAMAC?", 13) == 0)
                            {
                                memcpy(&wifi_mac_string[0], &rx_buffer[28], 17);
                                wifi_mac_string[17] = '\0';
                            }

                            wifi_rx_state = WIFI_RX_ATFB_OK;
                            receive.rx_state = WIFI_RX_ATFB_OK;
                            push_event = true;
                        }
                        else if(memcmp(&(rx_buffer[rx_index - 7]), "ERROR", 5) == 0)
                            //else if((rx_buffer[rx_index - 4] == 'O') && (rx_buffer[rx_index - 3] == 'R'))
                        {               
                            wifi_rx_state = WIFI_RX_ATFB_ERROR;
                            receive.rx_state = WIFI_RX_ATFB_ERROR;
                            push_event = true;
                        }
                    }
                    
                    line_start = rx_index;
                }
                break;
                
            case WIFI_RX_IPD:
                if(rx_buffer[rx_index - 1] == ':')
                {
                    if(rx_index > 8)
                    {
                        client_id_active = rx_buffer[5] - '0';
                        client_data_size = 0;
                        for(i = 7; i < (rx_index - 1); i++)
                        {
                            client_data_size *= 10;
                            client_data_size += (rx_buffer[i] - '0'); 
                        }
                        
                        wifi_rx_state = WIFI_RX_IPD_RECEVING;
                        client_data_index = 0;
                        receive.client_id = client_id_active;
                        receive.rx_state = WIFI_RX_IPD_RECEVING;
                    }
                    else
                    {
                        wifi_rx_state = WIFI_RX_IPD_ERROR;
                        receive.rx_state = WIFI_RX_IPD_ERROR;
                    }     
                    push_event = true;
                }
                break;
                
            case WIFI_RX_IPD_RECEVING:
                client_data_index++;  
                rx_index--;
                if(client_data_index >= client_data_size)
                {
                    wifi_rx_state = WIFI_RX_IPD_OK;
                    receive.client_id = client_id_active;
                    receive.rx_state = WIFI_RX_IPD_OK;
                    push_event = true;
                }
                break;
                
            case WIFI_RX_ID:
                if(rx_buffer[rx_index - 1] == '\n')
                {
                    if(memcmp(&(rx_buffer[2]), "CLOSED", 6) == 0)
                        //if((rx_buffer[2] == 'C') && (rx_buffer[3] == 'L') && (rx_buffer[4] == 'O') && (rx_buffer[5] == 'S'))
                    {
                        client_list[(rx_buffer[0] - '0')] = 0;
                        wifi_rx_state = WIFI_RX_ID_CLOSED;
                        receive.client_id = rx_buffer[0] - '0';
                        receive.rx_state = WIFI_RX_ID_CLOSED;
                        push_event = true;
                    }
                    else if(memcmp(&(rx_buffer[2]), "CONNECT", 7) == 0)
                        //else if((rx_buffer[2] == 'C') && (rx_buffer[3] == 'O') && (rx_buffer[4] == 'N') && (rx_buffer[5] == 'N'))
                    {
                        client_list[(rx_buffer[0] - '0')] = 1;
                        wifi_rx_state = WIFI_RX_ID_CONNECT;
                        receive.client_id = rx_buffer[0] - '0';
                        client_id_active = receive.client_id;
                        receive.rx_state = WIFI_RX_ID_CONNECT;
                        push_event = true;
                    }               
                    else
                    {
                        wifi_rx_state = WIFI_RX_ERROR;
                        receive.rx_state = WIFI_RX_ERROR;
                        push_event = true;
                    }
                }
                break;
                
            case WIFI_RX_RECV:
                if(rx_buffer[rx_index - 1] == '\n')
                {
                    if(memcmp(&(rx_buffer[line_start]), "SEND OK", 7) == 0)
                        //if((rx_buffer[rx_index - 3] == 'O') && (rx_buffer[rx_index - 2] == 'K'))
                    {
                        wifi_rx_state = WIFI_RX_SEND_OK;
                        receive.rx_state = WIFI_RX_SEND_OK;
                        push_event = true;
                    }
                    else if(memcmp(&(rx_buffer[line_start]), "SEND FAIL", 9) == 0)
                        //else if((rx_buffer[rx_index - 5] == 'F') && (rx_buffer[rx_index - 4] == 'A') && (rx_buffer[rx_index - 3] == 'I') && (rx_buffer[rx_index - 2] == 'L'))
                    {               
                        wifi_rx_state = WIFI_RX_SEND_FAIL;
                        receive.rx_state = WIFI_RX_SEND_FAIL;
                        push_event = true;
                    }
                    
                    line_start = rx_index;
                }
                break;
                
            case WIFI_RX_ERROR:
                /* Wait for full packet */
                if(rx_buffer[rx_index - 1] == '\n')
                {
                    receive.rx_state = WIFI_RX_ERROR;
                    push_event = true;
                }
                break;
            default:
                break;
            }
        }
        
        if(wifi_rx_state == WIFI_RX_IPD_RECEVING)
        {
            /* receiving data payload */
            HAL_UART_Receive_DMA(huart, (client_data + client_data_index), 1);
        }
        else if(push_event == true)
        {
            /* start a new packet */
            HAL_UART_Receive_DMA(huart, rx_buffer, 1);
            rx_index = 0;
        }
        else
        {
            /* ready for receive next byte */
            HAL_UART_Receive_DMA(huart, (rx_buffer + rx_index), 1);
        }
        
        /* post to queue when state update */       
        if(push_event == true)
        {   
            /* send new state to queue */
            xQueueSendFromISR( receive_queue, &receive, &xHigherPriorityTaskWoken );
            
            /* switch context if necessary.*/
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

/*******************************************************************************
* @Brief   UART Transmit Complete Callback
* @Param   
* @Note    Update transmit finish flag
* @Return  
*******************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &hwifi_uart)
    {
        wifi_uart_tx_done = true;
    }
}

#endif /* USE_DEMO_VERSION */
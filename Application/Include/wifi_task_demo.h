/*
***************************************************************************************************
*                               WiFi Module Control Task
*       this is demo version which use demo protocol and PC software TCP_Client V1.00.exe
*
* File   : wifi_task.h
* Author : Douglas Xie
* Date   : 2017.10.09
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifdef USE_DEMO_VERSION

#ifndef WIFI_API_H_DEMO
#define WIFI_API_H_DEMO

/* Includes -------------------------------------------------------------------------------------*/
#include "global_config.h"

/* Macro defines --------------------------------------------------------------------------------*/
/* Task period */
#define WIFI_TASK_PERIOD        (10 / portTICK_PERIOD_MS)
#define WIFI_RESET_DELAY        (1000 / portTICK_PERIOD_MS)

/* WiFi interface define */
#define WIFI_USART              USART1
#define hwifi_uart              huart1

/* Wifi module uart baudrate */
#define WIFI_BAUDRATE_DEFAULT   115200UL
#define WIFI_BAUDRATE_RUNNING   921600UL

/* UART transmit timeout */
#define WIFI_RX_FB_TIMEOUT      (1000 / portTICK_PERIOD_MS)
#define WIFI_RX_DATA_TIMEOUT    (2000 / portTICK_PERIOD_MS)

/* UART buffer size */
#define WIFI_TX_BUF_SIZE        128
#define WIFI_RX_BUF_SIZE        128
#define WIFI_DATA_BUF_SIZE      CLIENT_BUFFER_SIZE

/* New line code */
#define NEW_LINE                "\r\n"

/* WiFi queue parameter */
#define WIFI_QUEUE_LENGTH        (5)                        /* Queue max item number */
#define WIFI_QUEUE_ITEM_SIZE     (sizeof(WiFi_Receive_t)) /* Item size is WiFi_Receive_t type */

/*------------------- WiFi Setup -------------------*/
#define WIFI_SSID               "AniTech_"
#define WIFI_PASSWORD           "12345678"                  /* Max 64 byte ASCII */
//#define WIFI_CHANNEL            11                          /* Channel 1 - 12 */
#define WIFI_CHANNEL            (((uint8_t)wifi_mac_string[16])%12 + 1)
#define WIFI_ENCRYPT            WIFI_ECN_WPA_WPA2_PSK       /* WiFi_Enctypt_t member */

/* TCP server port */
#define WIFI_TCP_SERVER_PORT    2017           

/* TCP server timeout: 5 min */
#define WIFI_TCP_SERVER_TIMEOUT 300

/* WiFi error repeat time */
#define WIFI_ERROR_REPEAT       5

/* WiFi packet size */
#define WIFI_PACKET_SIZE        1024

/* Data Type Define -----------------------------------------------------------------------------*/
/* WiFi main task state */
typedef enum
{
    WIFI_CTRL_ECHO,
    WIFI_CTRL_SETUP_UART,
    WIFI_CTRL_SETUP_AP,
    WIFI_CTRL_START_SERVER,
    WIFI_CTRL_GET_MAC,      /* add for default SoftAP mode information */
    WIFI_CTRL_GET_IP,       /* add for station mode information */

    WIFI_CTRL_CLIENT_MANAGE,
    WIFI_CTRL_RECE_REQUEST,
    WIFI_CTRL_SEND_RESPOND,
    WIFI_CTRL_SEND_IMAGE,
    WIFI_CTRL_ALIVE_TEST,
    
    /* IDLE ---event--------> SEND DATA */
    /* IDLE ---rx ipd-------> RECE DATA */
    /* IDLE ---rx id--------> CLIENT MANAGE */
    /* IDLE ---rx unknown---> IDLE */
    WIFI_CTRL_IDLE,

} WiFi_CtrlState_t;

/* WiFi receive data type */
typedef enum
{
    /* General feedback */
    WIFI_RX_NONE,
    WIFI_RX_ERROR,              /* ERROR */
    WIFI_RX_OVERFLOW,           /* Receive buffer overflow */
    
    /* WiFi config, AT command feedback */
    WIFI_RX_ATFB,               /* ATE1, AT+GMR ... */
    WIFI_RX_ATFB_OK,            /* OK */
    WIFI_RX_ATFB_ERROR,         /* ERROR*/
    
    /* TCP client connect or closed */
    WIFI_RX_ID,                 /* n,xxx  n is in range 0~4 */
    WIFI_RX_ID_CONNECT,         /* n,CONNECT */
    WIFI_RX_ID_CLOSED,          /* n,CLOSED */
    
    /* Receive data from TCP client */
    WIFI_RX_IPD,                /* +IPD, */
    WIFI_RX_IPD_RECEVING,       /* +IPD, receving */
    WIFI_RX_IPD_OK,             /* +IPD,n,m:xxxxx(m bytes) */
    WIFI_RX_IPD_ERROR,          /* Receive IPD error */
    
    /* Send data to TCP client */
    WIFI_RX_SEND_READY,         /* > */
    WIFI_RX_RECV,               /* Recv mbytes */
    WIFI_RX_SEND_OK,            /* SEND OK */
    WIFI_RX_SEND_FAIL,          /* SEND FAIL */
    
} WiFi_RxState_t;

/* WiFi encrypt type */
typedef enum
{
    WIFI_ECN_OPEN = 0,
    WIFI_ECN_WEP,
    WIFI_ECN_WPA_PSK,
    WIFI_ECN_WPA2_PSK,
    WIFI_ECN_WPA_WPA2_PSK
} WiFi_Enctypt_t;

/* WiFi rx queue item struct */
typedef struct
{
    unsigned char client_id;
    WiFi_RxState_t rx_state;
    
} WiFi_Receive_t;

/* Public variables ----------------------------------------------------------------------------*/


/* Function declaration -------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   WiFi Control Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void WiFi_ControlTask(void * argument);



#endif /* WIFI_API_H_DEMO */
#endif /* USE_DEMO_VERSION */

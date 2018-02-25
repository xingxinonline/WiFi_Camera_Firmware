/*
***************************************************************************************************
*                                 Global Config File
*
* File   : global_config.h
* Author : Douglas Xie
* Date   : 2017.04.24
***************************************************************************************************
* Copyright (C) 2016-2017 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

/* Include Head Files ---------------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

/* Macro Define ---------------------------------------------------------------------------------*/
#define APP_VERSION     ((uint32_t)205)                 /* v2.05 */
#define APP_VER_DBG     "RunTime Version V2.05\r\n"     /* display for debug print */
#define APP_VER_DISP    "      V2.05     "              /* 16bytes */
#define APP_NAME_DISP   " AniTech System "

#ifdef USE_DEMO_VERSION
#define FW_VERSION      "V2.00"        
#define HW_VERSION      "V2.00"
#define SN_CODE         "12345678"
#endif

/*--------------- FreeRTOS Parameter Macro Define -----------------------------------------------*/
/*=======================================================*/
/* FreeRTOS heap size actual use:                        */
/* ------ Runtime min free heap size:    XXXX bytes -----*/
/* ------ Actual use heap size:          XXXX bytes -----*/

/*=======================================================*/
/* Application task stack actual use: test on 2017.05.05 */
/* ------ Default task:             XXX bytes -----------*/
/* ------ Control wifi task:        XXX bytes -----------*/
/* ------ Debug task:               XXX bytes -----------*/

/*=======================================================*/
/* Application task period ------------------------------*/
/* ------ Default task:            1000 ms --------------*/
/* ------ Control wifi task:       200 ms --------------*/
/* ------ Debug task:              2000 ms --------------*/

/*=======================================================*/
/* Appication task stack size define --------------------*/
#define CFG_STACK_DEFAULT       (128)   /* 512  bytes */
#define	CFG_STACK_MONITOR       (128)   /* 512  bytes */
#define	CFG_STACK_WIFI          (256)   /* 1024 bytes */
#define	CFG_STACK_CLIENT        (128)   /* 512  bytes */
#define	CFG_STACK_MOTOR         (128)   /* 512  bytes */
#define	CFG_STACK_DISPLAY       (128)   /* 512  bytes */
#define	CFG_STACK_CAMERA        (128)   /* 512  bytes */
#define	CFG_STACK_SAVE          (128)   /* 512  bytes */
#define	CFG_STACK_DEBUG         (128)   /* 512  bytes */

/*=======================================================*/
/* Application task priority at which the tasks are created. */
#define CFG_PRIORITY_DEFAULT    4   /* osPriorityNormal */
#define	CFG_PRIORITY_MONITOR    9
#define	CFG_PRIORITY_WIFI       8
#define	CFG_PRIORITY_CLIENT     8
#define	CFG_PRIORITY_MOTOR      7
#define	CFG_PRIORITY_CAMERA     6
#define	CFG_PRIORITY_SAVE       5
#define	CFG_PRIORITY_DISPLAY    4
#define	CFG_PRIORITY_DEBUG      3
  

#endif /* GLOBAL_CONFIG_H */

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/

/*
***************************************************************************************************
*                            Flash Memory API of Info and Config
*
* File   : memory.h
* Author : Douglas Xie
* Date   : 2018.02.11
***************************************************************************************************
* Copyright (C) 2017-2018 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

#ifndef _MEMORY_H_
#define _MEMORY_H_

/* Includes -------------------------------------------------------------------------------------*/
#include "global_config.h"
#include "stdint.h"
#include "stdbool.h"

/* Macro defines --------------------------------------------------------------------------------*/
/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_12     ((uint32_t)0x08100000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_13     ((uint32_t)0x08104000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_14     ((uint32_t)0x08108000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_15     ((uint32_t)0x0810C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_16     ((uint32_t)0x08110000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_17     ((uint32_t)0x08120000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18     ((uint32_t)0x08140000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19     ((uint32_t)0x08160000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20     ((uint32_t)0x08180000) /* Base @ of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_21     ((uint32_t)0x081A0000) /* Base @ of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_22     ((uint32_t)0x081C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23     ((uint32_t)0x081E0000) /* Base @ of Sector 11, 128 Kbytes */
      
/* Flash allocation define */
#define INFO_ADDR_START         ((uint32_t)0x08008000)
#define INFO_ADDR_END           ((uint32_t)0x0800BFFF)

#define CONFIG_ADDR_START       ((uint32_t)0x0800C000)
#define CONFIG_ADDR_END         ((uint32_t)0x0800FFFF)

#define APP_ADDR_START          ((uint32_t)0x08020000)
#define APP_ADDR_END            ((uint32_t)0x0805FFFF)

#define OTA_ADDR_START          ((uint32_t)0x08060000)
#define OTA_ADDR_END            ((uint32_t)0x0809FFFF)

/* Application config parameter define */
#define APP_ESP8266_SoftAP      ((uint32_t)0)           /* ap , default mode */
#define APP_ESP8266_STATION     ((uint32_t)0xABEA8266)  /* station */
#define APP_CONFIG_OK           ((uint8_t) 0x0755)

/* Data Type Define -----------------------------------------------------------------------------*/
typedef union APP_STATUS
{
    struct
    {
        uint32_t bootloader_version;
        uint32_t app_version;
        uint32_t ota_version;
        uint32_t ota_length;
        uint16_t ota_crc;
        uint16_t reserved;
        uint32_t checksum;
    };
    uint32_t array32[6];
    uint8_t  array[24];
} App_Info_t;

#pragma   pack(1)
typedef struct CFG_SCH
{
    uint8_t t_hour;
    uint8_t t_minute;
    uint8_t feed_m1;
    uint8_t feed_m2;
    uint8_t feed_m3;
    uint8_t feed_m4;
    uint8_t feed_m5;
    /* total 7 bytes */
} SchCfg_t;

typedef struct CFG_MOTOR
{
    uint8_t m_dir[5];
    uint16_t m_freq[5];
    uint16_t m_step[5];
    /* total 25 bytes */
} MotorCfg_t;

typedef union APP_CONFIG
{
    struct
    {
        uint32_t    esp8266_mode;      /* AP_MODE or STATION_MODE */
        uint16_t    wifi_config_state;
        uint16_t    cloud_config_state;
        /* station mode config */
        uint8_t     wifi_ssid[32];
        uint8_t     wifi_passwd[32];
        uint8_t     account_id[32];
        uint8_t     account_passwd[32];
        uint8_t     cloud_server[64];
        uint16_t    cloud_port;     //2     
        MotorCfg_t  motor_cfg;      //25
        SchCfg_t    schedule[5];    //35
        uint16_t    sch_count;      //2     
        uint32_t    checksum;
    };
    uint32_t array32[65];
    uint8_t  array[260];
} App_Config_t;
#pragma   pack()

/* Public variables ----------------------------------------------------------------------------*/
extern App_Info_t app_info;
extern App_Config_t app_config;

/* Function declaration -------------------------------------------------------------------------*/

void Mem_ReadInfo(void);
void Mem_WriteInfo(void);
void Mem_ResetConfig(void);
void Mem_ReadConfig(void);
void Mem_WriteConfig(void);
void Mem_EraseApp(uint32_t start_addr, uint32_t end_addr);
void Mem_WriteApp(uint32_t start_addr, uint8_t *data_buf, uint32_t data_len);
uint16_t CRC16_CCITT(const uint8_t* pdata, uint16_t length);
uint32_t Mem_GetChecksum32(uint32_t *pdata, uint32_t length);
uint8_t Mem_GetChecksum8(uint8_t init_value, uint8_t *pdata, uint32_t length);
uint32_t Mem_GetSector(uint32_t address);


#endif /* _MEMORY_H_ */


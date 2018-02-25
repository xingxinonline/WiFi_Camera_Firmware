/*
***************************************************************************************************
*                            Flash Memory API of Info and Config
*
* File   : memory.c
* Author : Douglas Xie
* Date   : 2018.02.11
***************************************************************************************************
* Copyright (C) 2017-2018 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

/* Include Head Files ---------------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "global_config.h"
#include "memory.h"

/* Macro Define ---------------------------------------------------------------------------------*/

/* Global Variable ------------------------------------------------------------------------------*/
App_Info_t app_info;
App_Config_t app_config;

/* Private Function Declaration -----------------------------------------------------------------*/

/* Public Function ------------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief    Info Sector Update
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Mem_ReadInfo(void)
{
    uint32_t i = 0;

    /* read app information sector data */
    for(i = 0; i < sizeof(app_info); i++)
    {
        app_info.array[i] = *((uint8_t *)(INFO_ADDR_START + i));
    }
    
    /* validate checksum */
    if(app_info.checksum == Mem_GetChecksum32(app_info.array32, (sizeof(app_info)/4) - 1))
    {  
        if(app_info.app_version != APP_VERSION)
        {
            /* write new app information */
            app_info.app_version = APP_VERSION;
            Mem_WriteInfo();
        }
    }
    else
    {
        /* reboot to update info in bootloader */
        HAL_NVIC_SystemReset();
    }
}

/*******************************************************************************
* @Brief    Write App Info Sector
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Mem_WriteInfo(void)
{
    uint32_t i = 0;
    uint32_t address, sector_error = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;

    app_info.checksum = Mem_GetChecksum32(app_info.array32, (sizeof(app_info)/4) - 1);
    
    /* erase info sector */
    HAL_FLASH_Unlock();
    
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector        = Mem_GetSector(INFO_ADDR_START);
    EraseInitStruct.NbSectors     = 1;
    while(HAL_FLASHEx_Erase(&EraseInitStruct, &sector_error) != HAL_OK)
    {
        HAL_Delay(100);
    }
    
    /* write new info data */
    address = INFO_ADDR_START;
    for(i = 0; i < sizeof(app_info)/4; )
    {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, app_info.array32[i]) == HAL_OK)
        {
            address = address + 4;
            i++;
        }
    }
    
    HAL_FLASH_Lock();
}

/*******************************************************************************
* @Brief    Read Config Sector Data
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Mem_ReadConfig(void)
{
    uint32_t i = 0;

    /* read app information sector data */
    for(i = 0; i < sizeof(app_config); i++)
    {
        app_config.array[i] = *((uint8_t *)(CONFIG_ADDR_START + i));
    }
    
    /* validate checksum */
    if(app_config.checksum != Mem_GetChecksum32(app_config.array32, (sizeof(app_config)/4) - 1))
    {  
        /* reset default value when checksum error */
        memset(&app_config, 0, sizeof(app_config));   
        Mem_WriteConfig();
    }
}

/*******************************************************************************
* @Brief    Write Config Sector Data
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Mem_WriteConfig(void)
{
    uint32_t i = 0;
    uint32_t address, sector_error = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;

    /* update checksum */
    app_config.checksum = Mem_GetChecksum32(app_config.array32, (sizeof(app_config)/4) - 1);
    
    /* erase info sector */
    HAL_FLASH_Unlock();
    
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector        = Mem_GetSector(CONFIG_ADDR_START);
    EraseInitStruct.NbSectors     = 1;
    while(HAL_FLASHEx_Erase(&EraseInitStruct, &sector_error) != HAL_OK)
    {
        HAL_Delay(100);
    }
    
    /* write new info data */
    address = CONFIG_ADDR_START;
    for(i = 0; i < sizeof(app_config)/4; )
    {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, app_config.array32[i]) == HAL_OK)
        {
            address = address + 4;
            i++;
        }
    }
    
    HAL_FLASH_Lock();
}

/*******************************************************************************
* @Brief    Erase App Flash Sector
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Mem_EraseApp(uint32_t start_addr, uint32_t end_addr)
{
    uint32_t sector_error = 0;
    uint32_t first_sector = 0;
    uint32_t number_of_sector = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;

    HAL_FLASH_Unlock();
    
    /* Get the 1st sector to erase */
    first_sector = Mem_GetSector(start_addr);
    
    /* Get the number of sector to erase from 1st sector*/
    number_of_sector = Mem_GetSector(end_addr) - first_sector + 1;
    
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = first_sector;
    EraseInitStruct.NbSectors = number_of_sector;
    
    while(HAL_FLASHEx_Erase(&EraseInitStruct, &sector_error) != HAL_OK)
    {
        HAL_Delay(100);
    }
    
    HAL_FLASH_Lock();
}

/*******************************************************************************
* @Brief    Write App Bin Data
* @Param   
* @Note     should erase app flash before write data
* @Return  
*******************************************************************************/
void Mem_WriteApp(uint32_t start_addr, uint8_t *data_buf, uint32_t data_len)
{
    uint32_t i = 0;
    uint32_t write_addr = 0;

    HAL_FLASH_Unlock();
    
    /* write firmware bin data */
    write_addr = start_addr;
    while(i < data_len)
    {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, write_addr, data_buf[i]) == HAL_OK)
        {
            i++;
            write_addr++;
        }
    }
    
    HAL_FLASH_Lock();
}

/*******************************************************************************
* @Brief   CRC16 Verify - CCITT Mode
* @Param   [in]pdata: point to data buffer
*          [in]length: data buffer length
* @Note    
* @Return  crc result, 16bit
*******************************************************************************/   
uint16_t CRC16_CCITT(const uint8_t* pdata, uint16_t length)
{
    const uint16_t seed = 0xFFFF;
    const uint16_t poly16 = 0x1021;
    
    uint16_t i = 0;
    uint16_t wTemp = 0;      
    uint16_t wCRC = 0;      
    
    wCRC = seed;
    
    for(i = 0; i < length; i++)      
    {             
        for(int j = 0; j < 8; j++)      
        {      
            wTemp = ((pdata[i] << j) & 0x80 ) ^ ((wCRC & 0x8000) >> 8);      
    
            wCRC <<= 1;      
    
            if(wTemp != 0)       
            {
                wCRC ^= poly16;
            }     
        }      
    }      
    
    return wCRC;     
}  

uint32_t Mem_GetChecksum32(uint32_t *pdata, uint32_t length)
{
    uint32_t checksum = 0x12345678;
    uint32_t i = 0;
    
    for(i = 0; i < length; i++)
    {
        checksum += *pdata;
        pdata++;
    }
    
    return checksum;
}

uint8_t Mem_GetChecksum8(uint8_t init_value, uint8_t *pdata, uint32_t length)
{
    uint8_t checksum = 0;
    uint32_t i = 0;
    
    checksum = init_value;
    for(i = 0; i < length; i++)
    {
        checksum += *pdata;
        pdata++;
    }
    
    return checksum;
}

/**
* @brief  Gets the sector of a given address
* @param  None
* @retval The sector of a given address
*/
uint32_t Mem_GetSector(uint32_t address)
{
    uint32_t sector = 0;
    
    if((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_SECTOR_0;
    }
    else if((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_SECTOR_1;
    }
    else if((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_SECTOR_2;
    }
    else if((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_SECTOR_3;
    }
    else if((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_SECTOR_4;
    }
    else if((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_SECTOR_5;
    }
    else if((address < ADDR_FLASH_SECTOR_7) && (address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_SECTOR_6;
    }
    else if((address < ADDR_FLASH_SECTOR_8) && (address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_SECTOR_7;
    }
    else if((address < ADDR_FLASH_SECTOR_9) && (address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_SECTOR_8;
    }
    else if((address < ADDR_FLASH_SECTOR_10) && (address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_SECTOR_9;
    }
    else if((address < ADDR_FLASH_SECTOR_11) && (address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_SECTOR_10;
    }
    else if((address < ADDR_FLASH_SECTOR_12) && (address >= ADDR_FLASH_SECTOR_11))
    {
        sector = FLASH_SECTOR_11;
    }
    else if((address < ADDR_FLASH_SECTOR_13) && (address >= ADDR_FLASH_SECTOR_12))
    {
        sector = FLASH_SECTOR_12;
    }
    else if((address < ADDR_FLASH_SECTOR_14) && (address >= ADDR_FLASH_SECTOR_13))
    {
        sector = FLASH_SECTOR_13;
    }
    else if((address < ADDR_FLASH_SECTOR_15) && (address >= ADDR_FLASH_SECTOR_14))
    {
        sector = FLASH_SECTOR_14;
    }
    else if((address < ADDR_FLASH_SECTOR_16) && (address >= ADDR_FLASH_SECTOR_15))
    {
        sector = FLASH_SECTOR_15;
    }
    else if((address < ADDR_FLASH_SECTOR_17) && (address >= ADDR_FLASH_SECTOR_16))
    {
        sector = FLASH_SECTOR_16;
    }
    else if((address < ADDR_FLASH_SECTOR_18) && (address >= ADDR_FLASH_SECTOR_17))
    {
        sector = FLASH_SECTOR_17;
    }
    else if((address < ADDR_FLASH_SECTOR_19) && (address >= ADDR_FLASH_SECTOR_18))
    {
        sector = FLASH_SECTOR_18;
    }
    else if((address < ADDR_FLASH_SECTOR_20) && (address >= ADDR_FLASH_SECTOR_19))
    {
        sector = FLASH_SECTOR_19;
    }
    else if((address < ADDR_FLASH_SECTOR_21) && (address >= ADDR_FLASH_SECTOR_20))
    {
        sector = FLASH_SECTOR_20;
    }
    else if((address < ADDR_FLASH_SECTOR_22) && (address >= ADDR_FLASH_SECTOR_21))
    {
        sector = FLASH_SECTOR_21;
    }
    else if((address < ADDR_FLASH_SECTOR_23) && (address >= ADDR_FLASH_SECTOR_22))
    {
        sector = FLASH_SECTOR_22;
    }
    else /* (address < FLASH_END_ADDR) && (address >= ADDR_FLASH_SECTOR_23) */
    {
        sector = FLASH_SECTOR_23;
    }  
    return sector;
}


/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
static uint8_t buffer[_MAX_SS]; /* a work buffer for the f_mkfs() */
/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */  
  SD_CreateNewFile("a", "v");
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */  
}

/* USER CODE BEGIN Application */
     
void SD_CreateNewFile(uint8_t *filename, uint8_t *text_string)
{
    FRESULT res;                                          /* FatFs function common result code */
    uint32_t byteswritten, bytesread;                     /* File write/read counts */
    uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
    uint8_t rtext[100];                                   /* File read buffer */
    
    /*##-1- Link the micro SD disk I/O driver ##################################*/
    if(retSD == 0)
    {  
        /*##-2- Register the file system object to the FatFs module ##############*/
        if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) != FR_OK)
        {
            /* FatFs Initialization Error */
            Error_Handler();
        }
        else
        {
            /*##-3- Create a FAT file system (format) on the logical drive #########*/
            /* WARNING: Formatting the uSD card will delete all content on the device */
            retSD = f_mkfs((TCHAR const*)SDPath, FM_FAT32, 0, buffer, sizeof(buffer));
            if(retSD != FR_OK)
            {
                /* FatFs Format Error */
                Error_Handler();
            }
            else
            {
                /*##-4- Create and Open a new text file object with write access #####*/
                retSD = f_open(&SDFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE);
                if(retSD != FR_OK)
                {
                    /* 'STM32.TXT' file Open for write Error */
                    Error_Handler();
                }
                else
                {
                    /*##-5- Write data to the text file ################################*/
                    res = f_write(&SDFile, wtext, sizeof(wtext), (void *)&byteswritten);
                    
                    if((byteswritten == 0) || (res != FR_OK))
                    {
                        /* 'STM32.TXT' file Write or EOF Error */
                        Error_Handler();
                    }
                    else
                    {
                        /*##-6- Close the open text file #################################*/
                        f_close(&SDFile);
                        
                        /*##-7- Open the text file object with read access ###############*/
                        if(f_open(&SDFile, "STM32.TXT", FA_READ) != FR_OK)
                        {
                            /* 'STM32.TXT' file Open for read Error */
                            Error_Handler();
                        }
                        else
                        {
                            /*##-8- Read data from the text file ###########################*/
                            res = f_read(&SDFile, rtext, sizeof(rtext), (UINT*)&bytesread);
                            
                            if((bytesread == 0) || (res != FR_OK))
                            {
                                /* 'STM32.TXT' file Read or EOF Error */
                                Error_Handler();
                            }
                            else
                            {
                                /*##-9- Close the open text file #############################*/
                                f_close(&SDFile);
                                
                                /*##-10- Compare read data with the expected data ############*/
                                if ((bytesread != byteswritten))
                                {                
                                    /* Read data is different from the expected data */
                                    Error_Handler();
                                }
                                else
                                {
                                    /* Success of the demo: no error occurrence */
                                    //TODO: ADD SUCCESS HANDLE
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    /*##-11- Unlink the micro SD disk I/O driver ###############################*/
    FATFS_UnLinkDriver(SDPath);
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

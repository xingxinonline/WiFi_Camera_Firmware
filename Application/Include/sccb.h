#ifndef __SCCB_H
#define __SCCB_H
#include "stm32f4xx_hal.h" 
//////////////////////////////////////////////////////////////////////////////////	 
/*SCCB BUS set*/							  
////////////////////////////////////////////////////////////////////////////////// 


//IO方向设置
#define SCCB_SDA_IN()  GPIO_in(DCMI_SDA_GPIO_Port,DCMI_SDA_Pin)	//PC3输入
#define SCCB_SDA_OUT() GPIO_out(DCMI_SDA_GPIO_Port,DCMI_SDA_Pin)	//PC3 输出


//IO操作函数	  
#define SCCB_SCL_HIGH           HAL_GPIO_WritePin(DCMI_SCL_GPIO_Port, DCMI_SCL_Pin, GPIO_PIN_SET)
#define SCCB_SCL_LOW            HAL_GPIO_WritePin(DCMI_SCL_GPIO_Port, DCMI_SCL_Pin, GPIO_PIN_RESET)
#define SCCB_SDA_HIGH           HAL_GPIO_WritePin(DCMI_SDA_GPIO_Port, DCMI_SDA_Pin, GPIO_PIN_SET)
#define SCCB_SDA_LOW            HAL_GPIO_WritePin(DCMI_SDA_GPIO_Port, DCMI_SDA_Pin, GPIO_PIN_RESET)
#define SCCB_READ_SDA           HAL_GPIO_ReadPin(DCMI_SDA_GPIO_Port, DCMI_SDA_Pin)		//输入SDA    
#ifdef OV2640
#define SCCB_ID                 0X60			//OV7670的ID
#else
#define SCCB_ID                 0X42 			//OV7670的ID
#endif
#define OV7670_DOWN_LOW         HAL_GPIO_WritePin(DCMI_POWN_GPIO_Port, DCMI_POWN_Pin, GPIO_PIN_RESET)
#define OV7670_RESET_LOW        HAL_GPIO_WritePin(DCMI_RESET_GPIO_Port, DCMI_RESET_Pin, GPIO_PIN_RESET)
#define OV7670_RESET_HIGH       HAL_GPIO_WritePin(DCMI_RESET_GPIO_Port, DCMI_RESET_Pin, GPIO_PIN_SET)
///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
uint8_t SCCB_WR_Byte(uint8_t dat);
uint8_t SCCB_RD_Byte(void);
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);
void GPIO_out(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_in(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
#endif














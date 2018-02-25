#ifndef __DELAY_H
#define __DELAY_H 			   
#include "stm32f4xx_hal.h"    

extern uint16_t g_time_count;

void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);
#endif






























#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//////////////////////////////////////////////////////////////////////////////////  
uint16_t g_time_count;
void delay_us(uint32_t nus)
{		
    static uint32_t temp_time=0;
    
    while (temp_time<(nus/10))//10us interrupt
    {
        if(g_time_count)
        {
            temp_time++;
            g_time_count=0;
        }
    }
    temp_time=0;
}

void delay_ms(uint16_t nms)//10us interrupt
{
#if 0
    
    static uint32_t temp_time=0;

    while (temp_time<(nms*100))
    {
        if(g_time_count)
        {
            temp_time++;
            g_time_count=0;
        }
    }
    temp_time=0;
    
#else
    
    vTaskDelay(nms/portTICK_PERIOD_MS);

#endif
}








































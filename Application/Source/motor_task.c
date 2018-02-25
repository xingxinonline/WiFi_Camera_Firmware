/*
***************************************************************************************************
*                            Motor Control Task
*
* File   : motor_task.c
* Author : Douglas Xie
* Date   : 2017.10.27
***************************************************************************************************
* Copyright (C) 2017 Douglas Xie.  All rights reserved.
***************************************************************************************************
*/

/* Include Head Files ---------------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "string.h"

#include "global_config.h"
#include "client_task.h"
#include "wifi_task.h"
#include "motor_task.h"
#include "debug_task.h"

/* Macro Define ---------------------------------------------------------------------------------*/



/* Global Variable ------------------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
Motor_Group_t motor_group;


/* Private Function Declaration -----------------------------------------------------------------*/
void Motor_SetTimerPeriod(uint16_t period);
void Motor_SetDefault(void);



/* Public Function ------------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Motor Control Task
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Motor_ControlTask(void * argument)
{
    Motor_State_t motor_state = MOTOR_IDLE;
    const uint8_t led_period = 100 / MOTOR_TASK_PERIOD;    /* led blink about 5 times per second */
    uint8_t led_counter = 0;
    
    /* Default setting */
    Motor_SetTimerPeriod(0);
    Motor_SetDefault();
        
    DBG_SendMessage(DBG_MSG_TASK_STATE, "Motor Task Start\r\n");

    for(;;)
    {
        switch(motor_state)
        {
        case MOTOR_RUN_CH1:
            if(motor_group.motor1.step == 0)
            {
                /* No action for motor1, swift to next channel */
                motor_state = MOTOR_RUN_CH2;
            }
            else
            {
                /* Update timer if channel timer period is different with current setting */
                if( motor_group.timer_period != motor_group.motor1.period )
                {
                    Motor_SetTimerPeriod(motor_group.motor1.period);
                    Motor_OutputLow(MOTOR_STEP_CH1);
                }

                /* Start control motor1 */
                if( motor_group.motor1.enable == 0 )
                {
                    if(motor_group.motor1.direction == 'L')
                    {
                        Motor_TurnLeft(MOTOR_DIR_CH1);
                    }
                    else
                    {
                        Motor_TurnRight(MOTOR_DIR_CH1);
                    }
                    Motor_OutputLow(MOTOR_STEP_CH1);
                    Motor_TurnEnable(MOTOR_EN_CH1);
                    motor_group.motor1.enable = 1;
                    
                    DBG_SendMessage(DBG_MSG_MOTOR, "Motor: Channel1 Start Turning\r\n");
                }
            }
            break;
            
        case MOTOR_RUN_CH2:
            if(motor_group.motor2.step == 0)
            {
                /* No action for motor2, swift to next channel */
                Motor_OutputLow(MOTOR_STEP_CH2);
                Motor_TurnDisable(MOTOR_EN_CH2);
                motor_group.motor2.enable = 0;
                motor_state = MOTOR_RUN_CH3;
            }
            else
            {
                /* Update timer if channel timer period is different with current setting */
                if( motor_group.timer_period != motor_group.motor2.period )
                {
                    Motor_SetTimerPeriod(motor_group.motor2.period);
                    Motor_OutputLow(MOTOR_STEP_CH2);
                }

                /* Start control motor2 */
                if( motor_group.motor2.enable == 0 )
                {
                    if(motor_group.motor2.direction == 'L')
                    {
                        Motor_TurnLeft(MOTOR_DIR_CH3);
                    }
                    else
                    {
                        Motor_TurnRight(MOTOR_DIR_CH2);
                    }
                    Motor_OutputLow(MOTOR_STEP_CH2);
                    Motor_TurnEnable(MOTOR_EN_CH2);
                    motor_group.motor2.enable = 1;
                    
                    DBG_SendMessage(DBG_MSG_MOTOR, "Motor: Channel2 Start Turning\r\n");
                }
            }
            break;
            
        case MOTOR_RUN_CH3:
            if(motor_group.motor3.step == 0)
            {
                /* No action for motor3, swift to next channel */
                Motor_OutputLow(MOTOR_STEP_CH3);
                Motor_TurnDisable(MOTOR_EN_CH3);
                motor_group.motor3.enable = 0;
                motor_state = MOTOR_RUN_CH4;
            }
            else
            {
                /* Update timer if channel timer period is different with current setting */
                if( motor_group.timer_period != motor_group.motor3.period )
                {
                    Motor_SetTimerPeriod(motor_group.motor3.period);
                    Motor_OutputLow(MOTOR_STEP_CH3);
                }

                /* Start control motor3 */
                if( motor_group.motor3.enable == 0 )
                {
                    if(motor_group.motor3.direction == 'L')
                    {
                        Motor_TurnLeft(MOTOR_DIR_CH3);
                    }
                    else
                    {
                        Motor_TurnRight(MOTOR_DIR_CH3);
                    }
                    Motor_OutputLow(MOTOR_STEP_CH3);
                    Motor_TurnEnable(MOTOR_EN_CH3);
                    motor_group.motor3.enable = 1;
                    
                    DBG_SendMessage(DBG_MSG_MOTOR, "Motor: Channel3 Start Turning\r\n");
                }
            }
            break;
            
        case MOTOR_RUN_CH4:
            if(motor_group.motor4.step == 0)
            {
                /* No action for motor4, swift to next channel */
                Motor_OutputLow(MOTOR_STEP_CH4);
                Motor_TurnDisable(MOTOR_EN_CH4);
                motor_group.motor4.enable = 0;
                motor_state = MOTOR_RUN_CH5;
            }
            else
            {
                /* Update timer if channel timer period is different with current setting */
                if( motor_group.timer_period != motor_group.motor4.period )
                {
                    Motor_SetTimerPeriod(motor_group.motor4.period);
                    Motor_OutputLow(MOTOR_STEP_CH4);
                }

                /* Start control motor4 */
                if( motor_group.motor4.enable == 0 )
                {
                    if(motor_group.motor4.direction == 'L')
                    {
                        Motor_TurnLeft(MOTOR_DIR_CH4);
                    }
                    else
                    {
                        Motor_TurnRight(MOTOR_DIR_CH4);
                    }
                    Motor_OutputLow(MOTOR_STEP_CH4);
                    Motor_TurnEnable(MOTOR_EN_CH4);
                    motor_group.motor4.enable = 1;
                    
                    DBG_SendMessage(DBG_MSG_MOTOR, "Motor: Channel4 Start Turning\r\n");
                }
            }
            break;
            
        case MOTOR_RUN_CH5:
            if(motor_group.motor5.step == 0)
            {
                /* No action for motor5, swift to next channel */
                Motor_OutputLow(MOTOR_STEP_CH5);
                Motor_TurnDisable(MOTOR_EN_CH5);
                motor_group.motor5.enable = 0;
                motor_state = MOTOR_IDLE;
            }
            else
            {
                /* Update timer if channel timer period is different with current setting */
                if( motor_group.timer_period != motor_group.motor5.period )
                {
                    Motor_SetTimerPeriod(motor_group.motor5.period);
                    Motor_OutputLow(MOTOR_STEP_CH5);
                }

                /* Start control motor5 */
                if( motor_group.motor5.enable == 0 )
                {
                    if(motor_group.motor5.direction == 'L')
                    {
                        Motor_TurnLeft(MOTOR_DIR_CH5);
                    }
                    else
                    {
                        Motor_TurnRight(MOTOR_DIR_CH5);
                    }
                    Motor_OutputLow(MOTOR_STEP_CH5);
                    Motor_TurnEnable(MOTOR_EN_CH5);
                    motor_group.motor5.enable = 1;
                    
                    DBG_SendMessage(DBG_MSG_MOTOR, "Motor: Channel5 Start Turning\r\n");
                }
            }
            break;
            
        case MOTOR_IDLE:
            if( (motor_group.motor1.step == 0) && 
                (motor_group.motor2.step == 0) && 
                (motor_group.motor3.step == 0) && 
                (motor_group.motor4.step == 0) && 
                (motor_group.motor5.step == 0))
            {
                /* Stop pwm timer to save power when there is no motor running */
                if(motor_group.timer_period != 0)
                {
                    /* Disable pwm timer by set period to 0 */
                    Motor_SetTimerPeriod(0);
                    
                    DBG_SendMessage(DBG_MSG_MOTOR, "Motor: Waiting for Event\r\n");
                }
            }
            else
            {
                motor_state = MOTOR_RUN_CH1;
            }
            break;
      
        default:
            break;
        }
        
        /* Led blink while motor turning */
        if(motor_state == MOTOR_IDLE)
        {
            Motor_LedOff();
            led_counter = 0;
        }
        else
        {
            if(led_counter == 0)
            {
                Motor_LedToggle();
            }
            
            led_counter++;
            if(led_counter>=led_period)
            {
                led_counter = 0;
            }
        }
            
        vTaskDelay(MOTOR_TASK_PERIOD);
    }
}

/*******************************************************************************
* @Brief   Motor Step Control
* @Param   
* @Note    motor step control signal generation in timer IRQ handler
* @Return  
*******************************************************************************/
void Motor_StepControl_IRQ(void)
{
    if(motor_group.motor1.enable == 1)
    {
        if(motor_group.motor1.step != 0)
        {
            motor_group.motor1.step--;
            Motor_OutputToggle( MOTOR_STEP_CH1 );
        }
        else if(motor_group.motor1.enable == 1)
        {
            Motor_OutputLow(MOTOR_STEP_CH1);
            Motor_TurnDisable(MOTOR_EN_CH1);
            motor_group.motor1.enable = 0;
        }
    }
    else if(motor_group.motor2.enable == 1)
    {
        if(motor_group.motor2.step != 0)
        {
            motor_group.motor2.step--;
            Motor_OutputToggle( MOTOR_STEP_CH2 );
        }
        else if(motor_group.motor2.enable == 1)
        {
            Motor_OutputLow(MOTOR_STEP_CH2);
            Motor_TurnDisable(MOTOR_EN_CH2);
            motor_group.motor2.enable = 0;
        }
    }
    else if(motor_group.motor3.enable == 1)
    {
        if(motor_group.motor3.step != 0)
        {
            motor_group.motor3.step--;
            Motor_OutputToggle( MOTOR_STEP_CH3 );
        }
        else if(motor_group.motor3.enable == 1)
        {
            Motor_OutputLow(MOTOR_STEP_CH3);
            Motor_TurnDisable(MOTOR_EN_CH3);
            motor_group.motor3.enable = 0;
        }
    }
    else if(motor_group.motor4.enable == 1)
    {
        if(motor_group.motor4.step != 0)
        {
            motor_group.motor4.step--;
            Motor_OutputToggle( MOTOR_STEP_CH4 );
        }
        else if(motor_group.motor4.enable == 1)
        {
            Motor_OutputLow(MOTOR_STEP_CH4);
            Motor_TurnDisable(MOTOR_EN_CH4);
            motor_group.motor4.enable = 0;
        }
    }
    else if(motor_group.motor5.enable == 1)
    {
        if(motor_group.motor5.step != 0)
        {
            motor_group.motor5.step--;
            Motor_OutputToggle( MOTOR_STEP_CH5 );
        }
        else if(motor_group.motor5.enable == 1)
        {
            Motor_OutputLow(MOTOR_STEP_CH5);
            Motor_TurnDisable(MOTOR_EN_CH5);
            motor_group.motor5.enable = 0;
        }

    }
}


/*******************************************************************************
* @Brief   Set Motor Direction
* @Param   c_ch[in]: channel char type value '1' ~ '5'
*          c_dir[in]: direction char type value 'L' or 'R'
* @Note    set motor running direction
* @Return  
*******************************************************************************/
void Motor_SetDirection(uint8_t c_ch, uint8_t c_dir)
{
    switch(c_ch)
    {
    case '1':
        motor_group.motor1.direction = c_dir;
        break;
        
    case '2':
        motor_group.motor2.direction = c_dir;
        break;
        
    case '3':
        motor_group.motor3.direction = c_dir;
        break;
        
    case '4':
        motor_group.motor4.direction = c_dir;
        break;
        
    case '5':
        motor_group.motor5.direction = c_dir;
        break;
      
    default:
        break;
    }        
}

/*******************************************************************************
* @Brief   Get Motor Direction
* @Param   c_ch[in]: channel char type value '1' ~ '5'
* @Note    get motor running direction
* @Return  'L' or 'R'
*******************************************************************************/
uint8_t Motor_GetDirection(uint8_t c_ch)
{
    uint8_t c_dir = 'N';
    
    switch(c_ch)
    {
    case '1':
        c_dir = motor_group.motor1.direction;
        break;
        
    case '2':
        c_dir = motor_group.motor2.direction;
        break;
        
    case '3':
        c_dir = motor_group.motor3.direction;
        break;
        
    case '4':
        c_dir = motor_group.motor4.direction;
        break;
        
    case '5':
        c_dir = motor_group.motor5.direction;
        break;
      
    default:
        break;
    }      
    
    return c_dir;
}

/*******************************************************************************
* @Brief   Set Motor Step
* @Param   c_ch[in]: channel char type value '1' ~ '5'
*          step[in]: motor step counter value
* @Note    set motor step
* @Return  
*******************************************************************************/
void Motor_SetStep(uint8_t c_ch, uint16_t step)
{
    switch(c_ch)
    {
    case '1':
        motor_group.motor1.step = step * 2;
        break;
        
    case '2':
        motor_group.motor2.step = step * 2;
        break;
        
    case '3':
        motor_group.motor3.step = step * 2;
        break;
        
    case '4':
        motor_group.motor4.step = step * 2;
        break;
        
    case '5':
        motor_group.motor5.step = step * 2;
        break;
      
    default:
        break;
    } 
}

/*******************************************************************************
* @Brief   Get Motor Step
* @Param   c_ch[in]: channel char type value '1' ~ '5'
* @Note    get motor step
* @Return  motor step counter value
*******************************************************************************/
uint16_t Motor_GetStep(uint8_t c_ch)
{
    uint16_t step = 0;
        
    switch(c_ch)
    {
    case '1':
        step = motor_group.motor1.step;
        break;
        
    case '2':
        step = motor_group.motor2.step;
        break;
        
    case '3':
        step = motor_group.motor3.step;
        break;
        
    case '4':
        step = motor_group.motor4.step;
        break;
        
    case '5':
        step = motor_group.motor5.step;
        break;
      
    default:
        break;
    }       
    
    return step;
}


/*******************************************************************************
* @Brief   Set Motor Signal Frequency
* @Param   c_ch[in]: channel char type value '1' ~ '5'
*          frequency[in]: channel step signal frequency value
* @Note    set motor step signal frequency
* @Return  
*******************************************************************************/
void Motor_SetFrequency(uint8_t c_ch, uint32_t freq)
{
    uint16_t period = 0;
    
    /* calculate period value */
    period = (uint16_t)(MOTOR_TIMER_FREQ / (freq * 2));
        
    switch(c_ch)
    {
    case '1':
        motor_group.motor1.period = period;
        break;
        
    case '2':
        motor_group.motor2.period = period;
        break;
        
    case '3':
        motor_group.motor3.period = period;
        break;
        
    case '4':
        motor_group.motor4.period = period;
        break;
        
    case '5':
        motor_group.motor5.period = period;
        break;
      
    default:
        break;
    }        
}


/*******************************************************************************
* @Brief   Get Motor Signal Frequency
* @Param   c_ch[in]: channel char type value '1' ~ '5'
* @Note    get motor step signal frequency
* @Return  frequency value
*******************************************************************************/
uint32_t Motor_GetFrequency(uint8_t c_ch)
{
    uint32_t freq = 0;
    uint16_t period = 0;
    
    switch(c_ch)
    {
    case '1':
        period = motor_group.motor1.period;
        break;
        
    case '2':
        period = motor_group.motor2.period;
        break;
        
    case '3':
        period = motor_group.motor3.period;
        break;
        
    case '4':
        period = motor_group.motor4.period;
        break;
        
    case '5':
        period = motor_group.motor5.period;
        break;
      
    default:
        break;
    }    

    /* calculate frequency value */   
    freq = MOTOR_TIMER_FREQ / (period * 2);
    
    return freq;    
}

/* Private Function -----------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Set Motor Timer Period 
* @Param   
* @Note    set pwm timer period to update speed
* @Return  
*******************************************************************************/
void Motor_SetTimerPeriod(uint16_t period)
{
    TIM_MasterConfigTypeDef sMasterConfig;
    
    /* Reset timer setting */
    HAL_TIM_Base_DeInit(&hmotor_timer);
    
    if(period != 0)
    {
        /* Update new setting */
        hmotor_timer.Instance = MOTOR_TIMER;
        hmotor_timer.Init.Prescaler = MOTOR_TIMER_PRES;
        hmotor_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
        hmotor_timer.Init.Period = period;
        if (HAL_TIM_Base_Init(&hmotor_timer) != HAL_OK)
        {
            Error_Handler();
        }
        
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&hmotor_timer, &sMasterConfig) != HAL_OK)
        {
            Error_Handler();
        }
        
        HAL_TIM_Base_Start_IT(&hmotor_timer);
    }
    
    /* Update current period to global variable */
    motor_group.timer_period = period;
}


/*******************************************************************************
* @Brief   Set Motor Parameter to Default
* @Note    
* @Return  
*******************************************************************************/
void Motor_SetDefault(void)
{
    motor_group.motor1.direction = MOTOR_DEFAULT_DIR;
    motor_group.motor1.period = MOTOR_DEFAULT_PERIOD;
    motor_group.motor1.step = 0;
    motor_group.motor1.enable = 0;

    motor_group.motor2.direction = MOTOR_DEFAULT_DIR;
    motor_group.motor2.period = MOTOR_DEFAULT_PERIOD;
    motor_group.motor2.step = 0;
    motor_group.motor2.enable = 0;

    motor_group.motor3.direction = MOTOR_DEFAULT_DIR;
    motor_group.motor3.period = MOTOR_DEFAULT_PERIOD;
    motor_group.motor3.step = 0;
    motor_group.motor3.enable = 0;

    motor_group.motor4.direction = MOTOR_DEFAULT_DIR;
    motor_group.motor4.period = MOTOR_DEFAULT_PERIOD;
    motor_group.motor4.step = 0;
    motor_group.motor4.enable = 0;

    motor_group.motor5.direction = MOTOR_DEFAULT_DIR;
    motor_group.motor5.period = MOTOR_DEFAULT_PERIOD;
    motor_group.motor5.step = 0;
    motor_group.motor5.enable = 0;
}



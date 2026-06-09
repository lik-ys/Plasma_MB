/*******************************************************************************
*
* Copyright (C) 2009 Sharopin Yuri
*
* File              : mb_timer.c
* Compiler          : IAR EWAARM 5.xx
* Version           : 0.0
* Last modified     : 25.06.2009
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU: STM32F103xx @ (Xtal = MHz | CPUclk = MHz)
* Description       : Таймер для MB
*                   : 

* Hardware          : .sch .pcb
*
********************************************************************************/

/*===============================[ SPECIAL ]===================================*/ 
//  Условная компиляция

/*===============================[ IMPORT DECLARATIONS ]=======================*/ 
// Все системные, библиотечные и заголовочные файлы 
/* ----------------------------- [ System includes   ]-------------------------*/
#include    "stm32f3xx_hal.h"
/*-------------------------------[ Platform level    ]-------------------------*/
#include   "main.h"
/*-------------------------------[ Standart libs     ]-------------------------*/
//#include   <> // 
/*-------------------------------[ Application Level ]-------------------------*/
#include    "MB_RegComm.h"  
#include    "modbus.h"
#include    "mb_slave.h"

/*===============================[ PRIVATE CONSTANTS ]=========================*/ 
// Все частные #defines и constants должны быть объявлены в данном разделе. 


/*===============================[ PRIVATE TYPES ]=============================*/ 
// Все частные типы, которые используются в данном исходном файле, должны быть объявлены в данном разделе.  
/*===============================[ PRIVATE VARIABLES ]=========================*/ 
// В этом разделе объявляются все частные переменные.  
/*===============================[ PRIVATE FUNCTIONS ]=========================*/ 
// Все частные функции, используемые в данном исходном файле, должны быть объявлены в этом разделе.  
/*===============================[ PRIVATE PSEUDO FUNCTIONS ]==================*/ 
// Все макросы должны быть объявлены в этом разделе.  
/*===============================[ PUBLIC  VARIABLES ]=========================*/ 
// Все глобальные переменные должны быть заданы в этом разделе.  

/*===============================[ EXTERN FUNCTIONS ]=========================*/ 


void vMBPortTimersEnable( void )    /* TIM_MOD_BUS enable counter */
{    
  //pMBTim->Instance->CNT = 1;
  __HAL_TIM_SET_COUNTER(pMBTim, 1);
  
  //pMBTim->Instance->SR = 0;
  __HAL_TIM_CLEAR_FLAG(pMBTim, TIM_FLAG_UPDATE);
  
  if ( HAL_OK != HAL_TIM_Base_Stop( pMBTim ) )
  {
    Error_Handler();
  }else;
  
  if ( HAL_OK == HAL_TIM_Base_Start_IT( pMBTim ))
  {  
  }else
  {
    Error_Handler();
  }
}

void vMBPortTimersDisable( void )   /* TIM_MOD_BUS enable counter */
{  
  HAL_TIM_Base_Stop_IT( pMBTim );
}

BOOL  xMBPortTimersInit( USHORT usTimeOut50us )
{
  pMBTim->Init.Period = usTimeOut50us;
  
  if ( HAL_TIM_Base_Init( pMBTim ) != HAL_OK )
  {
    Error_Handler( );
  }  
  
  vMBPortTimersDisable( );
  
  return (BOOL)TRUE;
} // xMBPortTimersInit()

///////////////////////////////////////////////////////////////////////////////
// Обработчик прерываний таймера 

/*
*
*/
void vMBTimerISR( void )// ISR_TC0_Handler(void)
{   
  pxMBPortCBTimerExpired( );   // == xMBRTUTimerT35Expired()
} // vMBTimerISR()

/** (END OF FILE  : mb_timer.c) 
*******************************/ 


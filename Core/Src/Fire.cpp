/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : Fire.c
* Compiler          : IAR EWA 8.32
* Version           : 0.0
* Created File      : 01.11.2023
* Last modified     : 01.11.2023
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU: f407 @ (Xtal = MHz | CPUclk = MHz)
* Description       : Главная плата управления плазмотроном
*                   : 
* Hardware          : .sch .pcb
*
TODO сделать поджиг 
********************************************************************************/

#include "main.h"
#include "Fire.h"
#include  "StateMashine.h"//
#include  "Timer.hpp"

extern  State_t     gStateSM;
extern  Timer*      hTimer;

/***
*
*/
void FireStart( void )
{
  hTimer->Time_Out( Timer::start, TIME_OUT_PWR_ON, EV_FIRE_START );
  HAL_GPIO_WritePin( CMD_FIRE_PWR_GPIO_Port, CMD_FIRE_PWR_Pin, GPIO_PIN_SET );
}       
/***
*
*/
void FireON( void )
{
  if ( gStateSM.st.bFireOn )
  {
    HAL_GPIO_WritePin( CMD_FIRE_FIRE_GPIO_Port, CMD_FIRE_FIRE_Pin,GPIO_PIN_SET ) ;
    HAL_GPIO_WritePin( CMD_FIRE_LOCK_GPIO_Port, CMD_FIRE_LOCK_Pin, GPIO_PIN_SET );    
    if ( hTimer->IsTimeOut( EV_FIRE_ON ) )
    {
      gStateSM.st.bFireOn = 0;
      gStateSM.proc = ST_FIRE_OFF; 
      gStateSM.st.bFireOff = 1;
    }else;
  }else;  
  //HAL_GPIO_WritePin( CMD_FIRE_PWR_GPIO_Port, CMD_FIRE_PWR_Pin, GPIO_PIN_SET );
  //HAL_GPIO_WritePin( COMM_FIRE_GPIO_Port, COMM_FIRE_Pin, GPIO_PIN_SET );  
}
/***
*
*/
void FireOFF( void )
{ 
  HAL_GPIO_WritePin( CMD_FIRE_PWR_GPIO_Port, CMD_FIRE_PWR_Pin, GPIO_PIN_RESET );  
  HAL_GPIO_WritePin( CMD_FIRE_FIRE_GPIO_Port, CMD_FIRE_FIRE_Pin,GPIO_PIN_RESET ) ;
  HAL_GPIO_WritePin( CMD_FIRE_LOCK_GPIO_Port, CMD_FIRE_LOCK_Pin, GPIO_PIN_RESET );  
  HAL_GPIO_WritePin( COMM_FIRE_GPIO_Port, COMM_FIRE_Pin, GPIO_PIN_RESET);  
}

/***
*  Включаем CMD_FIRE_PWR  через 1 секунду  включаем CMD_FIRE_LOCK & CMD_FIRE_FIRE держим 3 секунды и выключаем все
*
*/
void FireProcess( void )
{
  if ( gStateSM.st.bFireStrat ) gStateSM.proc = ST_FIRE_START;
  if ( gStateSM.st.bFireWaite ) gStateSM.proc = ST_FIRE_WAITE;
  if ( gStateSM.st.bFireOn )    gStateSM.proc = ST_FIRE_ON;
  if ( gStateSM.st.bFireOff )   gStateSM.proc = ST_FIRE_OFF;
  
  switch( gStateSM.proc )
  {
  case ST_FIRE_START:  
    gStateSM.proc =   ST_FIRE_WAITE;
    WR_DEBUG("Fire Start \r\n");
    FireStart(); 
    gStateSM.st.bFireStrat = 0;
    gStateSM.st.bFireWaite = 1;
  break;
  case ST_FIRE_WAITE: 
    if ( hTimer->IsTimeOut( EV_FIRE_START ) ) 
    {
      gStateSM.proc = ST_FIRE_ON;
      hTimer->Time_Out( Timer::start, TIME_HOLD, EV_FIRE_ON );
      WR_DEBUG("Fire ON \r\n");
      gStateSM.proc =  ST_FIRE_ON;
      gStateSM.st.bFireWaite = 0;
      gStateSM.st.bFireOn = 1;
    }    
  case ST_FIRE_ON:        
    FireON();
    break;
  case ST_FIRE_OFF:WR_DEBUG("Fire OFF __ \r\n");
    FireOFF();
    gStateSM.st.bFireOff = 0;
    break;
  default:;    
  }
}


// End of file -------------------------------
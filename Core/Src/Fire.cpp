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

#include  "main.h"
#include  "Fire.h"
#include  "StateMachine.h"
#include  "Timer.hpp"
#include  "user_mb_app.h"

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
}// FireON()

/***
*
*/
void FireOFF( void )
{ 
  HAL_GPIO_WritePin( CMD_FIRE_PWR_GPIO_Port,  CMD_FIRE_PWR_Pin, GPIO_PIN_RESET );  
  HAL_GPIO_WritePin( CMD_FIRE_FIRE_GPIO_Port, CMD_FIRE_FIRE_Pin,GPIO_PIN_RESET ) ;
  HAL_GPIO_WritePin( CMD_FIRE_LOCK_GPIO_Port, CMD_FIRE_LOCK_Pin, GPIO_PIN_RESET );
}//FireOFF()

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
    gMbStatus.bit.bFireStart = 0;
    SetMBRgS( REG_R_STATUS_S, gMbStatus.reg );
    WR_DEBUG("Fire Start \r\n");
    FireStart();        
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
  case ST_FIRE_OFF:
    WR_DEBUG("Fire OFF __ \r\n");
    FireOFF();
    gStateSM.st.bFireOff = 0;  gStateSM.st.bFireStrat = 0; gStateSM.st.bCommFire = 0;
    HAL_NVIC_EnableIRQ( EXTI4_IRQn );
    gMbStatus.bit.bFireStart = 1;
    SetMBRgS( REG_R_STATUS_S, gMbStatus.reg );  
    ///hTimer->Time_Out( Timer::start, TIME_OUT_FIRE_OFF, EV_FIRE_OFF );
    break;
  default:;    
  }
} // FireProcess()

/***
*
*/
void CommandProcess( void )
{
  if ( gStateSM.st.bCommStart    == 1 )  gStateSM.proc = ST_COMM_START  ;
  if ( gStateSM.st.bCommFire     == 1 )  gStateSM.proc = ST_COMM_FIRE   ;
  if ( gStateSM.st.bMetalContact == 1 )  gStateSM.proc = ST_METAL_CONTACT; 
  
  if ( gMbCntrl.bit.bFireStart ) 
  {
    gMbCntrl.bit.bFireStart = 0;
    SetMBRgS( REG_W_CNTRL, gMbCntrl.reg );
    gMbStatus.bit.bFireStart= 1;
    SetMBRgS( REG_R_STATUS_S, gMbStatus.reg );    
    
    gStateSM.st.bCommFire = 1;     
  }else;
  
  switch( gStateSM.proc )
  {
    case ST_COMM_START  :
      {
      static int cnt1 = 0;
      WR_DEBUG("COMM_START_Pin  cnt = %i\r\n", cnt1++);
      gStateSM.st.bCommStart = 0;
      }
      break;
    case ST_COMM_FIRE   : 
      {
//      HAL_NVIC_DisableIRQ(EXTI2_IRQn);
//      HAL_NVIC_DisableIRQ(EXTI3_IRQn);
//      HAL_NVIC_DisableIRQ(EXTI4_IRQn);
        if ( 0 == gStateSM.st.bFireStrat )
        {
          static int cnt3 = 0;
          WR_DEBUG("COMM_FIRE_Pin:  cnt = %i \r\n", cnt3++);              
          gStateSM.st.bFireStrat = 1;        
        }
      }
      break;
    case ST_METAL_CONTACT:
      {
      static int cnt2 = 0;
      WR_DEBUG("DI_METAL_CONTACT_Pin:  cnt = %i \r\n", cnt2++);      
      gStateSM.st.bMetalContact = 0;
      }
      break;
    default:;
  }
} // CommandProcess()


// End of file -------------------------------
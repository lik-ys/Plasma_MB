/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : Fire.cpp
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
* Hardware          : Motherboard v3.1
*


2. COMM_START - старт от ЧПУ
   "1" - включаем: дежурку, поджиг
3. DI_METAL_CONTACT - реализовано  на ЧПУ

4. Включение чоппера ??

на ВУ ++ :
1)   кн дежурная кнопка вк/выкл
2)   кн Stop (всё кроме газа)

********************************************************************************/

#include  "main.h"
#include  "Fire.h"
#include  "StateMachine.h"
#include  "Timer.hpp"
#include  "user_mb_app.h"

#include "io_process.h"

port_t gFireO[ fire_last ] = {
  { CMD_FIRE_PWR_Pin , CMD_FIRE_PWR_GPIO_Port  },
  { CMD_FIRE_LOCK_Pin, CMD_FIRE_LOCK_GPIO_Port },
  { CMD_FIRE_FIRE_Pin, CMD_FIRE_FIRE_GPIO_Port }
  //{ CNC_DO_0_Pin,      CNC_DO_0_GPIO_Port      } 
};

port_t gFireI[ in_fire_last ] = {
  { FIRE_DI_0_Pin       , FIRE_DI_0_GPIO_Port       },
  { COMM_START_Pin      , COMM_START_GPIO_Port      },
  { DI_METAL_CONTACT_Pin, DI_METAL_CONTACT_GPIO_Port},
  { COMM_FIRE_Pin       , COMM_FIRE_GPIO_Port       }
};

extern  State_t     gStateSM;
extern  Timer*      hTimer;

static void IrqProc( void );
static void ReadFireIn(void );

static GPIO_PinState Read( eInFire_t port )
{
  return HAL_GPIO_ReadPin( gFireI[ port ].hPort, gFireI[ port ].pin );
}

static void Write(eFire_t port , GPIO_PinState st)
{
  HAL_GPIO_WritePin( gFireO[ port ].hPort, gFireO[ port ].pin, st );
}
/***
*
*/
static void FireStart( void )
{
  hTimer->Time_Out( Timer::start, TIME_OUT_PWR_ON, EV_FIRE_START );
  Write( cmd_pwr, GPIO_PIN_SET );
}       
/***
*
*/
static void FireON( void )
{
  if ( gStateSM.st.bFireOn )
  {
    //Write( cmd_fire, GPIO_PIN_SET );
    Write( cmd_lock, GPIO_PIN_SET );    
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
  Write( cmd_pwr,  GPIO_PIN_RESET );
  ///Write( cmd_fire, GPIO_PIN_RESET );
  Write( cmd_lock, GPIO_PIN_RESET );  
}//FireOFF()

/***
*  Включаем CMD_FIRE_PWR  через 1 секунду  включаем 
*  CMD_FIRE_LOCK & CMD_FIRE_FIRE держим 3 секунды и выключаем все
*
*/
void FireProcess( void )
{
  if ( 1 == gStateSM.st.bExti )
  {
    IrqProc();
  }else;
  ReadFireIn( );  
  
  if ( gStateSM.st.bFireStrat ) gStateSM.proc = ST_FIRE_START;
  if ( gStateSM.st.bFireWaite ) gStateSM.proc = ST_FIRE_WAITE;
  if ( gStateSM.st.bFireOn )    gStateSM.proc = ST_FIRE_ON;
  if ( gStateSM.st.bFireOff )   gStateSM.proc = ST_FIRE_OFF;
  
  switch( gStateSM.proc )
  {
  case ST_FIRE_START:  
    gStateSM.proc =   ST_FIRE_WAITE;
    WR_DEBUG("Fire Start \r\n");
    gMbStatus.bit.bFireStart = 1;
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );    
    FireStart();        
    gStateSM.st.bFireWaite = 1;
    gStateSM.st.bTestCurr1 = 1;
    hTimer->Time_Out( Timer::start, PA_TIME_OUT, EV_PILOT_ARC_TO );
    hTimer->Time_Out( Timer::start, TIME_FIRE_START, EV_COMM_FIRE);
    
  break;
  case ST_FIRE_WAITE: 
    if ( hTimer->IsTimeOut( EV_FIRE_START ) ) 
    {
      gStateSM.proc = ST_FIRE_ON;
      hTimer->Time_Out( Timer::start, TIME_HOLD, EV_FIRE_ON );
      WR_DEBUG("Fire ON time TIME_HOLD = %i\r\n", TIME_HOLD);
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
    gMbStatus.bit.bFireStart = 0;
    preMbCntrl.bit.bFireStart = 0;
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );  
    hTimer->Time_Out( Timer::start, TIME_OUT_FIRE_OFF, EV_FIRE_OFF );
    hTimer->Time_Out( Timer::stop, TIME_FIRE_START, EV_COMM_FIRE);
    break;
  default:;    
  }
} // FireProcess()

#define  CNT_FIRE_READ 5
void ReadStart( void )
{

    static uint16_t cnt = 0;
    if ( GPIO_PIN_SET == Read( comm_start )) 
    {
      if ( cnt++ > CNT_FIRE_READ )
      {
        cnt = 0;
        gStateSM.proc = ST_COMM_START  ;
        gStateSM.st.bExti = 0;
        gStateSM.st.bStart = 1;
      }else;
    }else
    {
      if ( cnt++ > CNT_FIRE_READ )
      {
        cnt = 0;
        gStateSM.st.bExti = 0;
        gStateSM.proc = ST_COMM_STOP;
        gStateSM.st.bStop = 1;
        //gStateSM.st.bCommStart = 0;
      }
    };
}//ReadStart()

/***
*  Обработка дребезга входов прерываний
*  
*/
static void IrqProc( void )
{

  ReadStart();
  if ( 1 == gStateSM.st.bCommFire     )  
  {
    static uint16_t cnt = 0;
    if ( GPIO_PIN_SET == Read( com_fire_in) )
    {
      if (cnt++ > CNT_FIRE_READ )
      {
        cnt = 0;
        gStateSM.st.bFireStrat = 1;
//        gStateSM.st.bCommFire = 1;
//        gStateSM.proc = ST_COMM_FIRE;
        gStateSM.st.bExti = 0;            
      }else;
    }else;
  }else;
  if ( 1 == gStateSM.st.bMetalContact )  
  {
    static uint16_t cnt = 0;
    if ( GPIO_PIN_RESET == Read( metal_contact ))
    {
      if ( cnt++ > CNT_FIRE_READ )
      {
        cnt = 0;
        gStateSM.proc = ST_METAL_CONTACT;   
        gStateSM.st.bExti = 0;
      }else;
    }else;
  }else;
  if ( 1 == gStateSM.st.bFireIn)
  {                                 // TODO
    static uint16_t cnt = 0;
    if ( GPIO_PIN_RESET == Read( fire_in ) )
    {
      if(cnt++ > CNT_FIRE_READ)
      {
        cnt = 0;
        gStateSM.st.bExti   = 0;
        //gStateSM.st.bFireIn = 0; 
        gIO.in.bit.ComFire = 1;
      }
    }else;
  }else;
}// IrqProc()

#define  CNT_FIRE_READ_PE0 25
/**
* Чтение входа Статуса ?? Поджига PE0 (FIRE_DI_0)
*/
static void ReadFireIn(void )
{
  if ( gStateSM.st.bFireIn )
  {
    gMbStatus.bit.bFireIn = 1;
    SetMBRgS( REG_R_STATUS, gMbStatus.reg ); // TODO
    gStateSM.st.bFireIn =0;
  }else
  {
    static uint16_t cnt = 0;
    if ( GPIO_PIN_SET == Read( fire_in ) )
    {
      if( cnt++ > CNT_FIRE_READ_PE0 )
      {
        cnt = 0;
        gMbStatus.bit.bFireIn = 0;
        SetMBRgS( REG_R_STATUS, gMbStatus.reg );        
      }else;
    }else;
  }	
}// ReadFireIn()

/***
*
*/
//void PilotArcPort( FlagStatus st )
//{
//  Write( cmd_pilot_arc, (GPIO_PinState)st ); 
//} // PilotArcPort()

// End of file -------------------------------
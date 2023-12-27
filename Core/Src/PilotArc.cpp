/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : PilotArc.cpp
* Compiler          : IAR EWA 8.32
* Version           : 0.0
* Created File      : 21.11.2023
* Last modified     : 21.11.2023
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU:  @ (Xtal = MHz | CPUclk = MHz)
* Description       : Дежурная дуга
*                   : 
* Hardware          : .sch .pcb

1. включение дежурной дуги
   вкючаем выхoд CNC_DO_0 ждем измерения тока в шине "-" ( ADC_CURR2 ) 
   ждем уровня 2/3 от уставки в течении 2 секунд
*
********************************************************************************/
 
/*===============================[ SPECIAL ]===================================*/ 
//  Условная компиляция
 
/*===============================[ IMPORT DECLARATIONS ]=======================*/ 

#include  "main.h"
#include  "Fire.h"
#include  "StateMachine.h"
#include  "Timer.hpp"
#include  "user_mb_app.h"
#include  "PilotArc.hpp"

extern Timer *hTimer;

/***
*
*/
PilotArc::PilotArc( )
{
  port = { CMD_FIRE_FIRE_Pin, CMD_FIRE_FIRE_GPIO_Port };
  TimeOut = 0;
  curr = 0;
  htim = hTimer;
}//PilotArc()

/**
*
*/
void PilotArc::On( void )
{
  HAL_GPIO_WritePin( port.hPort, port.pin, GPIO_PIN_SET );  
}

/**
*
*/
void PilotArc::Off( void )
{
  HAL_GPIO_WritePin( port.hPort, port.pin, GPIO_PIN_RESET );
  htim->Time_Out( Timer::clr , PA_TIME_OUT, EV_PILOT_ARC_TO );
}

/***
*
*/
void PilotArc::Proc( void )
{   
  if ( gMbCntrl.bit.bPilotArc ) // cmd_pilot_arc
  {
    if ( curr > (2/3)*(1))  // todo
      htim->Time_Out( Timer::start, PA_TIME_OUT, EV_PILOT_ARC_TO );
  }
}

PilotArc::~PilotArc()
{}

/** (END OF FILE  : PilotArc.cpp) 
*******************************/ 

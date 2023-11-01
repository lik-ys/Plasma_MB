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

/*
*
*/
void FireStart( void )
{
}
void FireON( void )
{
}
/***
*  Включаем CMD_FIRE_PWR  через 1 секунду  включаем CMD_FIRE_LOCK & CMD_FIRE_FIRE держим 3 секунды и выключаем все
*
*/
void FireProcess( void )
{
  switch( eSM_proc )
  {
  case ST_FIRE_START: FireStart(); break;
  case ST_FIRE_WAITE: break;
  case ST_FIRE_ON:    break;
  case ST_FIRE_OFF:    break;
  default:;    
  }
}


// End of file -------------------------------
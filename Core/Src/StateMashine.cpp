/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : StateMashine.cpp
* Compiler          : IAR EWA 8.32
* Version           : 0.0
* Created File      : 01.06.2023
* Last modified     : 01.06.2023
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU: f407 @ (Xtal = MHz | CPUclk = MHz)
* Description       : Главная плата управления плазмотроном
*                   : 
* Hardware          : .sch .pcb
*
********************************************************************************/

#include  "StateMashine.h"// 
#include  "Core/_Protocols/_MB/mb_com.hpp"

eProcess_t eSM_proc;

void ProcessInit( void )
{
  eSM_proc = ST_IDLE;
}// ProcessInit()

void SM_loop( void )
{  
  switch( eSM_proc )    // 
  {
    case ST_IDLE:
      break;
    case ST_START:
      break;
    case ST_MB_MASTER:
      break;
    case ST_MB_SLAVE:
      break;   
    default: break;    
  } // switch( eSM_proc )
}// SM_process()
 
/** (END OF FILE  : StateMashine.cpp) 
*******************************/ 

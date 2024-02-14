/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : CommandExec.hpp
* Compiler          : IAR EWA 8.32
* Version           : 0.0
* Created File      : 15.11.2023
* Last modified     : 15.11.2023
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU:  @ (Xtal = MHz | CPUclk = MHz)
* Description       : 
*                   : 
* Hardware          : 
*
********************************************************************************/
/*===============================[ REDEFINITION DEFENCE ]======================*/ 
#ifndef __COMMAND_HPP__ 
#define __COMMAND_HPP__

#include "Timer.hpp"
#include "StateMachine.h"
#include "Fire.h"
#include "user_mb_app.h"
/*===============================[ SPECIAL ]===================================*/ 
    // Блок операторов условной компиляции 
/*===============================[ PUBLIC CONSTANTS ]==========================*/ 
    //  Публичные константы 
/*===============================[ PUBLIC TYPES ]==============================*/ 
    // Публичные типы 
/*===============================[ FORWARD REFERENCES ]========================*/ 
    // Ссылки вперед 
/*===============================[ PUBLIC VARIABLES ]==========================*/ 
    // Публичные переменные 
/*===============================[ PUBLIC FUNCTIONS ]==========================*/ 
    // Публичные функции 

void CmdPilotArc( void );
void CmdFireStart(void);
void CmdStartStopPwm( void );
void CmdMetalContact( void );

class Command// : public Timer
{
public:
  Command();
  
  void Proc( void );

  typedef void (*pExecFunc_t)(void);

  pExecFunc_t TableExcFunc[ NUMBERS_CNTL_BIT ] = {CmdStartStopPwm , CmdPilotArc, CmdFireStart , NULL, };

private:
  
};//Command

/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 
    // Inline функции (методы класса)
/*===============================[ END REDEFINITION DEFENCE]===================*/ 
    // Окончание однократно включаемого h-файла
    
#endif 
/** (END OF FILE  : CommandExec.hpp) 
*******************************/ 

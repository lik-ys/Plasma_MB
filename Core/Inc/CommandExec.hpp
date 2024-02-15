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
void CmdFireStart( void);
void CmdStartStopPwm( void );
void CmdMetalContact( void );
void CmdTimeOut( void );
void CmdRepeat( void );
void CmdWiteCurrent( void );

  typedef enum {
    P_PILOT_ARC     ,
    P_TIME_OUT_0    ,
    P_FIRE_START    ,
    P_WAIT_CURR     ,
    P_TIME_OUT_1    ,      
    P_CMD_REPEAT    ,
    P_START_PWM     ,
    P_STOP_PWM      ,
    P_END
  }tech_proc_t;
  
#define CNT_PROC   P_END
#define CNT_REPEAT 2    
  
class Command// : public Timer
{
public:
  Command();  
  void Proc( void );
  typedef void (*pExecFunc_t)(void);

  typedef enum{    
    PILOT_ARC   ,
    FIRE_START  ,
    ON_PWM      ,
  }start_func_t;
  
  void Start(start_func_t );
  
  pExecFunc_t TableExcFunc[ NUMBERS_CNTL_BIT ] = { CmdPilotArc, CmdFireStart, CmdStartStopPwm, NULL, };
  
  // описание процесса включения 
  typedef uint8_t NumTechProc_t;
  uint8_t repeat;
  NumTechProc_t num; // 0,1,..7
  NumTechProc_t tbl[ CNT_PROC ]       = {P_PILOT_ARC, P_TIME_OUT_0, P_START_PWM ,    P_FIRE_START, P_WAIT_CURR,    P_TIME_OUT_1, P_CMD_REPEAT };
  pExecFunc_t tblThechProc[ CNT_PROC ]= {CmdPilotArc, CmdTimeOut,   CmdStartStopPwm, CmdFireStart, CmdWiteCurrent, CmdTimeOut,   CmdRepeat    };
  void TechProc( void );

private:
};//Command

/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 
    // Inline функции (методы класса)
/*===============================[ END REDEFINITION DEFENCE]===================*/ 
    // Окончание однократно включаемого h-файла
    
#endif 
/** (END OF FILE  : CommandExec.hpp) 
*******************************/ 

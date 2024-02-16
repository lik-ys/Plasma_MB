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
    // Ѕлок операторов условной компил€ции 
/*===============================[ PUBLIC CONSTANTS ]==========================*/ 
    //  ѕубличные константы 
/*===============================[ PUBLIC TYPES ]==============================*/ 
    // ѕубличные типы 
/*===============================[ FORWARD REFERENCES ]========================*/ 
    // —сылки вперед 
/*===============================[ PUBLIC VARIABLES ]==========================*/ 
    // ѕубличные переменные 
/*===============================[ PUBLIC FUNCTIONS ]==========================*/ 
    // ѕубличные функции 


void CmdPilotArcStart(void);
void CmdPilotArc( void );
void CmdFireStart( void);
void CmdStartStopPwm( void );
void CmdStartPwm(void);
void CmdMetalContact( void );
void CmdTimeOut( void );
void CmdRepeat( void );
void CmdWiteCurrent( void );

/**
*
*/
typedef enum
{
  CHOPPER_START     , // синхронное выключение €чеек от команды  bit0 в  REG_W_CNTRL_START
  PILOT_ARC         , // ¬кл/выкл дежурной дуги
  FIRE_START        , // ¬кл/выкл поджига
  
  RESERV_B__1     , // TODO
  RESERV_B_0    ,
  RESERV_B_1    ,
  RESERV_B_2    ,
  RESERV_B_3    ,
  RESERV_B_4    ,
  CHOPPER_OFF   ,
  // 15 bit  -  EEPROM_write TODO
  NUMBERS_CNTL_BIT
}eCntrlRegBits_t;//

typedef enum {
  P_START_PWM    ,
  P_PILOT_ARC_START     ,
  P_PILOT_ARC    ,
  P_TIME_OUT_0    ,
    
  P_FIRE_START    ,
  P_WAIT_CURR     ,
  P_TIME_OUT_1    ,      
  P_CMD_REPEAT    ,
  P_STOP_PWM      ,
  P_END             // 9
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
  
  pExecFunc_t TableExcFunc[ NUMBERS_CNTL_BIT ] = { CmdStartStopPwm, CmdPilotArc, CmdFireStart, NULL, NULL,NULL, NULL, NULL,NULL, CmdStartStopPwm };
  
  // описание процесса включени€ 
  typedef uint8_t NumTechProc_t;
  int8_t repeat;
  NumTechProc_t num; // 0,1,..7
  NumTechProc_t tbl[ CNT_PROC ]       = {P_PILOT_ARC_START, P_TIME_OUT_0,     P_PILOT_ARC,  P_START_PWM, P_FIRE_START, P_WAIT_CURR,   P_TIME_OUT_1, P_CMD_REPEAT };
  pExecFunc_t tblThechProc[ CNT_PROC ]= {CmdStartPwm,       CmdPilotArcStart, CmdPilotArc,  CmdTimeOut,  CmdFireStart, CmdWiteCurrent,CmdTimeOut,   CmdRepeat    };
  void TechProc( void );
  void InitTechProc(void );

private:
};//Command

/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 
    // Inline функции (методы класса)
/*===============================[ END REDEFINITION DEFENCE]===================*/ 
    // ќкончание однократно включаемого h-файла
    
#endif 
/** (END OF FILE  : CommandExec.hpp) 
*******************************/ 

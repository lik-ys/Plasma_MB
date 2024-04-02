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


void CmdPilotArcStart(void);
void CmdPilotArc( void );
void CmdFireStart( void);
void CmdStartStopPwm( void );
void CmdStartPwm(void);
void CmdMetalContact( void );
void CmdTimeOut( void );
void CmdRepeat( void );
void CmdWiteCurrent( void );
void CmdMonitor(void);
void CmdStopPwm( void );
void CmdTestShortCurr(void );

/**
*
*/
typedef enum
{
  CHOPPER_START     , // 0 синхронное выключение ячеек от команды  bit0 в  REG_W_CNTRL_START
  PILOT_ARC         , // 1 Вкл/выкл дежурной дуги
  FIRE_START        , // 2 Вкл/выкл поджиг  
  RESERV_B__1       , // 3 TODO
  RESERV_B_0        , // 4 
  RESERV_B_1        , // 5
  RESERV_B_2        , // 6
  RESERV_B_3        , // 7
  RESERV_B_4        , // 8
  CHOPPER_OFF       , // 9
  // 15 bit  -  EEPROM_write TODO
  NUMBERS_CNTRL_BIT   // 10
}eCntrlRegBits_t;//

typedef enum {  
  // P_TEST_SHORT_CURR , // Проверка короткого замыкания 
  P_PILOT_ARC_START , // 0
  P_PILOT_ARC       , // 1
  P_TIME_OUT_0      , // 2
  P_TEST_SHORT_CURR , // 3
  //P_START_PWM       , // 4  
  P_FIRE_START      , // 5
  P_WAIT_CURR       , // 6
  P_TIME_OUT_1      , // 7   
  P_CMD_REPEAT      , // 8
  P_CURR_MONITOR    , // 9 - процесс реза, мониторим ток, через 1сек выключаем дежурку, если обрыв тока - выключаем ШИМ
  P_STOP_PWM        , // 10
  P_END               // 11
}tech_proc_t;
  
#define CNT_PROC   P_END
#define CNT_REPEAT 3    
  
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
  
  const pExecFunc_t TableExcFunc[ NUMBERS_CNTRL_BIT ] = { CmdStartStopPwm, CmdPilotArc, CmdFireStart, NULL, NULL,NULL, NULL, NULL,NULL, CmdStartStopPwm };
  
  // описание процесса включения 
  typedef uint8_t NumTechProc_t;
  int8_t repeat;
  NumTechProc_t num; // 0,1,..7
  const NumTechProc_t tbl[ CNT_PROC ]       = {P_PILOT_ARC_START,  P_PILOT_ARC,   P_TIME_OUT_0, P_TEST_SHORT_CURR, P_FIRE_START, P_WAIT_CURR,   P_TIME_OUT_1, P_CMD_REPEAT,  P_CURR_MONITOR,  P_STOP_PWM };
  const pExecFunc_t tblThechProc[ CNT_PROC ]= {CmdPilotArcStart,   CmdPilotArc,   CmdTimeOut,   CmdTestShortCurr,  CmdFireStart, CmdWiteCurrent,CmdTimeOut,   CmdRepeat,    CmdMonitor, CmdStopPwm };       // CmdStartPwm
  void TechProc( void );
  void InitTechProc(void );

private:
};//Command

/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 
    // Inline функции (методы класса)
/*===============================[ END REDEFINITION DEFENCE]===================*/ 
    // Окончание однократно включаемого h-файла
    
#endif 
/** (END OF FILE  : CommandExec.hpp) 
*******************************/ 

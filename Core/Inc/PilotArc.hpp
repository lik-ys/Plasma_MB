/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : PilotArc.hpp
* Compiler          : IAR EWA 8.32
* Version           : 0.0
* Created File      : 21.11.2023
* Last modified     : 21.11.2023
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU:  @ (Xtal = MHz | CPUclk = MHz)
* Description       : 
*                   : 
* Hardware          : .sch .pcb
*
********************************************************************************/
/*===============================[ REDEFINITION DEFENCE ]======================*/ 
#ifndef __PILOT_ARC_HPP__ 
#define __PILOT_ARC_HPP__

#include    "io_process.h"
#include    "main.h"

#define PA_TIME_OUT  2000

class PilotArc //: public Timer
{
public: 
  PilotArc(void );
  
  Timer * htim;
  port_t    port;     // выод управлени€ деж. дугой
  uint16_t  TimeOut;  // врем€ ожидан€ 
  uint16_t  curr;  // уровень напр€жени€ 
  
  void On( void );
  void Off(void );
  void Proc( void );
  
  ~PilotArc();
private:
};
    
#endif 
/** (END OF FILE  : PilotArc.hpp) 
*******************************/ 

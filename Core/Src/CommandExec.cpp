/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : CommandExec.cpp
* Compiler          : IAR EWA 8.32
* Version           : 0.0
* Created File      : 15.11.2023
* Last modified     : 15.11.2023
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU:  @ (Xtal = MHz | CPUclk = MHz)
* Description       : Command funcs executing
*                   : 
* Hardware          : 
*
********************************************************************************/
 
/*===============================[ SPECIAL ]===================================*/ 

#include  "CommandExec.hpp" 
#include  "PilotArc.hpp"

PilotArc gPilotArc;
PilotArc *hPilotArc = &gPilotArc;

Command::Command( void )
{
  //pExecFunc_t TableExcFunc[] = {CmdFireStart, CmdPilotArc};
}

/**
* 
*/
void Command :: Proc( void )
{  
  static int num = 0;
  if ( gMbActiveCntrl.reg & (1 << num ) )
  { 
    if ( NULL != TableExcFunc[num] ) TableExcFunc[num]();
  }else;
  if ( num++ >= NUMBERS_CNTL_BIT ) num = 0;  
}

// command functions
/**
*   Дежурная дуга
*/
void  CmdPilotArc(void )
{  
  if ( gMbCntrl.bit.bPilotArc ) // cmd_pilot_arc
  {
    WR_DEBUG("PILOT_ARC SET \r\n");
    //PilotArc( SET );
    
    gMbStatus.bit.bPilotArc = 1; 
    // timer start
    
  }else
  {
    WR_DEBUG("PILOT_ARC RESET \r\n");    
    //PilotArc( RESET );
    gMbStatus.bit.bPilotArc = 0;     
  } 
  SetMBRgS( REG_R_STATUS_S, gMbStatus.reg );
  gMbActiveCntrl.bit.bPilotArc = 0;
} //CmdPilotArc()

/**
*   Поджиг
*/
void  CmdFireStart( void )
{
  WR_DEBUG("FIRE_START \r\n");  
  if ( gMbCntrl.bit.bFireStart ) // получили команду "поджиг "от МБ
  {
    gMbCntrl.bit.bFireStart  = 0;
    SetMBRgS( REG_W_CNTRL, gMbCntrl.reg );
    gStateSM.st.bCommFire = 1; 
    gStateSM.proc = ST_COMM_FIRE;
    gStateSM.st.bFireStrat = 1;
  }else;
  gMbActiveCntrl.bit.bFireStart = 0;  
}

/**
*  TODO
*/
void MetalContact( void )
{
  static int cnt2 = 0;
  WR_DEBUG("DI_METAL_CONTACT_Pin:  cnt = %i \r\n", cnt2++);      
  gStateSM.st.bMetalContact = 0;
} // MetalContact( )
 
/** (END OF FILE  : CommandExec.cpp.cpp) 
*******************************/ 

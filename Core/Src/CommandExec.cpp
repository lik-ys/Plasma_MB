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
extern Timer *hTimer;

Command  gCmd;
Command* hCmd = &gCmd;

Command::Command( void )
{
  num = 0;
  repeat = 0;
}

void Command::Start( start_func_t start )
{
  if ( NULL != TableExcFunc[start] ) TableExcFunc[start]();
}
/**
* 
*/
void Command:: Proc( void )
{  
  static int num = 0;
  if ( gMbActiveCntrl.reg & (1 << num ) )
  { 
    if ( NULL != TableExcFunc[num] ) TableExcFunc[num]();
    gMbActiveCntrl.reg  &= ~( 1 << num );
  }else;
  if ( num++ >= NUMBERS_CNTL_BIT ) num = 0;  
}// Proc()

/**
* 
*/
void Command::TechProc(void)
{
   if (gMbStatus.bit.bStartCNC) tblThechProc[tbl[num]]();   
}// TechProc()

// command functions
/**
*   Дежурная дуга
*/
static
void  CmdPilotArc(void )
{  
  if ( gMbCntrl.bit.bPilotArc ) // cmd_pilot_arc
  {
    WR_DEBUG("PILOT_ARC SET \r\n");
    //PilotArc( SET );    
    hPilotArc->On();
    gMbStatus.bit.bPilotArc = 1; 
    // timer start    
  }else
  {
    WR_DEBUG("PILOT_ARC RESET \r\n");    
    //PilotArc( RESET );
    gMbStatus.bit.bPilotArc = 0; 
    hPilotArc->Off();
  } 
  SetMBRgS( REG_R_STATUS, gMbStatus.reg );
  gMbActiveCntrl.bit.bPilotArc = 0;
  hCmd->num = P_PILOT_ARC;
  hTimer->Time_Out( Timer::start, PA_TIME_OUT, EV_PILOT_ARC_TO );
} //CmdPilotArc()

/**
*   Поджиг
*/
static void  CmdFireStart( void )
{
  WR_DEBUG("FIRE_START \r\n");  
  if ( gMbCntrl.bit.bFireStart ) // получили команду "поджиг "от МБ
  {
    gMbCntrl.bit.bFireStart  = 0;
    SetMBRgS( REG_W_CNTRL, gMbCntrl.reg );
    gStateSM.st.bCommFire = 1; 
    gStateSM.proc = ST_COMM_FIRE;
    gStateSM.st.bFireStrat = 1;
    UpateActiveRg();
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );    
  }else;
  gMbActiveCntrl.bit.bFireStart = 0;  
}// CmdFireStart()

/**
*  TODO
*/
static 
void CmdWiteCurrent(void )
{
  if ( GetMBRgS( REG_R_CURR_1)/10 > THRESHOLD_CURR_1 )  //
  {
    gStateSM.st.bIgnitionOk = 1;
    CncWrite( cnc_out0, GPIO_PIN_SET );   
    hCmd->num = P_END;
  }else
  { 
    if (hTimer->IsTimeOut( EV_COMM_FIRE ))
    {
      hCmd->num = P_CMD_REPEAT;
      hCmd->repeat = 0;
    }else;    
  }
}

/**
*  TODO
*/
static
void CmdRepeat( void )
{
  if ( CNT_REPEAT < hCmd->repeat-- )
  {
    hCmd->num = P_FIRE_START;
    WR_DEBUG("P_FIRE_START \r\n");
  }else
  {
    hCmd->num = P_END;
    WR_DEBUG("P_END \r\n");
  }
}
/**
*
*/
static
void CmdTimeOut( void )
{
  switch( hCmd->num )    // 
  {
    case P_PILOT_ARC:   hCmd->num = P_TIME_OUT_0;
      break;
    case P_TIME_OUT_0:  
      if (hTimer->IsTimeOut( EV_PILOT_ARC_TO )) hCmd->num = P_FIRE_START;
      break;
    case P_FIRE_START:  hCmd->num = P_WAIT_CURR;
      break;
    case P_WAIT_CURR:   hCmd->num = P_TIME_OUT_1;
      break;
    case P_TIME_OUT_1:  hCmd->num = P_CMD_REPEAT;
      break; 
    case P_CMD_REPEAT:  
      if ( hCmd->repeat < CNT_REPEAT ) hCmd->num = P_START_PWM;
      else  hCmd->num = P_START_PWM;
      break;
    case P_START_PWM:
      break;
    default:
      break;
  } // switch(  )
}

/**
*  TODO
*/
static
void CmdMetalContact( void )
{
  static int cnt2 = 0;
  WR_DEBUG("DI_METAL_CONTACT_Pin:  cnt = %i \r\n", cnt2++);      
  gStateSM.st.bMetalContact = 0;
} // CmdMetalContact( )

/**
* включение-выключение синхрочастоты
*/
static
void CmdStartStopPwm( void )
{
  if ( gMbCntrl.bit.bChopperStart )
  {
    pExtSync->Instance->CNT = 0;
    HAL_TIM_PWM_Start( pExtSync, TIM_CHANNEL_1 ); 
    gMbStatus.bit.bOnOffPwr = 1;
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );
  }else
  {
    pExtSync->Instance->CNT = 0;
    HAL_TIM_PWM_Stop( pExtSync, TIM_CHANNEL_1 );
    gMbStatus.bit.bOnOffPwr = 0;
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );
  }  
}//StartPwm()

/** (END OF FILE  : CommandExec.cpp.cpp) 
*******************************/ 

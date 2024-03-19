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
  repeat = CNT_REPEAT;
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
  static int n = 0;
  if ( gMbStatus.bit.bStartCNC ) return;
  if ( gMbActiveCntrl.reg & (1 << n ) )
  { 
    if ( NULL != TableExcFunc[n] ) TableExcFunc[n]();
    gMbActiveCntrl.reg  &= ~( 1 << n );
  }else;
  if ( n++ >= NUMBERS_CNTRL_BIT ) n = 0;  
}// Proc()

void Command::InitTechProc(void)
{
  gMbStatus.bit.bStartCNC = 0;
  num = 0;
  repeat = CNT_REPEAT;
  gMbStatus.bit.bOnOffPwr = 0;
}
/**
* 
*/
void Command::TechProc(void)
{
  //if ( gActiveReg.rg ) return;  // BUG: блокирует код - активные биты не кончаются
  if ( gMbStatus.bit.bStartCNC ) {
    if ( 0 >= repeat )
    {
      gMbStatus.bit.bStartCNC = 0;
      num = 0;
    }
    else 
      if ((num < P_END)&&(tblThechProc[num]) != NULL ) 
        tblThechProc[num]();   
  }else{}
  // Выключение напряжения по завершении процесса резки: ток 0 - выкл ШИМ
  if ( 1 == gStateSM.st.bIgnitionOk )
  {
    // wait end current
    if ( GetMBRgS( REG_R_CURR_1 ) < THRESHOLD_CURR_OFF )
    { 
      CmdStopPwm(); 
      gStateSM.st.bIgnitionOk = 0;
    }else;
  }
}// TechProc()

static
void  CmdPilotArcStart(void )
{
    WR_DEBUG("--1-- PilotARC Timer Start on TIME_START = %i s. \r\n",TIME_START/1000);
    gMbCntrl.bit.bPilotArc = 1;  // Деж. дуга
    gMbActiveCntrl.bit.bPilotArc = 1;
    hTimer->Time_Out( Timer::start, TIME_START, EV_COMM_START); 
    hCmd->num = P_PILOT_ARC;
}
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
    hPilotArc->On();
    gMbStatus.bit.bPilotArc = 1;
    hCmd->num = P_TIME_OUT_0; 
    // timer start    
    gMbCntrl.bit.bOnOffPwr = 1; 
    gMbCntrl.bit.bPilotArc = 0;
  }else
  {
    WR_DEBUG("PILOT_ARC RESET \r\n");    
    gMbStatus.bit.bPilotArc = 0; 
    hPilotArc->Off();
  } 
  SetMBRgS( REG_R_STATUS, gMbStatus.reg );
  gMbActiveCntrl.bit.bPilotArc = 0;
    

} //CmdPilotArc()

/**
*   Поджиг
*/
static void  CmdFireStart( void )
{  
  if ( 0 == gMbStatus.bit.bStartCNC ) return;
  gStateSM.st.bFireStrat = 1;  // -> FireProcess() -> ST_FIRE_START:
  WR_DEBUG("FIRE_START \r\n");
  hCmd->num = P_WAIT_CURR;
  UpateActiveRg();
  SetMBRgS( REG_R_STATUS, gMbStatus.reg );  
}// CmdFireStart()

/**
*  Ждем ток THRESHOLD_CURR_1 -> готовность ЧПУ
*/
static 
void CmdWiteCurrent(void )
{
  static int16_t cnt = 3;
  if ( GetMBRgS( REG_R_CURR_1) > THRESHOLD_CURR_1 )  //
  {
    gStateSM.st.bIgnitionOk = 1;
    CncWrite( cnc_out0, GPIO_PIN_SET );  // Готовность для ЧПУ 
    hCmd->num = P_CURR_MONITOR;
    hTimer->Time_Out( Timer::start, PILOT_ARC_OFF_TO, EV_IGNITION );    
  }else
  { 
    if ( hTimer->IsTimeOut( EV_COMM_FIRE ))
    {
      if (cnt> 0 )cnt--;
      else {
        hCmd->num = P_END; 
        cnt = 3;
        return;
      }
      hCmd->num = P_CMD_REPEAT;  // -> CmdRepeat()
    }else;    
  }
}//CmdWiteCurrent()

/**
*  TODO
*/
static
void CmdRepeat( void )
{
  if ( 0 == gStateSM.st.bFireOff ){
    if ( hCmd->repeat > 0 )
    {
      hCmd->repeat--;
      hCmd->num = P_FIRE_START;  // -> CmdFireStart()
      gMbCntrl.bit.bFireStart = 1;
      WR_DEBUG("----- repeat fire start\r\n");
    }else
    {
      hCmd->num = P_END;
      WR_DEBUG("P_END \r\n");
    }
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
    case P_PILOT_ARC_START:   
      hCmd->num = P_TIME_OUT_0;
      break;
    case P_TIME_OUT_0:  
      if ( hTimer->IsTimeOut( EV_COMM_START ) ) {
        hCmd->num = P_START_PWM;     
        WR_DEBUG("-------- TimeOut is EV_COMM_START \r\n");
      }
      break;
    case P_PILOT_ARC:
      hCmd->num = P_FIRE_START;
      break;
    case P_FIRE_START:  
      hCmd->num = P_WAIT_CURR;
      break;
    case P_WAIT_CURR:   hCmd->num = P_TIME_OUT_1;
      WR_DEBUG("-------- P_WAIT_CURR \r\n");
      break;
    case P_TIME_OUT_1:  
      WR_DEBUG("-------- TimeOut is P_TIME_OUT_1 \r\n");
      hCmd->num = P_CMD_REPEAT;
      break; 
    case P_CMD_REPEAT:        
      WR_DEBUG("-------- P_CMD_REPEAT \r\n");
      hCmd->num = P_CMD_REPEAT;
      break;
    case P_START_PWM:
      hCmd->num =  P_FIRE_START;
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
  if ( 1 == gMbCntrl.bit.bFireStart ) return;
  if ( gMbCntrl.bit.bChopperStart )
  {
    pExtSync->Instance->CNT = 0;
    HAL_TIM_PWM_Start( pExtSync, TIM_CHANNEL_1 ); 

    gMbCntrl.bit.bOnOffPwr = 1;

    gActiveReg.rgPWM =1; // т.к. Ячейки ресетим после останова ШИМ, закиним уставки
    gActiveReg.rgSLOP_1 = 1;
    gActiveReg.rgSLOP_2 = 1;    

    hCmd->num = P_TIME_OUT_0;
    gMbCntrl.bit.bFireStart = 1;
  }else if (1 == gMbStatus.bit.bOnOffPwr )
  {
    pExtSync->Instance->CNT = 0;
    HAL_TIM_PWM_Stop( pExtSync, TIM_CHANNEL_1 );
    gMbStatus.bit.bOnOffPwr = 0;
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );
    //hCmd->num = P_END;
  }   
}//StartPwm()

/**
* включение синхрочастоты
*/
static
void CmdStartPwm( void )
{
  if ( hCmd->repeat <= 0) 
  {
    hCmd->num = P_END;  return;
  }
    gActiveReg.rgCNTRL = 1;   // эмуляция работы МБ ВУ
    gActiveReg.rgPWM =1;
    //gActiveReg.rgP = 1;
    //gActiveReg.rgI = 1;    
    //gActiveReg.rgD = 1;
    //gActiveReg.rgCURR = 1;
    gActiveReg.rgSLOP_1 = 1;
    gActiveReg.rgSLOP_2 = 1;
    
    gMbCntrl.bit.bOnOffPwr = 1;
    gMbCntrl.bit.bChopperStart = 1;
    UpateActiveRg(); 
    SetMBRgS(REG_W_CNTRL, gMbCntrl.reg);    
    pExtSync->Instance->CNT = 0;
    HAL_TIM_PWM_Start( pExtSync, TIM_CHANNEL_1 ); 

    if (0 == gMbStatus.bit.bOnOffPwr) {
      hTimer->Time_Out( Timer::start, PA_TIME_OUT, EV_PILOT_ARC_TO );
      gMbStatus.bit.bOnOffPwr = 1;
      SetMBRgS( REG_R_STATUS, gMbStatus.reg ); 
    }
    
    ///if (Timer::start != hTimer->GetState(EV_PILOT_ARC_TO)) gMbStatus.bit.bOnOffPwr = 0;
   
    gMbCntrl.bit.bPilotArc = 1;  
   
    if ( hTimer->IsTimeOut( EV_PILOT_ARC_TO ) )
    {
      hCmd->num = P_FIRE_START;
    }    
}//StartPwm()
/**
* выключение синхрочастоты
*/
static
void CmdStopPwm( void )
{
    
    gMbCntrl.bit.bOnOffPwr = 0;
    gMbCntrl.bit.bChopperStart = 0;
    SetMBRgS(REG_W_CNTRL, gMbCntrl.reg); 
    UpateActiveRg(); // эмуляция работы черезе МБ
    
    pExtSync->Instance->CNT = 0;
    HAL_TIM_PWM_Stop( pExtSync, TIM_CHANNEL_1 );
    gMbStatus.bit.bOnOffPwr = 0;
    gMbStatus.bit.bChopperStart = 0;
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );
    hCmd->num = P_END;
    gMbCntrl.bit.bPilotArc =0;
}//StartPwm()

/**
*   процесс реза, мониторим ток, через 1сек выключаем дежурку, если обрыв тока - выключаем ШИМ
*/
static void CmdMonitor(void )
{
  if ( 1 == gMbCntrl.bit.bPilotArc)
  {
    if ( hTimer->IsTimeOut( EV_IGNITION ) )  // PILOT_ARC_OFF_TO
    {
      gMbCntrl.bit.bPilotArc = 0;
      CmdPilotArc( ); 
    }
  }
  
}
/** (END OF FILE  : CommandExec.cpp.cpp) 
*******************************/ 

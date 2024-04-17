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
    if ( 0 >= repeat && (num != P_CURR_MONITOR) )
    {
      //gMbStatus.bit.bStartCNC = 0;
      num = P_CURR_MONITOR;
    }
    else 
      if ((num < P_END)&&(tblThechProc[num]) != NULL ) {
        tblThechProc[num]();   
        if ( num < P_END ) gActiveReg.rgProcess = 1;
      }
  }else{}
  // Выключение напряжения по завершении процесса резки: ток 0 - выкл ШИМ
  if ( 1 == gStateSM.st.bIgnitionOk )
  {
    // wait end current
    if (hTimer->IsTimeOut( EV_IGNITION1) )
    if ( PhParam.Current1 < THRESHOLD_CURR_OFF )
    { 
      //CmdStopPwm(); //- TODO : выключение стаутса готовности ЧПУ что бы он остановился 
      gStateSM.st.bIgnitionOk = 0;
      //CncWrite( cnc_redy, GPIO_PIN_RESET);
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
    ///gMbCntrl.bit.bOnOffPwr = 1; 
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
  if ( 1 == gMbStatus.bit.bStartCNC || 1 == gMbCntrl.bit.bFireStart )
  {
    gStateSM.st.bFireStrat = 1;  // -> FireProcess() -> ST_FIRE_START:
    WR_DEBUG("FIRE_START \r\n");
    hCmd->num = P_WAIT_CURR;
    UpateActiveRg();
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );  
  }else return;
}// CmdFireStart()

/**
*  Ждем ток THRESHOLD_CURR_1 -> готовность ЧПУ
*/
static 
void CmdWiteCurrent(void )
{
  if ( PhParam.Current1 > THRESHOLD_CURR_1 )  //
  {
    gStateSM.st.bIgnitionOk = 1;
    CncWrite( cnc_redy, GPIO_PIN_SET );  // Готовность для ЧПУ 
    hCmd->num = P_CURR_MONITOR;
    hTimer->Time_Out( Timer::start, PILOT_ARC_OFF_TO, EV_IGNITION  ); 
    hTimer->Time_Out( Timer::start, PILOT_ARC_OFF_TO1, EV_IGNITION1);
  }else
  { 
    if ( hTimer->IsTimeOut( EV_FIRE_OFF ))
    {
      if (hCmd->repeat <= 0 )
      {
        hCmd->num = P_END; 
        return;
      }
      // Выключить поджиг и повторить включение
      hCmd->num = P_CMD_REPEAT;  // -> CmdRepeat()
    }else;    
  }
}//CmdWiteCurrent()

/**
*  TODO  BUG - первый раз выполняется второй раз нет
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
      hCmd->repeat = P_CMD_REPEAT;
    }
  }
}//CmdRepeat()
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
        hCmd->num = P_TEST_SHORT_CURR;     
        WR_DEBUG("-------- TimeOut is EV_COMM_START \r\n");
      }
      break;
    case P_PILOT_ARC:
      hCmd->num = P_FIRE_START;
      break;
    case P_FIRE_START:  
      hCmd->num = P_WAIT_CURR;
      break;
    case P_WAIT_CURR:   
      hCmd->num = P_TEST_SHORT_CURR;
      //hCmd->num = P_TIME_OUT_1;
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
    ///case P_START_PWM:
      ///hCmd->num =  P_FIRE_START;
      ///break;
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
    gActiveReg.rgCNTRL  = 1;
    gActiveReg.rgPWM = 1; // т.к. Ячейки ресетим после останова ШИМ, закиним уставки
    gActiveReg.rgSLOP_1 = 1;
    gActiveReg.rgSLOP_2 = 1;    

    hCmd->num = P_TIME_OUT_0;
    gMbCntrl.bit.bFireStart = 1;
  }else if (1 == gMbStatus.bit.bOnOffPwr )
  {
//    pExtSync->Instance->CNT = 0;
//    HAL_TIM_PWM_Stop( pExtSync, TIM_CHANNEL_1 );
    gMbStatus.bit.bOnOffPwr = 0;
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );
    gActiveReg.rgCNTRL  = 1;
    //hCmd->num = P_END;
  }   
}//StartPwm()

/*
  Проверка включения ШИМ на всех явчейках
*/
static 
uint16_t PwmOnAllCells( void )
{    
  uint16_t bPwm = 1;
  if ( gCells.bit.bCell_1 )
    if (0 == gMbSlaveSt[0].bit.bOnOffPwr ) bPwm = 0;
  if ( gCells.bit.bCell_2 )
    if (0 == gMbSlaveSt[1].bit.bOnOffPwr ) bPwm = 0;
  if ( gCells.bit.bCell_3 )
    if (0 == gMbSlaveSt[2].bit.bOnOffPwr ) bPwm = 0;
  if ( gCells.bit.bCell_4 )
    if (0 == gMbSlaveSt[3].bit.bOnOffPwr ) bPwm = 0;
  if ( gCells.bit.bCell_5 )
    if (0 == gMbSlaveSt[4].bit.bOnOffPwr ) bPwm = 0;
  if ( gCells.bit.bCell_6 )
    if (0 == gMbSlaveSt[5].bit.bOnOffPwr ) bPwm = 0;  
  return bPwm;
}//PwmOnAllCells()

#define SHORT_CURR_TO       3000 //  время проверки короткого замыкания 
#define OPEN_CIRCUIT_C      5    //  тока ХХ
#define OPEN_CIRCUIT_V      590  //  напр. ХХ
#define TEST_PWM_SET        10
#define DEF_PWM_START   90 // 50% - стартовый ШИМ по-умолчанию, от него разварачиваем до уставки ШИМ
static uint16_t sPwm ;
/*  // TODO
0 - включить дежурку CmdPilotArc()
1 - подать ШИМ 10 на 1 сек
2 - ток есть -> КЗ
3 - тока нет ОК
*/
static 
void CmdTestShortCurr( void )
{
  static uint16_t st = 0; 
  WR_DEBUG("--0-- CmdTestShortCurr() time= %i s. \r\n",SHORT_CURR_TO/1000);
  
  if ( 1 == gMbStatus.bit.bShortCircuit ) return;
  // дежурка включена - включаем шим
  if ( (0 == st) && (gMbStatus.bit.bPilotArc) )  st = 1;
  
  switch(st)
  {
  case 1:
    gActiveReg.rgPWM = 1;  
    sPwm = GetMBRgS( REG_W_PWM );
    SetMBRgS( REG_W_PWM, TEST_PWM_SET );
    st = 2;
    break;
  case 2:   // ожидание появления напряжения 
    if (0 == gActiveReg.rgPWM)
    {
      st = 3;
      CmdStartPwm();
      hTimer->Time_Out( Timer::start, SHORT_CURR_TO, EV_TEST_SHORT_CICUT );      
    }else; 
    
    break;
  case 3:  // - измеряем ток
    if ( hTimer->IsTimeOut(EV_TEST_SHORT_CICUT) )
    {
      ///if ( 1 )/////DBG!!! 
      if ( PhParam.Current1 <=  OPEN_CIRCUIT_C && PhParam.Voltage >= OPEN_CIRCUIT_V )
      {
        st = 4;
        gMbStatus.bit.bShortCircuit = 0;
        gActiveReg.rgPWM = 1;
        SetMBRgS( REG_W_PWM, DEF_PWM_START );   // TODO дождаться когда уставка дойдет      
      }
      else{
        st = 0;
        hCmd->num = P_END;  
        gMbStatus.bit.bShortCircuit = 1; 
        CmdStopPwm();
        gMbCntrl.bit.bPilotArc = 0;
        CmdPilotArc( );
      }
      SetMBRgS( REG_R_STATUS, gMbStatus.reg );
    }else;    
    break;
    case 4: // ждем выставления 50% ШИМа
      if ( 0 == gActiveReg.rgPWM )  
      {
        st = 0; 
        hCmd->num = P_FIRE_START;  // TODO BUG не успевает доходить 50 %
      }      
    break;
  }//switch()
}//CmdTestShortCurr()

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
    if (0 == gMbStatus.bit.bOnOffPwr) {  
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
      hTimer->Time_Out( Timer::start, PA_TIME_OUT, EV_PILOT_ARC_TO );
      gMbStatus.bit.bOnOffPwr = 1;
      SetMBRgS( REG_R_STATUS, gMbStatus.reg ); 
    }
    if ( PwmOnAllCells() )
    {
      hCmd->num = P_FIRE_START;      
    }    
    if ( hTimer->IsTimeOut( EV_PILOT_ARC_TO ) )
    {       
      gMbCntrl.bit.bPilotArc = 1; 
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
    gActiveReg.rgCNTRL = 1;
//    pExtSync->Instance->CNT = 0;
//    HAL_TIM_PWM_Stop( pExtSync, TIM_CHANNEL_1 );
    gMbStatus.bit.bOnOffPwr = 0;
    gMbStatus.bit.bChopperStart = 0;
    SetMBRgS( REG_R_STATUS, gMbStatus.reg );
    hCmd->num = P_END;
    gMbCntrl.bit.bPilotArc =0;
}//StartPwm()

/**
*   процесс реза, мониторим ток, через (hPilotArc->time_out_off) mсек выключаем дежурку, 
*   если обрыв тока - выключаем ШИМ
*/
static void CmdMonitor(void )
{
  if ( 1 == gMbStatus.bit.bPilotArc)
  {
    if ( hTimer->IsTimeOut( EV_IGNITION ) )  // PILOT_ARC_OFF_TO
    {
      gMbCntrl.bit.bPilotArc = 0;
      CmdPilotArc( ); 
    }
  }
  if ( PhParam.Current1 <  50 )
  {
    CmdStopPwm();
    CncWrite( cnc_redy, GPIO_PIN_RESET ); // снимаем "готовность чпу"
    gMbCntrl.bit.bPilotArc = 0;
    CmdPilotArc(); 
  }else
  {
    static int16_t cnt = 31;
    if (--cnt == 30)
    {
      gActiveReg.rgPWM = 1;  
      SetMBRgS( REG_W_PWM, sPwm );    
    }
    if (cnt < 0) cnt = 31;
  }
}
/** (END OF FILE  : CommandExec.cpp.cpp) 
*******************************/ 

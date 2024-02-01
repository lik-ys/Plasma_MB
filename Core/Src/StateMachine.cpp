/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : StateMaСЃhine.cpp
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
TODO сделать поджиг 
********************************************************************************/

#include  <stdint.h>

#include  "StateMachine.h"// 
#include  "mb_com.hpp"
#include  "mb_m.h"
#include  "io_process.h"
#include  "main.h"
#include  "user_mb_app.h"
#include  "Fire.h"
#include  "Timer.hpp"
#include  "CommandExec.hpp"
#include  "PilotArc.hpp"

Command  gCmd;
Command* hCmd = &gCmd;

Timer gTimer;
Timer *hTimer = &gTimer;

extern void     SetMBRgS( eMBRegS_t numMBReg, uint16_t data );
extern PilotArc *hPilotArc;

eProcess_t  eSM_proc;
State_t     gStateSM = { TIME_10ms, TIME_100ms, TIME_1000ms, ST_IDLE, {0,0,0},{0,0,} };

RgCntrl_t   gMbCntrl   = {0,};
RegStatus_t gMbSt      = {0,};
RgCntrl_t   preMbCntrl = {0,};
RgCntrl_t   gMbActiveCntrl = {0,};
RgCntrl_t   gMbStatus  = {0,};

ADC_data_t ADCdata[ ADC_BUF_LENGHT ] = {0,0,};
ADC_data_t ADCdat =  {0,0,0};

HAL_StatusTypeDef 	HAL_status;

uint8_t bufTx[3] =  {0x55, 0x55, 0x55 };

extern ModBusCom *pMBcntrl;
extern ModBusCom *pMBhl;
#ifdef __cplusplus
extern "C" {
#endif
extern TIM_HandleTypeDef* pExtSync   ;
#ifdef __cplusplus
}
#endif

void ADC_Process( void );
void SM_Tick( void );


/***
**
*/
void ProcessInit( void )
{
  memcpy( gStateSM.tick, 0, sizeof(gStateSM.tick) );
  
  eSM_proc = ST_IDLE;
  gLed.led = led1_pin;
  
  pMBhl->Init(); 
  pMBcntrl->Init();
  
  hTimer->Time_Out( Timer::start, TIME_OUT_TEST, PROC_EV_DEBUG );
  
 //gMbCntrl.bit.bChopperStart = 1;
  //CmdStartStopPwm( );

    
}// ProcessInit()

/**
** 
*/
void SM_loop( void )
{  
  pMBhl->Loop( );
  pMBcntrl->Loop();
  
  CHAR data  = 0;
  CHAR * pdata = &data;
  
  SM_Tick();
  if ( 1 == gStateSM.st.bAdcCmplt){
    gStateSM.proc = ST_ADC_CMPLT;
  }else;  
  //if ( 1 == gStateSM.st.bToggleLed )  gStateSM.proc =  ST_TOGGLE_LED;
  
  hPilotArc->Proc();
  
  switch( gStateSM.proc )    // 
  {
    case ST_IDLE:
      break;      
    case ST_START:
      break;
    case ST_TOGGLE_LED:
      //RS485_Dir_m( tx );
      //xMBMasterPortSerialPutByte( 0x55 ); // работает 
     // RS485_Dir_m( rx );
     // xMBPortSerialGetByte(pdata);
      ToggleLed( &gLed );      
      gStateSM.st.bToggleLed = 0;
      gStateSM.proc = ST_IDLE;    
    break;
      
    case ST_MB_MASTER:
      eSM_proc = ST_IDLE;
      break;
    case ST_MB_SLAVE:
      eSM_proc = ST_IDLE;
      break;   
    case ST_TX:
      eSM_proc = ST_IDLE;
      break;
    case ST_ADC_CMPLT:
      ADC_Process( );
      eSM_proc = ST_IDLE;
      break;      
    default: break;    
  } // switch( eSM_proc )
}// SM_process()
 
/**
**
*/
void HAL_IncTick( void )
{
 // static uint32_t preTick = 0;
  
  uwTick += uwTickFreq;    
  if ( 0 == gStateSM.div10 )
  {    
    gStateSM.div10 = TIME_10ms;
    gStateSM.time.b10ms = 1;
  }else {
    gStateSM.div10--;    
  }
  if ( 0 == gStateSM.div100 )
  {
    gStateSM.div100 = TIME_100ms;
    gStateSM.time.b100ms = 1;
  }else {
    gStateSM.div100--;    
  }  
  if ( 0 == gStateSM.div1000 )
  {
    gStateSM.div1000 = TIME_1000ms;
    gStateSM.time.b1000ms = 1;  
  }else {
    gStateSM.div1000--;    
  }  
} // HAL_IncTick()

/**
*
**/
void SM_Tick( void )
{ 
  if ( 1 == gStateSM.time.b10ms )
  {
    gStateSM.proc = ST_HIGHT_PROCESS;
    gStateSM.st.bToggleLed = 1;
  }else;
  if ( 1 == gStateSM.time.b100ms )
  {
    gStateSM.proc = ST_MEDIUM_PROCESS;    
  }else;
  if ( 1 == gStateSM.time.b1000ms )
  {
    gStateSM.proc = ST_SLOW_PROCESS;
    ClrLed( &gLed );
    if ( gLed.led == led1_pin ) 
         gLed.led = led0_pin;
    else gLed.led = led1_pin;    
  }else;  

  if (     1 == gStateSM.st.bStart )  gStateSM.proc = ST_COMM_START;
  else if (1  == gStateSM.st.bStop )  gStateSM.proc = ST_COMM_STOP;
  
  if ( gMbStatus.bit.bStartCNC && hTimer->IsTimeOut( EV_COMM_START ) )
  {
//    gMbStatus.bit.bStartCNC = 0;
//    SetMBRgS( REG_R_STATUS_S, gMbStatus.reg );
  }else;
  
  switch( gStateSM.proc )
  {
  case ST_HIGHT_PROCESS:
    gStateSM.time.b10ms = 0;
    gStateSM.proc = ST_IDLE;
    break;
  case ST_MEDIUM_PROCESS:
    InRead();
    FireProcess();   
    hCmd->Proc();
    ReadStart( );
    gStateSM.time.b100ms = 0;

    xMBMasterPortEventPost( EV_MASTER_READY );
    //MBMasterRecieved();    
    
    gStateSM.proc = ST_IDLE;
    break;
  case ST_SLOW_PROCESS: 
    gStateSM.time.b1000ms = 0;     
    // TestOut();    
    gStateSM.time.b1000ms = 0; 
    gStateSM.proc = ST_TOGGLE_LED;
    break;
  case ST_COMM_START:          
      //hTimer->Time_Out( Timer::start, TIME_START, EV_COMM_START);      
      if (1 == gStateSM.st.bCommStart ) 
      {     
        gStateSM.st.bCommStart = 0;
        gMbStatus.bit.bStartCNC = 1;
        SetMBRgS( REG_R_STATUS, gMbStatus.reg );          
        WR_DEBUG("Start from CNC \r\n");    
      }
      gStateSM.st.bStart = 0;
    break;
  case ST_COMM_STOP:      
      if ( 1 == gStateSM.st.bCommStart )
      {         
        gStateSM.st.bCommStart = 0;
        gMbStatus.bit.bStartCNC = 0;
        SetMBRgS( REG_R_STATUS, gMbStatus.reg );                
        WR_DEBUG("Stop from CNC \r\n");    
      }
      gStateSM.st.bStop = 0;
    break;
  default:
    gStateSM.proc = ST_IDLE;
  }// switch();    
} // SM_Tick()


/**
  * @brief
  * @param  
  * @retval 
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  gStateSM.st.bAdcCmplt = 1;
    //getADC( );
}

#define  ADC_ZERO 7
#define  CURR1_COEF (float)8.4
#define  CURR2_COEF (float)8.4
#define  ADC_VOLT_COEF  (float)5.17

/*
Храрактеристика датчика напряжения  LV25-P
Uin V     Uout  V
101         1.1
200         2.2
300         3.3
400         4.44
500         5.6
600         6.8
*/

float VoltCoef =  ADC_VOLT_COEF;
int16_t adc_zero = ADC_ZERO; 
int16_t adc_v_zero = 17; 
/**
  * @brief  
  * @param
  * @retval
  */
void ADC_Process( void ) 
{
  static int32_t stAdc_cur1 = 0;
  static int32_t stAdc_cur2 = 0;
  static int32_t stAdc_volt = 0;
  
  if ( gStateSM.st.bAdcCmplt )
  {
    gStateSM.st.bAdcCmplt = 0;
    for(uint16_t i = 0; i< ADC_BUF_LENGHT; i++)
    {
      stAdc_cur1 += ADCdata[i].Current1;
      stAdc_cur2 += ADCdata[i].Current2;
      stAdc_volt += ADCdata[i].Voltage;      
    }
    stAdc_cur1 /= ADC_BUF_LENGHT;
    stAdc_cur2 /= ADC_BUF_LENGHT;
    stAdc_volt /= ADC_BUF_LENGHT;
    
    ADCdat.Current1  = stAdc_cur1;
    ADCdat.Current2  = stAdc_cur2;
    ADCdat.Voltage   = stAdc_volt;
     
    if ( stAdc_cur1 < ADC_ZERO) adc_zero = 0;
    if ( stAdc_cur2 < ADC_ZERO) adc_zero = 0;
    
    SetMBRgS( REG_R_CURR_1, (uint16_t)floor(10*(stAdc_cur1 - adc_zero) / CURR1_COEF));   // TODO  
    SetMBRgS( REG_R_CURR_2, (uint16_t)floor(10*(stAdc_cur2 - adc_zero) / CURR2_COEF));   // 50A - 424 ///  22.5 - 192// 0 - 7
    int16_t voltage  = (int16_t)floor(1*(stAdc_volt + adc_v_zero) / VoltCoef);
    if ( voltage < 0 ) voltage = 0;
    
    SetMBRgS( REG_R_VOLT, voltage );   //
    
    DBG_ITM_Event(ITM_CH1, stAdc_cur1);
  }else;
} // ADC_Process()

/**
  * @brief  Изменилось управляющие слово - выполнить действие
  *         Работает для 16р регистра
  * @param
  * @retval Bit Number, если время еще не прошло выдаем
  */
int8_t  FindActiveBit( uint16_t ActiveBitRg )
{
  uint8_t nb = 0;        // номер бита

  for ( nb = 0; nb < NUMBERS_CNTL_BIT; nb ++ )
  {
    if ( ActiveBitRg & (1 << nb ) ) break;
  }

  if ( nb > 15 )     return -1;
  else               return nb;
} // FindActiveBit()
/** (END OF FILE  : StateMaСЃhine.cpp) 
*******************************/ 

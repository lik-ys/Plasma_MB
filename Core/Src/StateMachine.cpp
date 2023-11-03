/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : StateMashine.cpp
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

#include  "StateMaсhine.h"// 
#include  "mb_com.hpp"
#include  "mb_m.h"
#include  "io_process.h"
#include  "main.h"
#include  "user_mb_app.h"
#include  "Fire.h"
#include  "Timer.hpp"

Timer gTimer;
Timer *hTimer = &gTimer;

extern void     SetMBRgS( eMBRegS_t numMBReg, uint16_t data );

eProcess_t  eSM_proc;
State_t     gStateSM = { TIME_10ms, TIME_100ms, TIME_1000ms, ST_IDLE, {0,0,0},{0,0,} };

RgCntrl_t   gMbCntrl;
RgCntrl_t   gMbStatus;

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


/*
**
*/
void ProcessInit( void )
{
  memcpy( gStateSM.tick, 0, sizeof(gStateSM.tick) );
  
  eSM_proc = ST_IDLE;
  gLed.led = led1_pin;
  
  pMBhl->Init();
  pMBcntrl->Init();
  
  //HAL_TIM_Base_Start_IT( pExtSync );
  HAL_TIM_OC_Start( pExtSync,TIM_CHANNEL_1 );
  
  hTimer->Time_Out( Timer::start, TIME_OUT_TEST, PROC_EV_DEBUG );
}// ProcessInit()

/**
** 
*/
void SM_loop( void )
{  
  pMBhl->Loop( );
  pMBcntrl->Loop();
  
  FireProcess();
  CommandProcess();
  
  SM_Tick();
  
  CHAR data  = 0;
  CHAR * pdata = &data;
  
  if ( 1 == gStateSM.st.bAdcCmplt){
    gStateSM.proc = ST_ADC_CMPLT;
  }else;
  
  switch( gStateSM.proc )    // 
  {
    case ST_IDLE:
      break;
      
    case ST_TOGGLE_LED:
      ToggleLed( &gLed );
      eSM_proc = ST_IDLE;
      RS485_Dir_m( tx );
      xMBMasterPortSerialPutByte(0x55); // работает 
      RS485_Dir_m( rx );
      xMBPortSerialGetByte(pdata);
      eSM_proc = ST_IDLE;
      break;
      
    case ST_START:
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
    gStateSM.time.b10ms = 0;
    eSM_proc = ST_TOGGLE_LED;
  }else;
  if ( 1 == gStateSM.time.b100ms )
  {
    gStateSM.time.b100ms = 0;
    eSM_proc = ST_TX;
    
  }else;
  if ( 1 == gStateSM.time.b1000ms )
  {
    gStateSM.time.b1000ms = 0;
    
    //static int i = 0;
//    WR_DEBUG("__Debug__ cnt = %i \r\n", i++);
        
    ClrLed( &gLed );
    if ( gLed.led == led1_pin ) 
         gLed.led = led0_pin;
    else gLed.led = led1_pin;    
    
  }else;  
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

/**
  * @brief  
  * @param
  * @retval
  */
void ADC_Process( void ) 
{
  static uint32_t stAdc_cur1 = 0;
  static uint32_t stAdc_cur2 = 0;
  static uint32_t stAdc_volt = 0;
  
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
    
    SetMBRgS( REG_R_CURR_1, stAdc_cur1 );
    SetMBRgS( REG_R_CURR_2, stAdc_cur2 );
    SetMBRgS( REG_R_VOLT,   stAdc_volt );
    
    DBG_ITM_Event(ITM_CH1, stAdc_cur1);
  }else;
} // ADC_Process()

/** (END OF FILE  : StateMashine.cpp) 
*******************************/ 

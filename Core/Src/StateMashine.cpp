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
* Description       : √лавна€ плата управлени€ плазмотроном
*                   : 
* Hardware          : .sch .pcb
*
********************************************************************************/

#include  <stdint.h>

#include  "StateMashine.h"// 
#include  "mb_com.hpp"
#include  "mb_m.h"
#include  "io_process.h"
#include  "main.h"

eProcess_t  eSM_proc;
State_t     gStateSM = { TIME_10ms, TIME_100ms, TIME_1000ms };

HAL_StatusTypeDef 	HAL_status;

uint8_t bufTx[3] =  {0x55, 0x55, 0x55 };

extern ModBusCom *pMBcntrl;
extern ModBusCom *pMBhl;

void SM_Tick( void );

/*
**
*/
void ProcessInit( void )
{
  eSM_proc = ST_IDLE;
  gLed.led = led1_pin;
  
  pMBhl->Init();
  pMBcntrl->Init();
  
  //HAL_TIM_Base_Start_IT( pExtSync );
  HAL_TIM_OC_Start(pExtSync,TIM_CHANNEL_1 );
}// ProcessInit()

/**
**    todo проверить передачу
*/
void SM_loop( void )
{  
  pMBhl->Loop( );
  pMBcntrl->Loop();
  
  SM_Tick();
  
  CHAR data  = 0;
  CHAR * pdata = &data;
  
  switch( eSM_proc )    // 
  {
    case ST_IDLE:
      break;
      
    case ST_TOGGLE_LED:
      ToggleLed( &gLed );
      eSM_proc = ST_IDLE;
      xMBMasterPortSerialPutByte(0x55);
      RS485_Dir( rx );
      xMBPortSerialGetByte(pdata);
      break;
      
    case ST_START:
      break;
      
    case ST_MB_MASTER:
      break;
      
    case ST_MB_SLAVE:
      break;   
      
    case ST_TX:
      //HAL_status = HAL_UART_Transmit_DMA( pUartWIFI, bufTx, 3 );  // первый раз отправл€ет - потом бизи
//      HAL_status = HAL_UART_Transmit_DMA( pMBSlave,   bufTx, 3 );
//      HAL_status = HAL_UART_Transmit_DMA( pMBMaster,bufTx, 3 );
      //HAL_status = HAL_UART_Transmit_DMA( pUartDBG,  bufTx, 3 );
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
  static uint32_t preTick = 0;
  
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
        
    ClrLed( &gLed );
    if ( gLed.led == led1_pin ) 
         gLed.led = led0_pin;
    else gLed.led = led1_pin;    
    
  }else;  
} // SM_Tick()

/** (END OF FILE  : StateMashine.cpp) 
*******************************/ 

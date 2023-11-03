/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : StateMaсhine.h
* Compiler          : IAR EWA 8.32
* Version           : 0.0
* Created File      : 01.06.2023
* Last modified     : 01.06.2023
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU: f407 @ (Xtal = MHz | CPUclk = MHz)
* Description       : 
*                   : 
* Hardware          : .sch .pcb
*
********************************************************************************/
/*===============================[ REDEFINITION DEFENCE ]======================*/ 
#ifndef __STATE_MASHINE_HPP__ 
#define __STATE_MASHINE_HPP__ 

#include "main.h"  

#ifdef __cplusplus
extern "C" {
#endif
  
/*===============================[ SPECIAL ]===================================*/ 
    // Блок операторов условной компиляции 
/*===============================[ PUBLIC CONSTANTS ]==========================*/ 
    //  Публичные константы 
#define ADC_MAX         4095
#define DAC_MAX         4095

#define ADC_BUF_LENGHT  32
  
#define TIME_OUT_TEST   250
/*===============================[ PUBLIC TYPES ]==============================*/ 
    // Публичные типы 
typedef   enum
{
  eCurrent1  ,  // eVoltage
  eCurrent2  ,
  eVoltage   ,  
  eChanMax
} ADC_Channels_t;

typedef struct
{
  uint16_t Current1  ;
  uint16_t Current2  ;
  uint16_t Voltage   ;
} ADC_data_t;
/*===============================[ FORWARD REFERENCES ]========================*/ 
    // Ссылки вперед 
/*===============================[ PUBLIC VARIABLES ]==========================*/ 
    // Публичные переменные 
/*===============================[ PUBLIC FUNCTIONS ]==========================*/ 
    // Публичные функции  
/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 
    // Inline функции (методы класса)
/*===============================[ END REDEFINITION DEFENCE]===================*/ 
    // Окончание однократно включаемого h-файла
   
typedef enum
{
  ST_IDLE,
  ST_TOGGLE_LED,
  ST_START,
  ST_MB_MASTER,
  ST_MB_SLAVE,
  ST_TX,
  ST_ADC_CMPLT,
  
  ST_FIRE_START,
  ST_FIRE_WAITE,
  ST_FIRE_ON,
  ST_FIRE_OFF,
  
  ST_COMM_START     ,    
  ST_METAL_CONTACT  ,
  ST_COMM_FIRE      ,  
    
  ST_END    
} eProcess_t;

typedef struct
{
  uint16_t bIdle        : 1;
  uint16_t bToggleLed   : 1;
  uint16_t bStart       : 1;
  uint16_t bMB_Master   : 1;
  uint16_t bMB_Slave    : 1;
  uint16_t bAdcCmplt    : 1;
  
  uint16_t bFireStrat   : 1;
  uint16_t bFireWaite   : 1;
  uint16_t bFireOn      : 1;
  uint16_t bFireOff     : 1;
  
  uint16_t bCommStart   : 1;         
  uint16_t bMetalContact: 1;
  uint16_t bCommFire    : 1;
  
}bProcess_t;

typedef enum
{
  TIME_10ms   = 10,
  TIME_100ms  = 100,
  TIME_1000ms = 1000  
} eTime_t;

typedef struct
{
  uint16_t b10ms   : 1;
  uint16_t b100ms  : 1;
  uint16_t b1000ms : 1;  
}bTime_t;

typedef  enum
{
  EV_IDLE       ,           // 0
  EV_COMMAND    ,           // 1
  EV_MB_CONNECT_SLAVE ,     // 2
  EV_MB_CONNECT_MASTER,     // 3  
  EV_ADC_DATA   ,           // 4  
  
  EV_FIRE_START,
  EV_FIRE_WAITE,
  EV_FIRE_ON,
  EV_FIRE_OF,

  EV_COMM_START     ,    
  EV_METAL_CONTACT  ,
  EV_COMM_FIRE      ,
  
  PROC_EV_DEBUG     ,
  EV_COUNTS  
}eProcessCom_t;

typedef struct 
{
  uint16_t   div10;
  uint16_t   div100;
  uint16_t   div1000;
  eProcess_t  proc;
  
  bTime_t    time;
  bProcess_t st;  
  uint32_t   tick[ EV_COUNTS ];
  eProcessCom_t event;
  
}State_t;


// REG_W_CNTRL

typedef union 
{  
  struct {
    uint16_t bFireStart     : 1;
    uint16_t bChopperStart  : 1;    
  }bit;
  uint16_t reg;  
}RgCntrl_t;

extern eProcess_t  eSM_proc;
extern ADC_data_t  ADCdata[ ADC_BUF_LENGHT ];
extern State_t     gStateSM;

extern RgCntrl_t   gMbCntrl;
extern RgCntrl_t   gMbStatus;
  
void ProcessInit( void );

void SM_loop( void );

#ifdef __cplusplus
}
#endif

#endif 
/** (END OF FILE  : StateMashine.hpp) 
*******************************/ 

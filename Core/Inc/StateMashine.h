/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : StateMashine.h
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

#ifdef __cplusplus
extern "C" {
#endif
  
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
  
  ST_END    
} eProcess_t;

typedef struct
{
  uint16_t bIdle        : 1;
  uint16_t bToggleLed   : 1;
  uint16_t bStart       : 1;
  uint16_t bMB_Master   : 1;
  uint16_t bMB_Slave    : 1;

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

typedef struct 
{
  uint16_t   div10;
  uint16_t   div100;
  uint16_t   div1000;
  eProcess_t proc;
  bTime_t    time;
  bProcess_t st;  
}State_t;

extern  eProcess_t eSM_proc;
  
void ProcessInit(void);

void SM_loop( void );

#ifdef __cplusplus
}
#endif

#endif 
/** (END OF FILE  : StateMashine.hpp) 
*******************************/ 

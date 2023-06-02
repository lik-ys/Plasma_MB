/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : StateMashine.hpp
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
  ST_START,
  ST_MB_MASTER,
  ST_MB_SLAVE,
  
  ST_END    
} eProcess_t;
  
extern  eProcess_t eSM_proc;
  
void ProcessInit(void);

void SM_process( void );

#endif 
/** (END OF FILE  : StateMashine.hpp) 
*******************************/ 

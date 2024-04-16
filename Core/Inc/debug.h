/*******************************************************************************
*
* Copyright (C) 2022 Sharopin Yuri
*
* File              : Debug.h
* Compiler          : IAR EW ARM 8.32
* Version           : 0.0
* Created File      : 11.10.2022
* Last modified     : 11.10.2022
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU @ (Xtal = MHz | CPUclk = MHz)
* Description       : 
*                   : 
* Hardware          : .sch .pcb
*
********************************************************************************/
/*===============================[ REDEFINITION DEFENCE ]======================*/ 
#ifndef __DEBUG_H__ 
#define __DEBUG_H__
/*===============================[ SPECIAL ]===================================*/ 
                       // Ѕлок операторов условной компил€ции 
/*===============================[ IMPORT DECLARATIONS ]=======================*/
/* ----------------------------- [ System includes   ]-------------------------*/
//#include   <> // MCS
/* ------------------------------[ Platform includes ]-------------------------*/
//#include   <> // IDE, OS
/*-------------------------------[ Standart libs     ]-------------------------*/
#include   <stdio.h> 
/* ------------------------------[ Application level ]-------------------------*/
//#include   "" // 
/*===============================[ PUBLIC CONSTANTS ]=======================*/ 
// ¬се частные #defines и constants должны быть объ€влены в данном разделе. 
#define __UART_DEBUG__  0

#if ( 1 == __UART_DEBUG__ )
#define   WR_DEBUG(...)     {printf(__VA_ARGS__);  };//fflush(stdout);}  // Write Debuge
  ///------- ќтладка модул€ ModBus
  #ifdef    __MB_DEBUG__
#define   MB_DEBUG(...)     {printf("--MB--"__VA_ARGS__); }//delay_l(50000);}
  #else
    #define MB_DEBUG(...)   {}
  #endif
  ///------- ќтладка модул€ CAN
  #ifdef    __CAN_DEBUG__
  #define   CAN_DEBUG(...)    {printf("--CAN--"__VA_ARGS__); delay_l(50000);}
  #else
    #define CAN_DEBUG(...)  {}
  #endif
  ///------- ќтладка модул€ ...
#else
  #define   WR_DEBUG(...)   {}
  #define   MB_DEBUG(...)   {}
  #define   CAN_DEBUG(...)  {}
#endif

#define ITM_UNLOCK   0xC5ACCE55

#define __ITM_UNLOCK( unlock )     *((volatile unsigned long *)0xE0000FB0) = unlock;

#define REG_DEBUG       1   // передача измерени€ от MB к €чейки

/*===============================[ TYPE DEFINITIONS ]==========================*/ 
typedef enum
{
  ITM_CH1   = 1 ,
  ITM_CH2       ,
  ITM_CH3       ,
  ITM_CH4
}ITM_Channels_t;
/*===============================[ EXTERN VAR ]================================*/ 
//extern    int ext_var;
/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 
//inline                 // Inline функции (методы класса)
                       
/*===============================[ EXPORTED FUNCTIONS ]========================*/ 

void  DBG_ITM_Event( ITM_Channels_t itm_ch, uint32_t );

/*===============================[ END REDEFINITION DEFENCE]===================*/ 
                         // ќкончание однократно включаемого h-файла
                       
#endif
/** (END OF FILE  : Debug.h.h) 
*******************************/
 

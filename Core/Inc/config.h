/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : config.h
* Compiler          : IAR EW ARM 8.32
* Version           : 0.0
* Created File      : 02.06.2023
* Last modified     : 02.06.2023
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
#ifndef __CONFIG_H__ 
#define __CONFIG_H__
/*===============================[ SPECIAL ]===================================*/ 
                       // Ѕлок операторов условной компил€ции 
/*===============================[ IMPORT DECLARATIONS ]=======================*/
/* ----------------------------- [ System includes   ]-------------------------*/
//#include   <> // MCS
/* ------------------------------[ Platform includes ]-------------------------*/
//#include   <> // IDE, OS
/*-------------------------------[ Standart libs     ]-------------------------*/
//#include   <> // 
/* ------------------------------[ Application level ]-------------------------*/
//#include   "" // 
/*===============================[ PUBLIC CONSTANTS ]=======================*/ 
// ¬се частные #defines и constants должны быть объ€влены в данном разделе. 

#define MB_ADDR_SLAVE       10
#define SLAVE_BAUD_RATE     57600
#define MASTER_BAUD_RATE    57600

/*===============================[ TYPE DEFINITIONS ]==========================*/ 
//typedef   int   new_type_t 
/*===============================[ EXTERN VAR ]================================*/ 
//extern    int ext_var;
/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 
//inline                 // Inline функции (методы класса)
                       

/*===============================[ EXPORTED FUNCTIONS ]========================*/ 
/*===============================[ END REDEFINITION DEFENCE]===================*/ 
                         // ќкончание однократно включаемого h-файла
                       
#endif
/** (END OF FILE  : config.h) 
*******************************/
 

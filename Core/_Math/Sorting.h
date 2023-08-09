/*******************************************************************************
*
* Copyright (C) 2016 Sharopin Yuri
*
* File              : Sorting.h.h
* Compiler          : IAR EWAAVR 7.50
* Version           : 0.0
* Created File      : 18.12.2016
* Last modified     : 18.12.2016
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
#ifndef __SORT_DATA_H__ 
#define __SORT_DATA_H__
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

/*===============================[ TYPE DEFINITIONS ]==========================*/ 

//typedef   int   new_type_t 

/*===============================[ EXTERN VAR ]================================*/ 

//extern    int ext_var;

/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 
//inline                 // Inline функции (методы класса)
                       
/*===============================[ EXPORTED FUNCTIONS ]========================*/ 

void Sort_tab( uint16_t tab[], uint8_t lenght );

void Sort_tab_f( float32_t tab[ ], uint8_t lenght );

/*===============================[ END REDEFINITION DEFENCE]===================*/ 
                         // ќкончание однократно включаемого h-файла
                       
#endif
/** (END OF FILE  : Sorting.h.h) 
*******************************/
 

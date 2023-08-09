/*******************************************************************************
*
* Copyright (C) 2010 Sharopin Yuri
*
* File              : Filters.h
* Compiler          : IAR EWAAVR 5.xx
* Version           : 0.0
* Last modified     : 09.07.2010
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
#ifndef __FILTERS_H__
#define __FILTERS_H__
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

#define   FILTER_SIZE     19 // - дл€ Gauss1 // 8
#define   SIZE_FIR        16
/*===============================[ TYPE DEFINITIONS ]==========================*/

//typedef   int   new_type_t
typedef   float   Signal_t;   // дл€ F4 полностью вещественна€ обработка
/*===============================[ EXTERN VAR ]================================*/

extern    const float FIRCoef[ ];
extern    const uint16_t   SizeFIR;

extern    const float Gauss1[ ];
extern    const uint16_t   SizeGF;

/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/
//inline                 // Inline функции (методы класса)

/*===============================[ EXPORTED FUNCTIONS ]========================*/

void    Init_LF_Filter( );
float   LF_filter( float Xn );
void    LPFilter( uint16_t* RawSignal, uint16_t Len );
void    FilterTest( void );
void    Threshold( uint16_t* RawSignal, uint16_t Len );
Signal_t * Convolution(Signal_t* result, Signal_t* x, uint16_t N, const float * h, uint16_t M);
float*    Convolutionf( float* result, float* x, uint16_t N, const float* h, uint16_t M );
void     MedianFiltr( Signal_t* Res, Signal_t*x, uint16_t size, uint16_t win );

int32_t    AverageN_DeleteX   ( uint16_t* adc_sample, uint16_t N , uint16_t X );
float32_t  AverageN_DeleteX_f( float32_t* adc_sample, uint16_t N , uint16_t X );

/*===============================[ END REDEFINITION DEFENCE]===================*/
// ќкончание однократно включаемого h-файла

#endif
/** (END OF FILE  : Filters.h)
*******************************/


/*******************************************************************************
*
* Copyright (C) 2016 Sharopin Yuri
*
* File              : Sorting.c
* Compiler          : IAR EWA 7.50
* Version           : 0.0
* Created File      : 18.12.2016
* Last modified     : 18.12.2016
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU: STM32F407VE @ (Xtal = MHz | CPUclk = MHz)
* Description       : Сортировка
*                   :

* Hardware          : .sch .pcb
*
********************************************************************************/

/**
  * @verbatim Описание
  *
  * @endverbatim
*/

/** @addtogroup ....
  * @{
  */

/** @defgroup Name
  * @brief bla-bla-bla
  * @{
  */

/*===============================[ SPECIAL ]===================================*/
                      //  Условная компиляция

/*===============================[ IMPORT DECLARATIONS ]=======================*/
                      // Все системные, библиотечные и заголовочные файлы
/* ----------------------------- [ System includes   ]-------------------------*/
//#include   <> //  MCS
/*-------------------------------[ Platform level    ]-------------------------*/
//#include   <> //  IDE, OS
/*-------------------------------[ Standart libs     ]-------------------------*/
//#include   <> //
/*-------------------------------[ Application Level ]-------------------------*/
#include "main.h"
#include    "Sorting.h"          //

/*===============================[ PRIVATE CONSTANTS ]=========================*/
// Все частные #defines и constants должны быть объявлены в данном разделе.
/*===============================[ PRIVATE TYPES ]=============================*/
// Все частные типы, которые используются в данном исходном файле, должны быть объявлены в данном разделе.
/*===============================[ PRIVATE VARIABLES ]=========================*/
// В этом разделе объявляются все частные переменные.
/*===============================[ PUBLIC  VARIABLES ]=========================*/
// Все глобальные переменные должны быть заданы в этом разделе.
/*===============================[ PRIVATE PSEUDO FUNCTIONS ]==================*/
// Все макросы должны быть объявлены в этом разделе.
/*===============================[ PRIVATE FUNCTIONS ]=========================*/
// Все частные функции, используемые в данном исходном файле.

/** @defgroup
  * @{
  */

/**
  * @}
  */
/*===============================[ PUBLIC FUNCTIONS ]==========================*/
// Все экспортируемые фукнции.
/**
  * @brief   Sort the N ADC samples
  * @param ADC samples to be sorted
  * @param Numbre of ADC samples to be sorted
  * @retval None
  */
void Sort_tab( uint16_t tab[], uint8_t lenght )
{
   uint8_t   k 	 = 0x00, exchange = 0x01;
   uint16_t  tmp = 0x00;

    /* Sort tab */
    while( 1 == exchange )
    {
      exchange = 0;                                  
      for( k = 0; k < lenght - 1; k++ )
      {
        if( tab[ k ] > tab[ k + 1 ] )
        {
            tmp        = tab[ k   ];
            tab[ k   ] = tab[ k+1 ];
            tab[ k+1 ] = tmp;
            exchange   = 1;
        }else;
      } // for()
    } // while()
} // Sort_tab()

void Sort_tab_f( float32_t tab[ ], uint8_t lenght )
{
   uint8_t    k   = 0x00, exchange = 0x01;
   float32_t  tmp = 0x00;

    /* Sort tab */
    while( 1 == exchange )
    {
      exchange = 0;
      for( k = 0; k < lenght - 1; k++ )
      {
        if( tab[ k ] > tab[ k + 1 ] )
        {
            tmp        = tab[ k   ];
            tab[ k   ] = tab[ k+1 ];
            tab[ k+1 ] = tmp;
            exchange   = 1;
        }else;
      } // for()
    } // while()
} // Sort_tab()
/**
  * @}
  */

/**
  * @}
  */

/** (END OF FILE  : Sorting.c)
*******************************/


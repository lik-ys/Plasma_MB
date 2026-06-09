/*******************************************************************************
*
* Copyright (C) 2015 Sharopin Yuri
*
* File              : FifoBuf.c
* Compiler          : IAR EWA 6.60.2
* Version           : 0.0
* Last modified     : .2015
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU:  @ (Xtal = MHz | CPUclk = MHz)
* Description       : 
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

#include    "FifoBuf.h"          //
#include <string.h>
#include <stdio.h>
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
  * @brief  
  * @param  
  * @retval	  
  */
void InitFIFO ( FifoBuf_t* pFifo, void* pInData, uint16_t mask, uint16_t stride )
{
  pFifo->count = 0;
  pFifo->head  = 0;
  pFifo->len   = stride;
  pFifo->mask  = mask;
  pFifo->state = FIFO_EMPTY;
  pFifo->pBuf  = pInData;

} //InitFIFO( ) 

/**
  * @brief 
  * @param  pFifo - указатель на буфер, len_data - длина записи, pInData - указатель на данные
  * @retval	  
  */
FifoState_t PushFIFO( FifoBuf_t* pFifo, void* pInData  )
{
  if ( NULL == pFifo    )           return FIFO_ERROR_INPUT;
  if ( NULL == pInData  )           return FIFO_ERROR_INPUT; 

  if ( (FIFO_FREE == pFifo->state) || (FIFO_EMPTY == pFifo->state) )
  {
    uint8_t* pbuf = (uint8_t*)pFifo->pBuf;
    
    memcpy( &pbuf[ pFifo->head * pFifo->len ] , pInData, pFifo->len );

    pFifo->head++;
    pFifo->head &= pFifo->mask;
    if ( pFifo->count < (pFifo->mask + 1) ) pFifo->count ++; else;//todo
  }
  else
  {
    return pFifo->state;
  }
  return (pFifo->state  = ( pFifo->head != pFifo->tail ) ? FIFO_FREE : FIFO_FULL );
} // PushFIFO()

/**
  * @brief 
  * @param  
  * @retval	  
  */
FifoState_t PopFIFO( FifoBuf_t* pFifo, void* pOutData )
{
  if ( NULL == pFifo     )      return FIFO_ERROR_INPUT;
  if ( NULL == pOutData  )      return FIFO_ERROR_INPUT; 

  if ( ( pFifo->tail !=  pFifo->head ) || (FIFO_FULL == pFifo->state) )
  {
    uint8_t* pbuf = (uint8_t*)pFifo->pBuf;    
    memcpy( pOutData, &pbuf[ pFifo->tail *  pFifo->len ], pFifo->len );

    pFifo->tail++;
    pFifo->tail &= pFifo->mask;
    if ( pFifo->count > 0 ) pFifo->count --; else;//todo

  }
  else
  {
    return pFifo->state;
  }
  return (pFifo->state = (pFifo->tail != pFifo->head ) ? FIFO_FREE : FIFO_EMPTY );
}// PopFIFO()

/**
  * @brief  
  * @param  
  * @retval	  
  */
FifoState_t FlushFIFO ( FifoBuf_t* pFifo )
{
  pFifo->head  = pFifo->tail;
  pFifo->state = FIFO_EMPTY;
  pFifo->count = 0;
  return pFifo->state;
} //FlushFIFO()

/**
  * @brief
  * @param
  * @retval
  */
uint16_t GetSizeFIFO( FifoBuf_t* pFifo, void* pOutData )
{
  if ( NULL == pFifo     )      return FIFO_ERROR_INPUT;
  if ( NULL == pOutData  )      return FIFO_ERROR_INPUT;

  return pFifo->count;
}// PopFIFO()

/**
  * @}
  */

/**
  * @}
  */
 
/** (END OF FILE  : FifoBuf.c) 
*******************************/ 


/*******************************************************************************
*
* Copyright (C) 2022 Sharopin Yuri
*
* File              : MB_RegComm.c
* Compiler          : IAR EWA 8.32
* Version           : 0.0
* Created File      : 18.10.2022
* Last modified     : 18.10.2022
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
#include    "stm32f3xx_hal.h"     
#include    "MB_RegComm.h"          //
/*===============================[ PRIVATE CONSTANTS ]=========================*/ 
// Все частные #defines и constants должны быть объявлены в данном разделе. 
/*===============================[ PRIVATE TYPES ]=============================*/ 
// Все частные типы, которые используются в данном исходном файле, должны быть объявлены в данном разделе.  
/*===============================[ PRIVATE VARIABLES ]=========================*/ 
// В этом разделе объявляются все частные переменные.  
/*===============================[ PUBLIC  VARIABLES ]=========================*/ 
// Все глобальные переменные должны быть заданы в этом разделе.  
#include "MB_RegComm.h"
#include "modbus.h"
// Переменные регистров обмена по Modbus
// __IO
const uint16_t               usRegHoldingStart = REG_HOLDING_START;
//__no_init
__IO  uint16_t               usRegHoldingBuf[ REG_HOLDING_NREGS ];

// TODO пока работаем только с регистрами
const uint16_t               usRegInputStart = REG_INPUT_START;
      uint16_t               usRegInputBuf[ REG_INPUT_NREGS    ];
      uint8_t                ucRegCoilsBuf[ REG_COILS_SIZE / 8 ];
      
//RegStatus_t         xChangeStatus;      
RegStatus_t         mbRegStatus;        // (3)  Регистр состояния чоппера
//RegCntrlProces_t    mbRegCntrlRocess;   // (51) Регистр задания процесса (REG_W_CNTRL_PROCESS)
RegStatusProces_t   mbRegStatusRocess;  // (4 ) Регистр состояния процесса (REG_R_STATE_PROCESS)
RegCmdPwm_t         mbRegSet;           // 

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
void SetStReg( eMBcntrlStatus_t bit )
{  
  mbRegStatus.rg |= 1<<bit;
  SetMBRg( REG_R_STATE_UNIT, mbRegStatus.rg );
}

/**
  * @brief
  * @param  
  * @retval 
  */
void ClrStReg( eMBcntrlStatus_t bit )
{  
  mbRegStatus.rg &= ~(1<<bit);
  SetMBRg( REG_R_STATE_UNIT, mbRegStatus.rg );
}

/**
  * @brief
  * @param  
  * @retval 
  */
uint16_t TstStReg( eMBcntrlStatus_t bit )
{  
  return (mbRegStatus.rg & (1<<bit));  
}
/**
  * @brief
  * @param  
  * @retval 
  */
void SetMBRg( eMBReg_t numMBReg, uint16_t data )
{
  
  if ( numMBReg >= REG_LAST ) 
  {
    assert(numMBReg >= REG_LAST);
    WR_DEBUG("InputError \r\n");
  }
  
  usRegHoldingBuf[ numMBReg ] = data;  
} // SetMBRg()

/**
  * @brief
  * @param  
  * @retval 
  */
uint16_t GetMBRg( eMBReg_t numMBReg )
{
  if ( numMBReg >= REG_LAST ) 
  {
    assert(numMBReg >= REG_LAST);
    WR_DEBUG("InputError \r\n");
  }
  
  return usRegHoldingBuf[ numMBReg ];
}// GetMBRg()

/**
  * @}
  */
/**
  * @}
  */
 
/** (END OF FILE  : MB_RegComm.c) 
*******************************/ 

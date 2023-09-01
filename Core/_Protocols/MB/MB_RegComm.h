/*******************************************************************************
*
* Copyright (C) 2022 Sharopin Yuri
*
* File              : MB_RegComm.h
* Compiler          : IAR EW ARM 8.32
* Version           : 0.0
* Created File      : 18.10.2022
* Last modified     : 18.10.2022
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
#ifndef ____MB_REG_COMM_H__ 
#define ____MB_REG_COMM_H__
/*===============================[ SPECIAL ]===================================*/ 
                       // Блок операторов условной компиляции 
/*===============================[ IMPORT DECLARATIONS ]=======================*/
/* ----------------------------- [ System includes   ]-------------------------*/
//#include   <> // MCS
/* ------------------------------[ Platform includes ]-------------------------*/
//#include   <> // IDE, OS
/*-------------------------------[ Standart libs     ]-------------------------*/
//#include   <> // 
/* ------------------------------[ Application level ]-------------------------*/
//#include    "CommonType.h"
#include    "main.h"
/*===============================[ PUBLIC CONSTANTS ]=======================*/ 
// Все частные #defines и constants должны быть объявлены в данном разделе. 
/*===============================[ TYPE DEFINITIONS ]==========================*/ 

typedef enum _MB_REG
{
  REG_R_CURR_1          = 0,   // 
  REG_R_CURR_2          ,      // 
  REG_R_VOLT            ,

  REG_LAST = REG_R_VOLT          //
} eMBReg_t;// _MB_REG

#define REG_HOLDING_NREGS   REG_LAST          // Число регистров МБ
#define REG_INPUT_NREGS     1
#define REG_COILS_SIZE      8


/*===============================[ EXTERN VAR ]================================*/ 
extern    const uint16_t         usRegHoldingStart;
extern    __IO uint16_t          usRegHoldingBuf[ REG_HOLDING_NREGS ];
// TODO пока работаем только с регистрами
extern    const uint16_t         usRegInputStart;
extern    uint16_t               usRegInputBuf[ REG_INPUT_NREGS ];
extern    uint8_t                ucRegCoilsBuf[ REG_COILS_SIZE / 8 ];

extern    RegStatus_t            mbRegStatus;
extern    RegStatus_t            xChangeStatus;

extern    RegCntrlProces_t       mbRegCntrlRocess;
extern    RegStatusProces_t      mbRegStatusRocess;
extern    RegCmdPwm_t            mbRegSet;

/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 

//inline                 // Inline функции (методы класса)
                       
/*===============================[ EXPORTED FUNCTIONS ]========================*/ 

void     SetMBRg( eMBReg_t numMBReg, uint16_t data );
uint16_t GetMBRg( eMBReg_t numMBReg );

void     SetStReg( eMBcntrlStatus_t bit_mask );
void     ClrStReg( eMBcntrlStatus_t bit_mask );
uint16_t TstStReg( eMBcntrlStatus_t bit_mask );

/*===============================[ END REDEFINITION DEFENCE]===================*/ 
                         // Окончание однократно включаемого h-файла
                       
#endif
/** (END OF FILE  : MB_RegComm.h) 
*******************************/
 

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
#include    "CommonType.h"
#include    "modbus.h"
/*===============================[ PUBLIC CONSTANTS ]=======================*/ 
// Все частные #defines и constants должны быть объявлены в данном разделе. 
/*===============================[ TYPE DEFINITIONS ]==========================*/ 

typedef enum _MB_REG
{
  REG_R_CURR_1          = 0,   // Токи двух каналов
  REG_R_CURR_2          ,      // Уставка тока  REG_W_SET_OUT_CURRENT должна быть равна сумме токов вв двух каналах
  REG_R_IN_VOLT         ,
  REG_R_STATE_UNIT      = 3,   // (Вкл/Выкл, обрыв фаз, перегрев, тип управления, пепрегрузка по току, защита драйвера)
  REG_R_STATE_PROCESS   = 4,   // соотвествует REG_W_CNTRL_PROCESS (Ожидание, поджиг, Резка, Тушение, True Hole)
  
  REG_R_PWM1            ,
  REG_R_PWM2            ,
  REW_R_RESERV0         = 7 ,
  
  REG_W_PID_P           = 40,
  REG_W_PID_I           = 41,
  REG_W_PID_D           = 42,
  
  REG_W_SET_OUT_PWM     = 48,  // 0-100%
  REG_W_SET_OUT_CURRENT = 49,  // 0..300 А 
  REG_W_CONTROL         = 50,
  REG_W_CNTRL_PROCESS   = 51,  // соотвествует REG_R_STATE_PROCESS
  REG_W_CURRENT0        = 80,  // Таблица ID 80..92 Значение тока (12 значений) 
  REG_W_CURRENT1          ,    // 
  REG_W_CURRENT2          ,
  REG_W_CURRENT3          ,
  REG_W_CURRENT4          ,
  REG_W_CURRENT5          ,
  REG_W_CURRENT6          , 
  REG_W_CURRENT7          ,
  REG_W_CURRENT8          ,
  REG_W_CURRENT9          ,
  REG_W_CURRENT10         ,
  REG_W_CURRENT12         ,  
  REG_W_TIME0            = 93, // Таблица 93..104 Значение времени (12 значений)
  REG_W_TIME1             , 
  REG_W_TIME2             ,
  REG_W_TIME3             ,
  REG_W_TIME4             ,
  REG_W_TIME5             ,
  REG_W_TIME6             ,  
  REG_W_TIME7             ,
  REG_W_TIME8             ,
  REG_W_TIME9             ,
  REG_W_TIME10            ,
  REG_W_TIME12            ,  

  REG_LAST = REG_W_TIME12 + 40         //
} eMBReg_t;// _MB_REG

#define REG_HOLDING_NREGS   REG_LAST          // Число регистров МБ
#define REG_INPUT_NREGS     1
#define REG_COILS_SIZE      8

/*
* REG_R_STATE_UNIT
*/
typedef enum __STATE_UNIT
{  
  BIT_ST_UNIT_ON_OFF        ,   // 0: 1- ON генерация ШИМ
  BIT_ST_UNIT_PHASE_FAILURE ,   // 1: 1 - phase break 
  BIT_ST_UNIT_OVERHEAT      ,   // 2: 1 - overheat
  BIT_ST_UNIT_AUTO_MANUAL   ,   // 3: 1 - auto  0- manual
  BIT_ST_COM_CUR1           ,   // 4: статус комаратора 1   OverCur1
  BIT_ST_COM_CUR2           ,   // 5: статус комаратора 2   OverCur2
  BIT_ST_DRV1_FAULT         ,   // 6: driver protection status 1
  BIT_ST_DRV2_FAULT         ,   // 7: driver protection status 2
  BIT_ST_EV2_               ,   // 8:
  BIT_ST_EV3_               ,   // 9:
  BIT_ST_EV4_               ,   // 10: (PB6)EEV4 - KN_ON
  BIT_ST_EV7_THERMO         ,   // 11: 
  BIT_ST_ERROR_JMP_CFG      ,   // 12: Error of JMP configuration
  
  BIT_ST_UNIT_LAST  
} eMBcntrlStatus_t;

typedef union
{
  uint16_t rg;
  struct
  {
    uint16_t    on_off       : 1; //0:
    uint16_t    phaseFailure : 1; //1:
    uint16_t    overHeat     : 1; //2:
    uint16_t    autoManual   : 1; //3:
    uint16_t    overCurrent1 : 1; //4:
    uint16_t    overCurrent2 : 1; //5:
    uint16_t    DRV1_Fault   : 1; //6:
    uint16_t    DRV2_Fault   : 1; //7:
    uint16_t    event2       : 1; //8
    uint16_t    event3       : 1; //9
    uint16_t    event4       : 1; //10:
    uint16_t    event7       : 1; //11:    
    uint16_t    error_jmp_cfg: 1; //12:
  }status;                           
}RegStatus_t;                        

/*
* REG_W_CONTROL
*/
typedef enum __CONTROL
{
  BIT_CNTRL_STOP    = 0  ,
  BIT_CNTRL_START   = 1  ,
  BIT_CNTRL_LAST
    
} eMBcntrl_t;

/*
* REG_R_STATE_PROCESS
*/
typedef enum __PROCESS_STATE_BITS
{  
    PROCESS_WAITING     ,
    PROCESS_BURN        ,
    PROCESS_CUTTING     ,
    PROCESS_EXTINCTION  ,
    PROCESS_TRUE_HOLLE  ,
    PROCESS_LAST
      
} eMBprocess_t;

/*
* REG_W_CNTRL_PROCESS
*/
typedef struct
{
  uint16_t bWaiting   : 1;
  uint16_t bBurn      : 1;  // fair
  uint16_t bCutting   : 1;
  uint16_t bExtinguishing   :1;// suppression   
  uint16_t bTrueHole : 1;  
}Process_t;

/*
* REG_W_CNTRL_PROCESS
*/
typedef union
{
  uint16_t rg;
  Process_t bit;
}RegCntrlProces_t;

typedef union
{
  uint16_t rg;
  Process_t bit;
}RegStatusProces_t;
/*
* REG_R_STATE_UNIT
*/
typedef struct STATE_UNIT
{  
  uint16_t ST_UNIT_ON_OFF        ;  // 1 - ON
  uint16_t ST_UNIT_PHASE_BREAK   ;  // 1 - phase break 
  uint16_t ST_UNIT_OVERHEAT      ;  // 1 - overheat
  uint16_t ST_UNIT_AUTO_MANUAL   ;  // 1 - auto

} sMBcntrlProces_t;

/*
* REG_W_CNTRL
*/
typedef enum 
{
  CNTRL_STOP   = 0 ,
  CNTRL_START  = 1 , 
  CNTRL_LAST
    
} sMBcntrl_t;

/*
* REG_R_STATE_PROCESS
*/
typedef enum PROCESS_STATE_BITS
{  
  ePROCESS_WAITING    ,
  ePROCESS_BURN       ,
  ePROCESS_CUTTING    ,
  ePROCESS_EXTINCTION ,
  ePROCESS_TRUE_HOLLE ,

  ePROCESS_Last
} sMBprocess_t;


typedef struct _CNTRL_
{
  sMBcntrl_t    Cntrl    ;// REG_W_CONTROL
  sMBprocess_t  Process  ;// REG_W_CNTRL_PROCESS
}Cntrl_t;

/*
* Уставки
*/
typedef struct _PWM_SET
{
  uint16_t    Pwm1 : 12; // 0... 4095
  uint16_t    Pwm2 : 12;
  uint32_t    Current;
}
RegCmdPwm_t;

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
 

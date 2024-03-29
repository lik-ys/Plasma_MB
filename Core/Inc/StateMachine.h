/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : StateMaсhine.h
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

#include "main.h" 
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===============================[ SPECIAL ]===================================*/ 
    // Блок операторов условной компиляции 
/*===============================[ PUBLIC CONSTANTS ]==========================*/ 
    //  Публичные константы 
#define ADC_MAX         4095
#define DAC_MAX         4095

#define ADC_BUF_LENGHT  16
  
#define TIME_OUT_TEST   250
#define THRESHOLD_CURR_1    100  // 100 A  - пороговый ток включения готовности для ЧПУ
#define THRESHOLD_CURR_OFF  30  // 20 A  - пороговый ток выключения ШИМ на всех ячейкаях
  
#define PERIOD_MB_MASTER_TO  60  
#define PERIOD_REQUEST_TO    100
#define PERIOD_READ_TO       50  
#define PERIOD_MB_MASTER_TO  60  // 60
#define PERIOD_REQUEST_TO    100 // 100
#define PERIOD_READ_TO       50 //  40
  
/*===============================[ PUBLIC TYPES ]==============================*/ 
    // Публичные типы 
typedef   enum
{
  eCurrent1  ,  // eVoltage
  eCurrent2  ,
  eVoltage   ,  
  eChanMax
} ADC_Channels_t;

typedef struct
{
  uint16_t Current1  ; // ток возврата
  uint16_t Current2  ; // ток прямой    разница токов == току дежурки
  uint16_t Voltage   ;
} ADC_data_t;

typedef struct
{
  uint16_t Current1  ; // ток возврата
  uint16_t Current2  ; // ток прямой    разница токов == току дежурки
  uint16_t Voltage   ; // 
  uint16_t PilotCurr ; // ток дежурной дуги 
}PhParam_t;

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
  ST_TOGGLE_LED,
  ST_START,
  
  ST_ADC_CMPLT,
  
  ST_FIRE_START,
  ST_FIRE_WAITE,
  ST_FIRE_ON,
  ST_FIRE_OFF,
  
  ST_COMM_START     , // на зымыкание Устанавливаю состояние START на размыкание выключаем реле cnc0
  ST_COMM_STOP      ,
  ST_METAL_CONTACT  , // TODO
  ST_COMM_FIRE      , 
  
  ST_HIGHT_PROCESS,
  ST_MEDIUM_PROCESS,
  ST_SLOW_PROCESS,
    
  ST_END    
} eProcess_t;

typedef struct
{
  uint16_t bIdle        : 1;
  uint16_t bToggleLed   : 1;
  uint16_t bStart       : 1;
  uint16_t bStop        : 1;
  uint16_t bMB_Master   : 1;
  uint16_t bMB_Slave    : 1;
  uint16_t bAdcCmplt    : 1;
  
  uint16_t bExti        : 1;
  uint16_t bFireStrat   : 1;
  uint16_t bFireWaite   : 1;
  uint16_t bFireOn      : 1;
  uint16_t bFireOff     : 1;
  uint16_t bFireIn      : 1;
  
  uint16_t bCommStart   : 1;         
  uint16_t bMetalContact: 1;
  uint16_t bCommFire    : 1;
  
  uint16_t bTestCurr1   : 1; // Начать проверять рост тока дуги
  uint16_t bIgnitionOk  : 1; // Поджиг сработал - выдать на ЧПУ сигнал готовоность
  
  uint16_t bHightProc   : 1;
  uint16_t bMediumProc  : 1;
  uint16_t bSlowProc    : 1;
  
}bProcess_t;

typedef enum
{
  TIME_10ms   = 10,
  TIME_100ms  = 100,
  TIME_1000ms = 1000  
} eTime_t;

typedef struct
{
  uint16_t b10ms   : 1;
  uint16_t b100ms  : 1;
  uint16_t b1000ms : 1;  
}bTime_t;

typedef  enum
{
  EV_IDLE       ,           // 0
  EV_COMMAND    ,           // 1
  EV_MB_CONNECT_SLAVE ,     // 2
  EV_MB_CONNECT_MASTER,     // 3  
  EV_ADC_DATA   ,           // 4  
  
  EV_FIRE_START,
  EV_FIRE_WAITE,
  EV_FIRE_ON,
  EV_FIRE_OFF,

  EV_COMM_START     , // ЧПУ -> START -> состояние СТАРТ. Леха, камандует ПОДЖИГ, ожидаем тока 20мс, выключаем поджиг
  EV_METAL_CONTACT  ,
  EV_COMM_FIRE      ,
  
  EV_PILOT_ARC_TO   ,
  EV_IGNITION       , // Поджиг есть - мониторим ток
  EV_IGNITION1      ,
  
  EV_TEST_SHORT_CICUT,
  
  EV_WRITE_MBM      ,  // период записи в МБ мастер
  EV_REQUEST_TO     ,  
  EV_READ_TO        ,  // период опроса ячеек
  PROC_EV_DEBUG     ,
  
  EV_COUNTS  
}eProcessCom_t;

typedef struct 
{
  uint16_t   div10;
  uint16_t   div100;
  uint16_t   div1000;
  eProcess_t  proc;
  
  bTime_t    time;
  bProcess_t st;  
  uint32_t   tick[ EV_COUNTS ];
  eProcessCom_t event;
  
}State_t;

// REG_S_CNTRL
typedef union 
{  
  uint16_t reg;
  struct {
    uint16_t bOnOffPwr      : 1;    // 0 - вкл-выкл источника
    uint16_t bPilotArc      : 1;    // 1 - вкл-выкл дежурной дуги     
    uint16_t bFireStart     : 1;    // 2 - вкл-выкл поджига
    uint16_t bAutoManualM   : 1;    // 3 - 1 - автомат 0 - ручной TODO
    uint16_t bOverHeat_t    : 1;    // 4 – перегрев
    uint16_t bFaseFail_t    : 1;    // 5 – обрыв фазы
    uint16_t bChillerFail : 1;    // 6 – ошибка чиллера           
    uint16_t bStartCNC      : 1;    // 7 - 1 - Start, 0 - Stop CNC
   
    uint16_t bShortCircuit  : 1;    // 8 - синхронное выключение ячеек от команды  bit0 в  REG_W_CNTRL_START  
    uint16_t bFireIn        : 1;    // 9 - todo внешний поджиг     
    uint16_t bReserv10      : 1;
    uint16_t bReserv11      : 1;
    uint16_t bReserv12      : 1;
    uint16_t bReserv13      : 1;
    uint16_t bChopperStart  : 1;
    uint16_t bWriteEEPROM   : 1;      // 15 - EEprom write TODO
  }bit;   
}RgCntrl_t;

typedef union{
  uint16_t rg;
  struct{
      uint16_t rgReserv0  : 1;
      uint16_t rgCNTRL	  : 1;
      uint16_t rgPWM	  : 1;
      uint16_t rgCURR	  : 1;
      uint16_t rgSLOP_1   : 1; 
      uint16_t rgSLOP_2   : 1; 
      uint16_t rgP        : 1; 
      uint16_t rgI        : 1; 
      uint16_t rgD        : 1; 
      uint16_t rgTO_PA_Off: 1;
      uint16_t rgTimeSlopRise:1;
      uint16_t rgTimeSlopFail:1; // 12
      //for debug
      //uint16_t c1   :1;
      //uint16_t c2   :1;
  };
}bActivRg_t;  

// регистры управления ячейкой 
typedef union
{ 
  struct{
    uint16_t rgReserv0  ;
    uint16_t rgCNTRL	  ;
    uint16_t rgPWM	  ;
    uint16_t rgCURR	  ;
    uint16_t rgSLOP_1  ; 
    uint16_t rgSLOP_2  ; 
    uint16_t rgP       ; 
    uint16_t rgI       ; 
    uint16_t rgD       ;  
  };
  uint16_t mbReg[ 9 ];  
}CntrlCellsRg_t;

extern uint16_t    gProblemAddr;
extern eProcess_t  eSM_proc;
extern ADC_data_t  ADCdata[ ADC_BUF_LENGHT ];
extern ADC_data_t  ADCdat;
extern PhParam_t   PhParam;
extern State_t     gStateSM;

extern RgCntrl_t   preMbCntrl;
extern RgCntrl_t   gMbCntrl;
extern RgCntrl_t   gMbStatus;
extern RgCntrl_t   gMbActiveCntrl;
extern CntrlCellsRg_t  gCntrlCell;
extern bActivRg_t      gActiveReg;
extern RgCntrl_t   gMbSlaveSt[ NUMBERS_CELLS ];

void ProcessInit( void );

void SM_loop( void );

#ifdef __cplusplus
}
#endif

#endif 
/** (END OF FILE  : StateMashine.hpp) 
*******************************/ 


#ifndef _USER_MB_M_APP_
#define _USER_MB_M_APP_

#include "user_mb_app_m.h"

#ifdef __cplusplus
extern "C" {
#endif  

// регистр состояния чоппера REG_R_STATE_UNIT
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
}RegStatusM_t;  

/*
* (51)REG_W_CNTRL_PROCESS // рег. сост. процесса соотвествует (4)REG_R_STATE_PROCESS  или регистр режима работы
*/
typedef struct
{
  uint16_t bWaiting   : 1;
  uint16_t bBurn      : 1;  // fair
  uint16_t bCutting   : 1;
  uint16_t bExtinguishing   :1;// suppression   
  uint16_t bTrueHole : 1;  
}Process_t;

typedef union
{
  uint16_t rg;
  Process_t bit;
}RegStatusProces_t;

void UpDateReadRgM( uint16_t addr, eMBReg_t numRg );
void UpDateWriteRgM( uint16_t addr, eMBReg_t numRg );
void CntrCellsStatus( uint16_t addr, FlagStatus st);

#ifdef __cplusplus
}
#endif  
#endif
  
  
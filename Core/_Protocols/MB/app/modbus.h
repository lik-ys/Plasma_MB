#ifndef _USER_MB_APP_
#define _USER_MB_APP_

#include  <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  
  
typedef enum _MB_REG
{
  REG_W_CNTRL   = 0 ,
  REG_W_FREQ    ,
  REG_W_CNC_OUT , // дискретные выходы
  
  REG_R_STATUS  ,
  REG_R_ADC_CURR1 ,
  REG_R_ADC_CURR2 ,
  REG_R_ADC_VOLT  ,
  REG_R_CNC_IN  , // дискретные входы 
  
  REG_LAST
  
}eMBReg_t;

union STATUS
{
  uint16_t reg;
  struct {
    //FREG_ON,
    uint16_t CMD_FIRE :1;
    uint16_t CMD_FIRE_PWR:1;
    uint16_t CMD_FIRE_LOCK:1;
    uint16_t CMD_GAS_FIRE:1;
    uint16_t CMD_GAS_EXTINCTION:1;
    uint16_t CMD_GAS_OUT:1;
    uint16_t CMD_GAS_WAITING:1;
    uint16_t CMD_CHOPPER_FIRE:1;
    uint16_t CMD_CHOPPER_OUT:1;
    uint16_t CMD_CHOPPER_EXTINCTION:1;
    uint16_t CMD_CHOPPER_WAITING:1;
  }bit;  
}RegStatus_t;

union CNC_IN
{
  uint16_t cnc_in;
  struct{
    uint16_t in0 : 1;
    uint16_t in1 : 1;
    uint16_t in2 : 1;
  }bin;
} CncIn_t;

union CNC_OUT
{
  uint16_t cnc_out;
  struct {
    uint16_t out0 : 1;
    uint16_t out1 : 1;
    uint16_t out2 : 1;
    uint16_t out3 : 1;
  }CncOut;
} CncOut_t;

void UpDateReadRg( eMBReg_t numRg );
void UpDateWriteRg( eMBReg_t numRg );

#ifdef __cplusplus
}
#endif
#endif
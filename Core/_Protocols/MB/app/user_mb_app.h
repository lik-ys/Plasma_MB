#ifndef _USER_MB_APP_
#define _USER_MB_APP_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"

/* -----------------------Slave Defines -------------------------------------*/
/*
 */
typedef enum _MB_REG_S
{
	REG_W_CNTRL		= 1,
	REG_W_PWM		= 2,
	REG_W_CURR		= 3, 
    REG_W_SLOP_1    = 4, 
    REG_W_SLOP_2    = 5,
    REG_W_P         = 6,
    REG_W_I         = 7,
    REG_W_D         = 8,
    
    
    REG_W_CNC_OUT       = 16,// дискретные выходы  
    REG_W_FREQ          = 17,// „астота синхронизации 12345 √ц      
    // reserv  
    REG_R_STATUS       = 50,//
    REG_R_STATUS1      = 51,//    
    REG_R_VOLT         = 52,
    REG_R_CURR_1       = 53,
    REG_R_CURR_2       = 54, //

	REG_S_LAST                               //
} eMBRegS_t;


#define S_DISCRETE_INPUT_START        0
#define S_DISCRETE_INPUT_NDISCRETES   1//16

#define S_COIL_START                  0
#define S_COIL_NCOILS                 0//64

#define S_REG_INPUT_START             0
#define S_REG_INPUT_NREGS             2//100

#define S_REG_HOLDING_START           0
#define S_REG_HOLDING_NREGS           REG_S_LAST//100

/* salve mode: holding register's all address */
#define          S_HD_RESERVE                     0
#define          S_HD_CPU_USAGE_MAJOR             1
#define          S_HD_CPU_USAGE_MINOR             2
/* salve mode: input register's all address */
#define          S_IN_RESERVE                     0
/* salve mode: coil's all address */
#define          S_CO_RESERVE                     0
/* salve mode: discrete's all address */
#define          S_DI_RESERVE                     0

extern void     SetMBRgS( eMBRegS_t numMBReg, uint16_t data );
extern uint16_t GetMBRgS( eMBRegS_t numMBReg );

//REG_R_STATUS_S
typedef union STATUS
{
  uint16_t reg;
  struct {    
    uint16_t CMD_FIRE               :1;
    uint16_t CMD_FIRE_PWR           :1;
    uint16_t CMD_FIRE_LOCK          :1;
    uint16_t CMD_GAS_FIRE           :1;
    uint16_t CMD_GAS_EXTINCTION     :1;
    uint16_t CMD_GAS_OUT            :1;
    uint16_t CMD_GAS_WAITING        :1;
    uint16_t CMD_CHOPPER_FIRE       :1;
    uint16_t CMD_CHOPPER_OUT        :1;
    uint16_t CMD_CHOPPER_EXTINCTION :1;
    uint16_t CMD_CHOPPER_WAITING    :1;
    uint16_t CMD_FREG_ON            :1;
  }bit;  
}RegStatus_t;

//typedef union CNC_IN
//{
//  uint16_t cnc_in;
//  struct{
//    uint16_t in0 : 1;
//    uint16_t in1 : 1;
//    uint16_t in2 : 1;
//  }bin;
//} CncIn_t;

//typedef union CNC_OUT
//{
//  uint16_t cnc_out;
//  struct {
//    uint16_t out0 : 1;
//    uint16_t out1 : 1;
//    uint16_t out2 : 1;
//    uint16_t out3 : 1;
//  }CncOut;
//} CncOut_t;

void UpDateReadRg( eMBRegS_t numRg );
void UpDateWriteRg( eMBRegS_t numRg );

#ifdef __cplusplus
}
#endif

#endif // _USER_MB_APP_

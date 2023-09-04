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
	REG_R_CURR_1		= 0x0000 ,
	REG_R_CURR_2		= 0x0001 ,
	REG_R_VOLT			= 0x0002 ,
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

#ifdef __cplusplus
}
#endif

#endif // _USER_MB_APP_

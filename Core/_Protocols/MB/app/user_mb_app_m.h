#ifndef _USER_MB_MASTER_APP_
#define _USER_MB_MASTER_APP_

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"


/*
 * address of VFD
 */
typedef enum _MB_ADDR
{
	MB_Addr_reel	 = 0x01	,
	MB_Addr_platform = 0x02 ,
	MB_Addr_last
}mb_addr_t;

#define CNT_MB_ERROR_THR	20

//
/*
 * MB_MASTER_TOTAL_SLAVE_NUM
 * https://plc247.com/fx3u-modbus-rtu-ls-ig5a-vfd-tutorial/
 * ЧП отдает только по 8 регистров
 */
typedef enum _MB_REG
{
	REG_R_MODEL_INF			= 0x0000 ,
	REG_R_INV_POW			= 0x0001 ,
	REG_R_INV_VOLT			= 0x0002 ,
	REG_R_SW_VER			= 0x0003 ,
	REG_RW_LOCK				= 0x0004 ,
	REG_RW_FREQ				= 0x0005 ,
	REG_WR_RUN_CMD			= 0x0006 , // 0 - Stop, 1 - Forward, 2 - Reverse
	REG_RW_ACC_TIME			= 0x0007 ,

	REG_RW_DEC_TIME			= 0x0008 ,
	REG_R_OUT_CUR			= 0x0009 ,
	REG_R_OUT_FREQ			= 0x000A ,
	REG_R_OUT_VOLT			= 0x000B ,
	REG_R_DC_LINE_VOLT		= 0x000C ,
	REG_R_OUT_POWER		    = 0x000D ,
	REG_R_STATUS			= 0x000E ,
	REG_R_TRIP_INFO			= 0x000F , // внутреннее состояние

	REG_R_IN_SW_STATE		= 0x0010 ,
	REG_R_OUT_SW_STATE		= 0x0011 ,
	REG_R_ANI_V1			= 0x0012 ,
	REG_R_ANI_V2			= 0x0013 , // Not Used (с ростом V1 растёст и V2)
	REG_R_ANI_I				= 0x0014 , //
	REG_R_PRM				= 0x0015 ,

	REG_R_LAST_READ 		= 0x0016 ,
	// TODO
	REG_READ_ADDR_RG		= 0x0100 ,
	REG_WRITE_ADDR_RG		= 0x0108 ,
	// ...
	REG_LAST                               //
} eMBReg_t;


typedef struct
{
	uint32_t error;
	uint32_t rx;
	uint32_t tx;
	uint32_t ex; // execute
}mb_cnt_t;

typedef struct
{
	uint8_t execute  : 1; //
	uint8_t request  : 1; // запрос выполнен
	uint8_t response : 1; // ответ получен
	uint8_t error	 : 1; //
	uint8_t err_time_out:1;//
}mb_action_t;

extern mb_action_t 	mb_act;
extern mb_cnt_t 	mb_cnt;

#if ( 1== MB_MASTER_RTU_ENABLED )
/* -----------------------Master Defines -------------------------------------*/

#define M_DISCRETE_INPUT_START        0
#define M_DISCRETE_INPUT_NDISCRETES   1//16

#define M_COIL_START                  0
#define M_COIL_NCOILS                 0//64

#define M_REG_INPUT_START             0
#define M_REG_INPUT_NREGS             0x0016// REG_R_LAST_READ

#define M_REG_HOLDING_START           0
#define M_REG_HOLDING_NREGS           0x0016

/* master mode: holding register's all address */
#define          M_HD_RESERVE                     0
/* master mode: input register's all address */
#define          M_IN_RESERVE                     0
/* master mode: coil's all address */
#define          M_CO_RESERVE                     0
/* master mode: discrete's all address */
#define          M_DI_RESERVE                     0

extern USHORT   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];

void MBMasterRecieved(void );
void MBMasterTransmite(void );
void MBMasterError(void );
void MBMasterExec(void );
void MBMasterErrorTO( void );



#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif

#endif // _USER_MB_MASTER_APP_

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
 * address of slave devices
 */
typedef enum _MB_ADDR
{
	MB_cell_1 = 0x01 ,
	MB_cell_2 = 0x02 ,
	MB_cell_3,
    MB_cell_4,
    MB_cell_5,
    MB_cell_6
}mb_addr_t;

#define CNT_MB_ERROR_THR	20
/*
 * регистры подчиненных одинаковы
 */
typedef enum _MB_REG_SLAVE
{
  REG_R_CURR_1s          = 0,   // Токи двух каналов
  REG_R_CURR_2s          ,      // Уставка тока  REG_W_SET_OUT_CURRENT должна быть равна сумме токов вв двух каналах
  REG_R_IN_VOLTs         ,
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
  REG_W_TIME7             ,//100
  REG_W_TIME8             ,
  REG_W_TIME9             ,
  REG_W_TIME10            ,
  REG_W_TIME12            ,//104  

  REG_W_FIRST_DAC         ,//105 точки начала и конца пилы ЦАП
  REG_W_LAST_DAC          ,//104
  
  REG_LASTs = REG_W_LAST_DAC + 40         //
} eMBReg_t;// _MB_REG


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
#define M_REG_INPUT_NREGS             0// REG_R_LAST_READ

#define M_REG_HOLDING_START           0
#define M_REG_HOLDING_NREGS           REG_LASTs

/* master mode: holding register's all address */
#define          M_HD_RESERVE                     0
/* master mode: input register's all address */
#define          M_IN_RESERVE                     0
/* master mode: coil's all address */
#define          M_CO_RESERVE                     0
/* master mode: discrete's all address */
#define          M_DI_RESERVE                     0

extern uint16_t   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];

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

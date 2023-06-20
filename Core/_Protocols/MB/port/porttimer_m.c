#include "port.h"
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- User defenitions ---------------------------------*/
TIM_HandleTypeDef *tim_m;
static uint16_t timeout = 0;
volatile uint16_t counter_m = 0;

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTimersInit( USHORT usTimeOut50us, void *dHTIM )
{
	tim_m = (TIM_HandleTypeDef *)dHTIM;
	timeout = usTimeOut50us;
    return TRUE;
}

void vMBMasterPortTimersT35Enable()
{
    vMBMasterSetCurTimerMode(MB_TMODE_T35);
	counter_m=0;
	HAL_TIM_Base_Start_IT(tim_m);
}

void vMBMasterPortTimersConvertDelayEnable()
{
	vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
	HAL_TIM_Base_Start_IT(tim_m);
	counter_m=0;
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
	vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
	HAL_TIM_Base_Start_IT(tim_m);
	counter_m=0;
}

void vMBMasterPortTimersDisable()
{
	HAL_TIM_Base_Stop_IT(tim_m);
}

void MBMasterPortCBTimerExpired(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == tim_m->Instance)
	{
		if((++counter_m) >= timeout)
			pxMBMasterPortCBTimerExpired();
	}
}


#endif

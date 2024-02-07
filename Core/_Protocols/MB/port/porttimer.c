#include "port.h"
#include "mb.h"
#include "mbport.h"
#include "main.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0

/* ----------------------- User defenitions ---------------------------------*/
TIM_HandleTypeDef *tim;
static uint16_t timeout = 0;
volatile uint16_t counter = 0;

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us, void *dHTIM )
{
	tim = (TIM_HandleTypeDef *)dHTIM;
	timeout = usTim1Timerout50us;
	return TRUE;
}

inline void vMBPortTimersEnable( void )
{
  __HAL_TIM_SET_COUNTER( pMBTimSlave, 1);  

  __HAL_TIM_CLEAR_FLAG( pMBTimSlave, TIM_FLAG_UPDATE );
  
  if ( HAL_OK != HAL_TIM_Base_Stop( pMBTimSlave ) )
  {
    Error_Handler();
  }else;
  
  if ( HAL_OK == HAL_TIM_Base_Start_IT( pMBTimSlave ))
  {  
  }else
  {
    Error_Handler();
  }
}

inline void vMBPortTimersDisable(  )
{
	HAL_TIM_Base_Stop_IT(tim);
}
/*
*
*/
void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *htim )
{
	if( htim == pMBTimSlave)
	{
      if((++counter) >= timeout)   {
			pxMBPortCBTimerExpired();
            counter = 0;
      }
	}else;
    if (htim == pMBTimMaster){
      MBMasterPortCBTimerExpired( htim );
      
    }else;
}

#endif

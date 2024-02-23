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
  __HAL_TIM_SET_COUNTER( tim_m, 1);  

  __HAL_TIM_CLEAR_FLAG( tim_m, TIM_FLAG_UPDATE );
  
  if ( HAL_OK != HAL_TIM_Base_Stop( tim_m ) )
  {
    Error_Handler();
  }else;
  
  if ( vMBMasterGetCurTimerMode() == MB_TMODE_RESPOND_TIMEOUT )
  {
//counter_m = 1; 
  }else 
   //counter_m = 0; 
;
  vMBMasterSetCurTimerMode(MB_TMODE_T35);  
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
  if ( HAL_OK != HAL_TIM_Base_Stop_IT( tim_m ) )
  {
    Error_Handler();
  }else;  

  __HAL_TIM_SET_COUNTER( tim_m, 1);  
  __HAL_TIM_CLEAR_FLAG( tim_m, TIM_FLAG_UPDATE );  
  
  HAL_GPIO_WritePin( Test0_GPIO_Port, Test0_Pin, GPIO_PIN_SET );
  vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);  

  if ( HAL_OK != HAL_TIM_Base_Start_IT(tim_m) )
  {
    Error_Handler();
  }else;
  
  
  if ( HAL_OK != HAL_TIM_Base_Stop_IT( &htim5 ) )
  {
    Error_Handler();
  }else;    
// psc 1000 arr = 1000 = 12mc| arr = 3000 = 36мс
  __HAL_TIM_SET_COUNTER( &htim5, 1);  
  __HAL_TIM_CLEAR_FLAG( &htim5, TIM_FLAG_UPDATE ); 
    
  if ( HAL_OK != HAL_TIM_Base_Start_IT( &htim5 ) )
  {
    Error_Handler();
  }else;   
  
//  
//  for (uint32_t i = 100000;i>0;i--)__NOP();
//  __NOP();
//  __NOP();
//  //counter_m = 2;

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
      {
            HAL_GPIO_WritePin( Test0_GPIO_Port, Test0_Pin, GPIO_PIN_RESET );
			pxMBMasterPortCBTimerExpired();   
            counter_m = 0;  
      }
	}
    if ( htim5.Instance == htim->Instance )
    {
            HAL_GPIO_WritePin( Test0_GPIO_Port, Test0_Pin, GPIO_PIN_RESET );
			pxMBMasterPortCBTimerExpired();
            __NOP();
            __NOP();
            __NOP();
    }
}


#endif

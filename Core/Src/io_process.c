/*
 * io_process.c
 *
 *  input - output processing
 *
 *  Created on: 01 июн. 2023 г.
 *      Author: luch
 */

#include "main.h"
#include "io_process.h"

#define CNT_TO_DIR_RS485 1
   
st_led_t gLed = { led0_pin, 0 };
/*
 *
 */
void RS485_Dir( rs485_tx_rx_t tx_rx )
{
	if ( tx == tx_rx ){
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		HAL_GPIO_WritePin( DIR_RS1_GPIO_Port, DIR_RS1_Pin, GPIO_PIN_SET );
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
	}
	else{
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		HAL_GPIO_WritePin( DIR_RS1_GPIO_Port, DIR_RS1_Pin, GPIO_PIN_RESET );
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
	}
}//RS485_Dir()

/*
 *
 */
void RS485_Dir_m( rs485_tx_rx_t tx_rx )
{
	if ( tx == tx_rx ){
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		HAL_GPIO_WritePin( DIR_RS2_GPIO_Port, DIR_RS2_Pin, GPIO_PIN_SET );
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
	}
	else{
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		HAL_GPIO_WritePin( DIR_RS2_GPIO_Port, DIR_RS2_Pin, GPIO_PIN_RESET );
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
	}
}//RS485_Dir()

/*
 *
 */
void RS485_Dir2( rs485_tx_rx_t tx_rx )
{
	if ( tx == tx_rx ){
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		///HAL_GPIO_WritePin( LD6_MB_OIEN_GPIO_Port, LD6_MB_OIEN_Pin, GPIO_PIN_SET );
		HAL_GPIO_WritePin( DIR_RS2_GPIO_Port, DIR_RS2_Pin, GPIO_PIN_SET );
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
	}
	else{
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		///HAL_GPIO_WritePin( LD6_MB_OIEN_GPIO_Port, LD6_MB_OIEN_Pin, GPIO_PIN_RESET );
		HAL_GPIO_WritePin( DIR_RS2_GPIO_Port, DIR_RS2_Pin, GPIO_PIN_RESET );
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
	}
}//RS485_Dir()

/**
*
**/
void SetLed( st_led_t *sled )
{
  HAL_GPIO_WritePin( GPIOE, sled->led, GPIO_PIN_SET ); 
  sled->st = 1;  
}

void ClrLed( st_led_t *sled )
{
  HAL_GPIO_WritePin( GPIOE, sled->led, GPIO_PIN_RESET ); 
  sled->st = 0;
}

/**
*
**/
void ToggleLed( st_led_t *sled )
{
  if ( sled->st )
  {
    HAL_GPIO_WritePin( GPIOE, sled->led, GPIO_PIN_RESET );    
    sled->st = 0;
  }
  else
  {
    HAL_GPIO_WritePin( GPIOE, sled->led, GPIO_PIN_SET );
    sled->st = 1;
  }
}// ToggleLed()

__weak void HAL_UART_TxCpltCallback( UART_HandleTypeDef *huart)
{
  	__enable_irq();
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch( GPIO_Pin )    //  todo дребезг
  {
    case COMM_START_Pin: 
      gStateSM.st.bCommStart = 1;  
      __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
      break;
    case DI_METAL_CONTACT_Pin:     
      gStateSM.st.bMetalContact = 1;     
      __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
      break;
    case COMM_FIRE_Pin:
      gStateSM.st.bCommFire     = 1;
      __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
      break;    
    default:
      break;
  } // switch(  )
}

/** (END OF FILE  : io_process.c) 
*******************************/ 

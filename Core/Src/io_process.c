/*
 * io_process.c
 *
 *  input - output processing
 *
 *  Created on: 01 θών. 2023 γ.
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

/** (END OF FILE  : io_process.c) 
*******************************/ 

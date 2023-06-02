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


#define CNT_TO_DIR_RS485 5000
/*
 *
 */
void RS485_Dir( rs485_tx_rx_t tx_rx )
{
	if ( tx == tx_rx ){
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		///HAL_GPIO_WritePin( LD6_MB_OIEN_GPIO_Port, LD6_MB_OIEN_Pin, GPIO_PIN_SET );
		HAL_GPIO_WritePin( DIR_RS1_GPIO_Port, DIR_RS1_Pin, GPIO_PIN_SET );
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
	}
	else{
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		///HAL_GPIO_WritePin( LD6_MB_OIEN_GPIO_Port, LD6_MB_OIEN_Pin, GPIO_PIN_RESET );
		HAL_GPIO_WritePin( DIR_RS1_GPIO_Port, DIR_RS1_Pin, GPIO_PIN_RESET );
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
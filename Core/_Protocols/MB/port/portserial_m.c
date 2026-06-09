/*
 * portserial_m.c
 *
 *
 *  Created on: 07 aprel 2022 г.
 *      Author: luch
 */

#include "port.h"
#include "mb.h"
#include "mbport.h"
#include "io_process.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/
UART_HandleTypeDef *uart_m;
static uint8_t singlechar_m;

/* ----------------------- User defenitions ---------------------------------*/
#define RS485_MASTER_RTS_LOW	{}//HAL_GPIO_WritePin(RS485_RTS_GPIO_Port, RS485_RTS_Pin, GPIO_PIN_RESET)
#define RS485_MASTER_RTS_HIGH 	{}//HAL_GPIO_WritePin(RS485_RTS_GPIO_Port, RS485_RTS_Pin, GPIO_PIN_SET)

/* ----------------------- Start implementation -----------------------------*/

BOOL xMBMasterPortSerialInit(void *dHUART, ULONG ulBaudRate, void *dHTIM)
{
	uart_m = (UART_HandleTypeDef *)dHUART;

	return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if(xRxEnable)
	{
		RS485_Dir_m(rx);
		HAL_UART_Receive_IT(uart_m, &singlechar_m, 1);
	}	
	else
	{
		HAL_UART_AbortReceive_IT(uart_m);
	}

	if(xTxEnable)
	{
		RS485_Dir_m(tx);
		pxMBMasterFrameCBTransmitterEmpty();
	}
	else
	{
		HAL_UART_AbortTransmit_IT(uart_m);
	}
}

void vMBMasterPortClose(void)
{
	HAL_UART_AbortReceive_IT(uart_m);
	HAL_UART_AbortTransmit_IT(uart_m);
}

BOOL xMBMasterPortSerialPutBytes(volatile UCHAR *ucByte, USHORT usSize)
{
	HAL_UART_Transmit_IT(uart_m, (uint8_t *)ucByte, usSize);
	return TRUE;
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
	HAL_UART_Transmit_IT(uart_m, (uint8_t*)&ucByte, 1);
	return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = (uint8_t)(singlechar_m);
	return TRUE;
}
//
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart_m)
//{
//	if(huart->Instance == uart_m->Instance)
//	{
//		pxMBMasterFrameCBByteReceived();
//		HAL_UART_Receive_IT(uart_m, &singlechar_m, 1);
//	}
//}
//
//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(huart->Instance == uart_m->Instance)
//	{
//		pxMBMasterFrameCBTransmitterEmpty();
//	}
//}

#endif

#include "port.h"
#include "mb.h"
#include "mbport.h"
#include "io_process.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0
/*-------------------------- For Master -------------------------------------*/
extern uint8_t singlechar_m;
extern UART_HandleTypeDef *uart_m;
/* ----------------------- Static variables ---------------------------------*/
static UART_HandleTypeDef *uart;
static uint8_t singlechar;
/* ----------------------- User defenitions ---------------------------------*/
#define RS485_RTS_LOW	 HAL_GPIO_WritePin(DIR_RS1_GPIO_Port, DIR_RS1_Pin, GPIO_PIN_RESET)
#define RS485_RTS_HIGH 	 HAL_GPIO_WritePin(DIR_RS1_GPIO_Port, DIR_RS1_Pin, GPIO_PIN_SET)


/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortSerialInit( void *dHUART, ULONG ulBaudRate, void *dHTIM )
{
	uart = (UART_HandleTypeDef *)dHUART;

	return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if(xRxEnable)
	{
        HAL_UART_AbortReceive_IT(uart);
        RS485_Dir(rx);
        HAL_UART_Receive_IT(uart, &singlechar, 1); 
	}	
	else
	{
		HAL_UART_AbortReceive_IT(uart);
	}

	if(xTxEnable)
	{
		RS485_Dir(tx);
		pxMBFrameCBTransmitterEmpty();
	}
	else
	{
		HAL_UART_AbortTransmit_IT(uart);
	}
}

void vMBPortClose(void)
{
	HAL_UART_AbortReceive_IT(uart);
	HAL_UART_AbortTransmit_IT(uart);
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	HAL_UART_Transmit_IT(uart, (uint8_t*)&ucByte, 1);
	return TRUE;
}

BOOL xMBPortSerialPutBytes(volatile UCHAR *ucByte, USHORT usSize)
{
	HAL_UART_Transmit_IT(uart, (uint8_t *)ucByte, usSize);
	return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = (uint8_t)(singlechar);
	return TRUE;
}

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
	if(huart->Instance == uart->Instance)
	{
		pxMBFrameCBByteReceived();
		HAL_UART_Receive_IT(uart, &singlechar, 1);
	}
	if(huart->Instance == uart_m->Instance)
	{
		pxMBMasterFrameCBByteReceived();
		HAL_UART_Receive_IT(uart_m, &singlechar_m, 1);
	}    
}

void HAL_UART_TxCpltCallback( UART_HandleTypeDef *huart )
{
	if(huart->Instance == uart->Instance)
	{
		pxMBFrameCBTransmitterEmpty();
	}
	if( huart->Instance == uart_m->Instance )
	{
		pxMBMasterFrameCBTransmitterEmpty();
	}    
}

#endif

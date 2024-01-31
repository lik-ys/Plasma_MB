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

#define CNT_TO_DIR_RS485    200

port_t  Input[ InLast ] = {
  { CNC_DI_0_Pin, CNC_DI_0_GPIO_Port},
  { CNC_DI_1_Pin, CNC_DI_1_GPIO_Port},
  { CNC_DI_2_Pin, CNC_DI_2_GPIO_Port},
  { CNC_DI_3_Pin, CNC_DI_3_GPIO_Port},
  { FIRE_DI_0_Pin, FIRE_DI_0_GPIO_Port},
  { DI_METAL_CONTACT_Pin, DI_METAL_CONTACT_GPIO_Port}, // todo CNC - умеет сама отпрыгивать от площадки
  { COMM_FIRE_Pin, COMM_FIRE_GPIO_Port  }
};

port_t CncOut[ cnc_last ] = {
  { CNC_DO_0_Pin, CNC_DO_0_GPIO_Port},
  { CNC_DO_1_Pin, CNC_DO_1_GPIO_Port},
  { CNC_DO_2_Pin, CNC_DO_2_GPIO_Port},
  { CNC_DO_3_Pin, CNC_DO_3_GPIO_Port}
};

port_t CmdOut[ cmd_last ] = {   // TODO
  { CMD_CHOPPER_FIRE_Pin,        CMD_CHOPPER_FIRE_GPIO_Port},
  { CMD_CHOPPER_CUT_Pin,         CMD_CHOPPER_CUT_GPIO_Port},
  { CMD_CHOPPER_EXTINCTION_Pin,  CMD_CHOPPER_EXTINCTION_GPIO_Port},
  { CMD_CHOPPER_WAITING_Pin,     CMD_CHOPPER_WAITING_GPIO_Port},
  { CMD_GAS_Waiting_Pin,         CMD_GAS_Waiting_GPIO_Port},
  { CMD_GAS_Extinction_Pin,      CMD_GAS_Extinction_GPIO_Port},
  { CMD_GAS_Out_Pin,             CMD_GAS_Out_GPIO_Port},
  { CMD_GAS_Fire_Pin,            CMD_GAS_Fire_GPIO_Port}  
};
   

io_t gIO;
st_led_t gLed = { led0_pin, 0 };

/**
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

/**
*
*/
void RS485_Dir_m( rs485_tx_rx_t tx_rx )
{
	if ( tx == tx_rx ){
		//for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		HAL_GPIO_WritePin( DIR_RS2_GPIO_Port, DIR_RS2_Pin, GPIO_PIN_SET );
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
	}
	else{
		//for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
		HAL_GPIO_WritePin( DIR_RS2_GPIO_Port, DIR_RS2_Pin, GPIO_PIN_RESET );
		for (uint32_t i = CNT_TO_DIR_RS485; i>1; i--)__NOP();
	}
}//RS485_Dir()

/**
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
*/
void SetLed( st_led_t *sled )
{
  HAL_GPIO_WritePin( GPIOE, sled->led, GPIO_PIN_SET ); 
  sled->st = 1;  
}
/**
*
*/
void ClrLed( st_led_t *sled )
{
  HAL_GPIO_WritePin( GPIOE, sled->led, GPIO_PIN_RESET ); 
  sled->st = 0;
}
/**
*
*/
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

__weak
void HAL_UART_TxCpltCallback( UART_HandleTypeDef *huart)
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
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);   
  
  switch( GPIO_Pin )
  {
    case COMM_START_Pin:
      gStateSM.st.bExti = 1;
      gStateSM.st.bCommStart    = 1;  
      break;
    case DI_METAL_CONTACT_Pin:     
      gStateSM.st.bExti = 1;
      gStateSM.st.bMetalContact = 1;     
      break;
    case COMM_FIRE_Pin:
      gStateSM.st.bExti = 1;
      gStateSM.st.bCommFire     = 1;
      gIO.in.bit.ComFire  = 1;
      break; 
    case FIRE_DI_0_Pin:
      gStateSM.st.bExti = 1;
      gStateSM.st.bFireIn = 1;      
      break;
    default:
      break;
  } // switch(  )
}// 

#define CNT_IN_WRITE    5
/**
* TODO дребезг
*/
void InRead( void )
{
  static uint16_t reg = 0;
  static uint16_t cnt = 0;
  static Input_t  in  = {0,};
  
  for ( uint16_t i = 0; i < InLast ; i++ )
  {
    if ( GPIO_PIN_SET ==  HAL_GPIO_ReadPin( Input[ i ].hPort, Input[ i ].pin) )
    {
      in.reg  |=  ( 1 << i );
    }else{ 
      in.reg  &= ~( 1 << i );
    }
  }
  if ( reg != in.reg )
  {
    reg = in.reg;    
  }else
  {
    if ( cnt++ > CNT_IN_WRITE ) 
    {
      gIO.in.reg = in.reg;  
      cnt = 0;
    }else;
  }  
}// InRead( void )

/**
*
*/
void CncWrite( eCnc_out_t out, GPIO_PinState st )
{
  if ( out >= cnc_last ) return;

  if ( GPIO_PIN_SET == st )
  { 
    if ( 0 == gIO.cnc.reg | ( 1 << out) ) gIO.cnc.reg |= ( 1 << out);
    else return;
  }
  else
  {
    if ( 1 == gIO.cnc.reg | ( 1 << out) )   gIO.cnc.reg &= ~( 1 << out);
    else return;
  }
  HAL_GPIO_WritePin( CncOut[ out ].hPort, CncOut[ out ].pin, st );  
} // CncWrite()

/**
*
*/
void CmdWrite( eCmd_t out, GPIO_PinState st )
{
  if ( out >= cmd_last ) return;
  
  if ( GPIO_PIN_SET == st )
  { 
    if ( 0 == gIO.cmd.reg | ( 1 << out) ) gIO.cmd.reg |= ( 1 << out);
    else return;
  }
  else
  {
    if ( 1 == gIO.cmd.reg | ( 1 << out) ) gIO.cmd.reg &= ~( 1 << out);
    else return;
  }
  HAL_GPIO_WritePin( CmdOut[ out ].hPort, CmdOut[ out ].pin, st );  
} // CmdWrite()

/**
*
*/
void TestOut( void )
{
  static uint8_t out_pin = 0;
  if ( gMbStatus.bit.bFireStart )
  {
    CncWrite( (eCnc_out_t)out_pin, GPIO_PIN_SET );
    CmdWrite( (eCmd_t)out_pin,     GPIO_PIN_SET );
  }else
  {
    CncWrite( (eCnc_out_t)out_pin, GPIO_PIN_RESET );
    CmdWrite( (eCmd_t)out_pin,     GPIO_PIN_RESET );
  }
  if ( out_pin++ > cmd_last ) out_pin = 0;
}// TestOut()

/** (END OF FILE  : io_process.c) 
*******************************/ 

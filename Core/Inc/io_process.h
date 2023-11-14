/*
 * io_process.h
 *
 *  Created on: 01 июн. 2023 г.
 *      Author: luch
 */

#ifndef __IO_PROCESS_H__
#define __IO_PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define  WR_PORT_BIT( PORT_BASE, BitNumber ) (*(__IO uint32_t *) (PERIPH_BB_BASE | ((PORT_BASE + 0x14 - PERIPH_BASE) << 5) | ((BitNumber) << 2))) // ODR
#define  RD_PORT_BIT( PORT_BASE, BitNumber ) (*(__IO uint32_t *) (PERIPH_BB_BASE | ((PORT_BASE + 0x10 - PERIPH_BASE) << 5) | ((BitNumber) << 2))) // IDR

#define  TEST_TOGGEL_BB()  WR_PORT_BIT( GPIOE_BASE, 9 ) ^= 1  

typedef enum
{
  led0_pin = LED_WORK1_Pin,
  led1_pin = LED_WORK2_Pin,
  CNT_LEDS = 2
} leds_t;

typedef struct
{
  leds_t led;
  uint8_t st : 1; // on, off 
}st_led_t;

typedef struct
{
  uint16_t      pin;
  GPIO_TypeDef *hPort;
}port_t;//

/**
*
*/
typedef enum
{            
  cnc_in0    = 0,
  cnc_in1    = 1,
  cnc_in2    = 2,
  cnc_in3    = 3,
  Fire_in    = 4,
  MetContact = 5,
  COM_Fire, 
  InLast  
}eCntrlIn_t;

/***
*/
typedef struct
{
  union 
  {
    uint16_t reg;
    struct{
      uint8_t di_0 : 1;
      uint8_t di_1 : 1;
      uint8_t di_2 : 1;
      uint8_t di_3 : 1;
      uint8_t fireIn : 1;
      uint8_t MetCont : 1;
      uint8_t ComFire : 1;
    }bit;
  };  
  eCntrlIn_t In;  
}Input_t;// 

/***
*/
typedef enum
{    
  cnc_out0 = 0,
  cnc_out1 = 1,
  cnc_out2 = 2,
  cnc_out3 = 3,
  cnc_last
}eCnc_out_t;

/**
*
*/
typedef struct 
{
  union 
  {
    uint16_t reg;
    struct{
      uint8_t out0 : 1;
      uint8_t out1 : 1;
      uint8_t out2 : 1;
      uint8_t out3 : 1;
    }bit;
  };    
  eCnc_out_t nout;  
}CncOut_t;
/***
*/
typedef enum
{    
  cmd_chopper_fire    = 0,
  cmd_chopper_out     ,
  cmd_chopper_extinc  ,
  cmd_chopper_waiting ,

  cmd_gas_waiting     ,
  cmd_gas_Extinc      ,
  cmd_gas_out         ,
  cmd_gas_fire        ,
  
  cmd_last
  //TODO  sync_pwr          = SYNC_POWER_Pin, // ??? настроен на TIM1  
}eCmd_t;

/**
*
*/
typedef struct 
{
  union 
  {
    uint16_t reg;
    struct{
      uint8_t ch_fire  : 1;
      uint8_t ch_out   : 1;
      uint8_t ch_ext   : 1;
      uint8_t ch_wait  : 1;
      uint8_t gas_wait : 1;
      uint8_t gas_ext  : 1;
      uint8_t gas_out  : 1;
      uint8_t gas_fire : 1;      
    }bit;
  };    
  eCmd_t nout;  
}CmdOut_t;

typedef struct
{
  Input_t in;
  CmdOut_t cmd;
  CncOut_t cnc;  
}io_t;

/**
*
*/
typedef enum
{
  rx = 0,
  tx = 1
}rs485_tx_rx_t;  

void RS485_Dir( rs485_tx_rx_t tx_rx );
void RS485_Dir_m( rs485_tx_rx_t tx_rx );

void ToggleLed( st_led_t* );
void SetLed( st_led_t* );
void ClrLed( st_led_t* );

extern  st_led_t gLed;
extern  port_t   CncOut[ ];
extern  port_t   Input[ ];
extern  port_t   CmdOut[ ];

void InRead( void );
void CncWrite( eCnc_out_t out, GPIO_PinState st );
void CmdWrite( eCmd_t out, GPIO_PinState st );

extern io_t gIO;

#ifdef __cplusplus
}
#endif

#endif
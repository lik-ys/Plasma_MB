/*
 * io_process.h
 *
 *  Created on: 01 θών. 2023 γ.
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
	rx = 0,
	tx = 1
}rs485_tx_rx_t;  
  

void RS485_Dir( rs485_tx_rx_t tx_rx );

#ifdef __cplusplus
}
#endif

#endif
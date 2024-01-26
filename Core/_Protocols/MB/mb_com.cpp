/*
 * mb.cpp
 *
 *  Created on: 23 сент. 2022 г.
 *      Author: luch
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>
#include <string.h>

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "config.h"
#include "main.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mbport.h"
#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"

#include "mb_com.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#if MB_MASTER_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_MASTER_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_MASTER_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

#include "io_process.h"

ModBusCom MB_cntrl( ModBusCom::master ); // связь с чопперами
ModBusCom MB_hl( ModBusCom::slave );  // связь с ПК

ModBusCom *pMBcntrl = &MB_cntrl;
ModBusCom *pMBhl = &MB_hl;


//
//__weak
//void xSetMAsterEvent(eMBMasterEventType eEvent)
//{
//	eQueuedEvent = eEvent;
//}

ModBusCom :: ModBusCom( type_t t )
{
  type = t;
  addr = MB_cell_4;
} // ModBusCom

void ModBusCom::Init(void )
{
  if (type == slave )
  {
    eMBInit( MB_RTU,MB_ADDR_SLAVE, pMBSlave, SLAVE_BAUD_RATE, &htim3 );
    eMBEnable();
    RS485_Dir( rx );
  }
  else 
  if (type == master)
  {
    eMBMasterInit( MB_RTU, pMBMaster, MASTER_BAUD_RATE, &htim4 );
    eMBMasterEnable( );
	RS485_Dir_m( tx );
  }  
}// Init()

/*
*
*/
bool ModBusCom::Loop( void )
{
  if ( type == slave )
  {
    gMBErrorCode = eMBPoll( );
  }
  else 
  if ( type == master )
  {
    if ( 0 == eMBMasterIsEnabled()) return false;
    
    
    Read(addr);
    Inc();
    
    gMBErrorCode = eMBMasterPoll( );
	xGetMasterEvent( &gMBEvent);

	switch( gMBEvent )
	{
	case EV_MASTER_INIT:
	case EV_MASTER_READY                  :
		return TRUE;
		break;
	case EV_MASTER_FRAME_RECEIVED         : //mb_cnt.rx++;
		addr = (mb_addr_t)ucMBMasterGetDestAddress();
        //get_status( (mb_addr_t)addr );
		return TRUE;
		break;
	case EV_MASTER_EXECUTE                :
	case EV_MASTER_FRAME_SENT             : //mb_cnt.tx++;
		gMBEvent = EV_MASTER_READY;
	case EV_MASTER_ERROR_PROCESS          : //mb_cnt.error++;
		gMBEvent = EV_MASTER_READY;
	case EV_MASTER_PROCESS_SUCESS         : //mb_cnt.ex++;
		gMBEvent = EV_MASTER_READY;		
	case EV_MASTER_ERROR_EXECUTE_FUNCTION ://mb_cnt.error++;
		gMBEvent = EV_MASTER_READY;
		return TRUE;        
        
	case EV_MASTER_ERROR_RESPOND_TIMEOUT  ://mb_cnt.error++;
		gMBEvent = EV_MASTER_READY;
	case EV_MASTER_ERROR_RECEIVE_DATA     ://mb_cnt.error++;
		gMBEvent = EV_MASTER_READY;
		return FALSE;
	}    
  }
  return FALSE;  
} // Loop()

/*
**
*/
bool ModBusCom::Hr_query( mb_addr_t mb_addr, eMBReg_t saddr_rg )
{
	bool ret = true;
	if ( mb_act.response )
	{
		mb_act.response = 0;
		gMBMasterReqErrCode = eMBMasterReqReadHoldingRegister( mb_addr, saddr_rg - MB_RG_OFF_SET, 2, MB_TIME_OUT );
	}else ret = false;
	return ret;
}// Hr_query(); 

/*
 *
 */
bool ModBusCom::Hr_write( mb_addr_t mb_addr, eMBReg_t rg, uint16_t data)
{
  gMBMasterReqErrCode = eMBMasterReqWriteHoldingRegister( mb_addr, rg - MB_RG_OFF_SET, data, MB_TIME_OUT );
  return TRUE;
}// write()

/*
**
*/
bool ModBusCom::Read(mb_addr_t mb_addr)
{
   return Hr_query( mb_addr, static_cast<eMBReg_t>(REG_R_CURR_1s) );
}

void ModBusCom::Inc( void ) 
{
    if ( ++addr > MB_cell_end ) 
      addr = MB_cell_1;
}

ModBusCom::~ModBusCom(){}


#ifdef __cplusplus
}
#endif
/* --------------------------- End of file ------------------------------------ */



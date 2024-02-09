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
#include "user_mb_app_m.h"
#include "user_mb_app.h"
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
  if (t == slave ) addr = MB_cell_1; // адрес подчиненного для опроса ВУ
  if (t == master) addr = MB_cell_5;
} // ModBusCom

void ModBusCom::Init(void )
{
  if (type == slave )
  {
    eMBInit( MB_RTU,MB_ADDR_SLAVE, pMBSlave, SLAVE_BAUD_RATE, pMBTimSlave );
    eMBEnable();
    RS485_Dir( rx );
  }
  else 
  if (type == master)
  {
    eMBMasterInit( MB_RTU, pMBMaster, MASTER_BAUD_RATE, pMBTimMaster );
    eMBMasterEnable( );
	RS485_Dir_m( tx );
    SetRcvIdleState();
    MBMasterRecieved();
    xMBMasterPortEventPost(EV_MASTER_READY);
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
    
    //Read();
    Write();
    
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
      //WR_DEBUG("EV_MASTER_EXECUTE \r\n");
        return TRUE;
	case EV_MASTER_FRAME_SENT             : //mb_cnt.tx++;
		gMBEvent = EV_MASTER_READY;
        break;
	case EV_MASTER_ERROR_PROCESS          : //mb_cnt.error++;
		gMBEvent = EV_MASTER_READY;
	case EV_MASTER_PROCESS_SUCESS         : //mb_cnt.ex++;
		gMBEvent = EV_MASTER_READY;		
	case EV_MASTER_ERROR_EXECUTE_FUNCTION ://mb_cnt.error++;
		gMBEvent = EV_MASTER_READY;
        xSetMAsterEvent(gMBEvent);
		return TRUE;        
        
	case EV_MASTER_ERROR_RESPOND_TIMEOUT  ://mb_cnt.error++;
		gMBEvent = EV_MASTER_READY;
	case EV_MASTER_ERROR_RECEIVE_DATA     ://mb_cnt.error++;
		gMBEvent = EV_MASTER_READY;
        xSetMAsterEvent(gMBEvent);
		return FALSE;
	}    
  }
  return FALSE;  
} // Loop()

/**
**
*/
bool ModBusCom::Hr_query( mb_addr_t mb_addr, eMBReg_t saddr_rg )
{
	bool ret = true;

    gMBMasterReqErrCode = eMBMasterReqReadHoldingRegister( mb_addr, saddr_rg, 8, MB_TIME_OUT );
    // if (gMBMasterReqErrCode )...

	return ret;
}// Hr_query(); 

/**
 **
 */
bool ModBusCom::Hr_write( mb_addr_t mb_addr, eMBReg_t rg, uint16_t data)
{
  gMBMasterReqErrCode = eMBMasterReqWriteHoldingRegister( mb_addr, rg , data, MB_TIME_OUT );
  return TRUE;
}// write()

/**
**
*/
bool ModBusCom::Read( void )
{
  static uint16_t saddr = 0;
  bool res = false;
  if ( mb_act.response )
  {
    mb_act.response = 0;
    
    if ( ++saddr >= MB_cell_end )  saddr = 1;
    this->addr  = static_cast<mb_addr_t>(saddr);
    res = Hr_query( this->addr, static_cast<eMBReg_t>(REG_R_CURR_1s) );
  }else;// ret = false;  
  return res;
}// Read()

#include "mbport.h"  
#include "mb_m.h"
/***
**  write to slave cell
*/
bool ModBusCom::Write( void )
{
  bool ret = 0;
  static eMBMasterEventType   	eEvent = EV_MASTER_INIT;
  
  // xMBMasterPortEventGet(&eEvent);
//  if ( FALSE == xMBMasterPortEventGet(&eEvent) ) {
//    xMBMasterPortEventPost(EV_MASTER_READY);
//    xMBMasterPortEventGet(&eEvent);
//  }
  
//  if ((eEvent == EV_MASTER_FRAME_SENT) ) return 0;
  
  //if (gStateSM.time.b100ms)
  //  gActiveReg.rgPWM = 1;
  if (gActiveReg.rg) 
  {
    if ( gActiveReg.rgCNTRL){
    }
    if ( gActiveReg.rgPWM )  
    {
      this->addr  = static_cast<mb_addr_t>(0);
      gActiveReg.rgPWM = 0;
      //eEvent = xMasterEventGet(this->addr);
      //if (( EV_MASTER_EXECUTE == eEvent ) || ( EV_MASTER_INIT == eEvent ))
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_R_PWM1), GetMBRgS(REG_W_PWM) );
    }
    if ( gActiveReg.rgCURR) 
    {
      gActiveReg.rgCURR = 0;
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_R_CURR_1s), GetMBRgS(REG_W_CURR) );
    }
    if ( gActiveReg.rgSLOP_1){
      gActiveReg.rgSLOP_1 = 0;      
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_FIRST_DAC), GetMBRgS(REG_W_SLOP_1) );      
    }
    if ( gActiveReg.rgSLOP_2){
      gActiveReg.rgSLOP_2 = 0;
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_LAST_DAC), GetMBRgS(REG_W_SLOP_2) );    
    }
    if ( gActiveReg.rgP){
      gActiveReg.rgP = 0;
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_PID_P), GetMBRgS(REG_W_P) );      
    }
    if ( gActiveReg.rgI){
      gActiveReg.rgI = 0;
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_PID_I), GetMBRgS(REG_W_I) );
    }    
    if ( gActiveReg.rgD){
      gActiveReg.rgD = 0;
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_PID_D), GetMBRgS(REG_W_D) );
    }     
  }  
  return ret;
}// Write()

ModBusCom::~ModBusCom(){}

#ifdef __cplusplus
}
#endif
/* --------------------------- End of file ------------------------------------ */



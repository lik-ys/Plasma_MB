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

const ModBusCom MB_cntrl( ModBusCom::master ); // связь с чопперами
const ModBusCom MB_hl( ModBusCom::slave );  // связь с ПК

const ModBusCom *pMBcntrl = &MB_cntrl;
const ModBusCom *pMBhl = &MB_hl;


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
  if (t == master) addr = MB_cell_1; // Последняя ячейка в подчиненных
  

  counter  = 0;  
  b_read_en_dis     = 1;
  b_write_en_dis    = 1; 
  b_en_dis   = 1;

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
    for (uint16_t addr = 0; addr < MB_MASTER_TOTAL_SLAVE_NUM; addr++ )  MBMasterRecieved( addr );
    xMBMasterPortEventPost(EV_MASTER_READY);
  }  
}// Init()


/*
*
*/
bool ModBusCom::Loop( void )
{
  if ( this->type == slave )
  {
    if (0 == this->b_en_dis) return false;
    gMBErrorCode = eMBPoll( );
    this->counter++;
  }
  else 
  if ( this->type == master )
  {
    if ( 0 == this->b_en_dis ) return false;
    this->counter++;
    if ( 0 == eMBMasterIsEnabled()) return false;
    
    Read();
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
  }else
  {
    this->type = slave;
    WR_DEBUG("Error ModBusCom::Loop() \r\n");
    return FALSE;
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
{ // TODO зафиксировать номер регистра. при получении ответа проверить номер 
  // TODO НОвая запись только после получения ответа
  gMBMasterReqErrCode = eMBMasterReqWriteHoldingRegister( mb_addr, rg , data, MB_TIME_OUT );
  return TRUE;
}// write()

/**
**
*/
bool ModBusCom::Read( void )
{
  if ( gActiveReg.rg ) return false;
  if ( 0 == b_read_en_dis ) return false;
  static uint16_t saddr = 0;
  bool res = false;
  if ( mb_act.response )
  {
    mb_act.response = 0;
    
    //if ( ++saddr >= MB_cell_end )  saddr = 1;
    //this->addr  = static_cast<mb_addr_t>(3);
    res = Hr_query( this->addr, static_cast<eMBReg_t>(REG_R_CURR_1s) );  /// TODO без опроса запись работает с первого раза
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
  if ( isMBmRequest() )  return false;
  if (0 == b_write_en_dis) return false;
  bool ret = 0;
  static eMBMasterEventType   	eEvent = EV_MASTER_INIT;
  
  if ( gActiveReg.rg ) 
  {
    HAL_Delay(10);  // BUG : 
    static uint16_t saddr = 0;
    //this->addr  = static_cast<mb_addr_t>(3);  // 
    if ( gActiveReg.rgCNTRL)
    {
      if ( ++saddr >= MB_cell_end )  {
        saddr = 0;
        gActiveReg.rgCNTRL = 0; return 1;
      }      
      this->addr  = static_cast<mb_addr_t>(saddr);
      uint16_t rgCntrl;      
      rgCntrl = GetMBRgM((uint16_t)this->addr, REG_W_CONTROL);
      rgCntrl |= (1<<0);
      SetMBRgM((uint16_t)this->addr, REG_W_CONTROL, rgCntrl );
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_CONTROL), GetMBRgS(REG_W_CNTRL) );      
    }else;
    if ( gActiveReg.rgPWM )  
    {      
      if ( ++saddr >= MB_cell_end )  {
        saddr = 0;
        gActiveReg.rgPWM = 0;return 1;
      }
      this->addr  = static_cast<mb_addr_t>(saddr);
      //eEvent = xMasterEventGet(this->addr);
      //if (( EV_MASTER_EXECUTE == eEvent ) || ( EV_MASTER_INIT == eEvent ))
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_SET_OUT_PWM), GetMBRgS(REG_W_PWM) );
    }
    if ( gActiveReg.rgCURR) 
    {
      if ( ++saddr >= MB_cell_end )  {
        saddr = 0;
        gActiveReg.rgCURR = 0; return 1;
      }      
      this->addr  = static_cast<mb_addr_t>(saddr);            
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_SET_OUT_CURRENT), GetMBRgS(REG_W_CURR) );
    }
    if ( gActiveReg.rgSLOP_1){
      if ( ++saddr >= MB_cell_end )  {
        saddr = 0;
        gActiveReg.rgSLOP_1 = 0; return 1;
      }      
      this->addr  = static_cast<mb_addr_t>(saddr);                        
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_FIRST_DAC), GetMBRgS(REG_W_SLOP_1) );      
    }
    if ( gActiveReg.rgSLOP_2){
      if ( ++saddr >= MB_cell_end )  {
        saddr = 0;
        gActiveReg.rgSLOP_2 = 0;return 1;
      }                  
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_LAST_DAC), GetMBRgS(REG_W_SLOP_2) );    
    }
    if ( gActiveReg.rgP){
      if ( ++saddr >= MB_cell_end )  {
        saddr = 0;
        gActiveReg.rgP = 0;return 1;
      }            
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_PID_P), GetMBRgS(REG_W_P) );      
    }
    if ( gActiveReg.rgI){
      if ( ++saddr >= MB_cell_end )  {
        saddr = 0;
        gActiveReg.rgI = 0;return 1;
      }      
      ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_PID_I), GetMBRgS(REG_W_I) );
    }    
    if ( gActiveReg.rgD){
      if ( ++saddr >= MB_cell_end )  {
        saddr = 0;
        gActiveReg.rgD = 0;return 1;
      }      
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



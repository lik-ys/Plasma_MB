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
#include "CommandExec.hpp"


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
#include "Timer.hpp"
  
extern Timer *hTimer ;  

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
    for (uint16_t addr = 0; addr < MB_MASTER_TOTAL_SLAVE_NUM; addr++ )  MBMasterInit( addr );
    xMBMasterPortEventPost(EV_MASTER_READY);
    hTimer->Time_Out( Timer::start, PERIOD_MB_MASTER_TO, EV_WRITE_MBM);
    hTimer->Time_Out( Timer::start, PERIOD_READ_TO, EV_READ_TO);
  }  
}// Init()

/*
*
**/
static void CompareReg( uint16_t addr )
{
  if ( 0 == gActiveReg.rg )
  {
    if ( GetMBRgS(REG_W_CNTRL) != GetMBRgM(addr, REG_W_CONTROL) )
    {
      gActiveReg.rgCNTRL = 1;
      gProblemAddr = addr + 1;
    }
    if ( GetMBRgS( REG_W_PWM ) != GetMBRgM( addr,REG_R_PWM ) )
    {
      gActiveReg.rgPWM = 1;
      gProblemAddr = addr + 1;
    }
    if ( GetMBRgS( REG_W_SLOP_1) != GetMBRgM( addr,REG_R_FIRST_DAC ) )
    {
      gActiveReg.rgSLOP_1 = 1;
      gProblemAddr = addr + 1;
    }
    if ( GetMBRgS( REG_W_SLOP_2 ) != GetMBRgM( addr,REG_R_LAST_DAC ) )
    {
      gActiveReg.rgSLOP_2 = 1;
      gProblemAddr = addr + 1;
    }   
    //if ( addr == gProblemAddr )
  }
}//CompareReg()
/*
*
**/
static void CompareSets( void )
{
  if ( gCells.bit.bCell_1 ) CompareReg( 0 );
  if ( gCells.bit.bCell_2 ) CompareReg( 1 );
  if ( gCells.bit.bCell_3 ) CompareReg( 2 );
  if ( gCells.bit.bCell_4 ) CompareReg( 3 );
  if ( gCells.bit.bCell_5 ) CompareReg( 4 );
  if ( gCells.bit.bCell_6 ) CompareReg( 5 );
}

// Повторная передача бита управления 
void TestCntrl( void )
{
  if (  gMbCntrl.bit.bOnOffPwr != gMbStatus.bit.bOnOffPwr )
  {
    gActiveReg.rgCNTRL = 1;
  }else;
  
  if ( gMbStatus.bit.bOnOffPwr )
  {
    if ( gCells.bit.bCell_1 ){
      if (1 != gMbSlaveSt[0].bit.bOnOffPwr )
        gActiveReg.rgCNTRL = 1;
    }
    if ( gCells.bit.bCell_2 ){
      if (1 != gMbSlaveSt[1].bit.bOnOffPwr )
        gActiveReg.rgCNTRL = 1;
    }    
    if ( gCells.bit.bCell_3 ){
      if (1 != gMbSlaveSt[2].bit.bOnOffPwr )
        gActiveReg.rgCNTRL = 1;
    }
    if ( gCells.bit.bCell_4 ){
      if (1 != gMbSlaveSt[3].bit.bOnOffPwr )
        gActiveReg.rgCNTRL = 1;
    }
    if ( gCells.bit.bCell_5 ){
      if (1 != gMbSlaveSt[4].bit.bOnOffPwr )
        gActiveReg.rgCNTRL = 1;
    }
    if ( gCells.bit.bCell_6 ){
      if (1 != gMbSlaveSt[5].bit.bOnOffPwr )
        gActiveReg.rgCNTRL = 1;
    }    
  }
    
} // 
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
    CompareSets();
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
    ///if (gMBMasterReqErrCode )...

	return ret;
}// Hr_query(); 

/**
 **
 */
bool ModBusCom::Hr_write( mb_addr_t mb_addr, eMBReg_t rg, uint16_t data)
{ // TODO зафиксировать номер регистра. при получении ответа проверить номер 
  // TODO НОвая запись только после получения ответа
  if (mb_addr)
  { 
    if ( 1 == gMBactM[ mb_addr - 1 ].request)  return 0;
    MBMasterTransmite( mb_addr );
  }
  gMBMasterReqErrCode = eMBMasterReqWriteHoldingRegister( mb_addr, rg , data, MB_TIME_OUT );
  hTimer->Time_Out( Timer::start, PERIOD_MB_MASTER_TO, EV_WRITE_MBM );
  hTimer->Time_Out( Timer::start, PERIOD_REQUEST_TO, EV_REQUEST_TO );
  return TRUE;
}// write()

/**
**
*/
bool ModBusCom::Read( void )
{
  if ( gActiveReg.rg ) return false;
  if ( 0 == b_read_en_dis ) return false;
  if ( 0 ==  hTimer->IsTimeOut( EV_READ_TO) ) return false;
  
  static uint16_t saddr = 1;
  bool res = false;
  //saddr = 6;
  if ( isMBmRead(saddr) )
  {    
    this->addr  = static_cast<mb_addr_t>(saddr);
    res = Hr_query( this->addr, static_cast<eMBReg_t>(REG_R_CURR_1s) );  /// TODO без опроса запись работает с первого раза
  }else;// ret = false;  
  if ( ++saddr >= MB_cell_end )  saddr = 1;
  return res;
}// Read()

/* 
* актуальный адрес записи, получаем из анализа 
* статуса операции МБ и актиного бита записи
*/
uint16_t GetActualAddr( uint16_t  saddr)
{  
  static  uint16_t cnt_error = 0;     
  if ( isMBError(saddr) )    // ошибка записи -> повторяем передачу
  {         
    WR_DEBUG("--MB_error!!! CNT = %i Repeat write !!! cell == %i \r\n", cnt_error, saddr);  // -> repeat N
    if (++cnt_error > 10 ) 
    {
      cnt_error = 0;
      if ( ++saddr >= MB_cell_end ) 
        saddr = 0;
    }        
    return saddr;
  }else  
  if ( 0 != gProblemAddr )  // ячейка с несовпадением содержимого регистра -> повторяем передачу
  {
    saddr = gProblemAddr; 
    gProblemAddr = 0;
    return saddr;
  }else{
    cnt_error = 0;
    if ( ++saddr >= MB_cell_end ) saddr = 0;
    return saddr;
  }
}
#include "mbport.h"  
#include "mb_m.h"
/***
**  write to slave cell  TODO проверять usMRegHoldBufх[][] включен ли ШИМ
*/
bool ModBusCom::Write( void )
{
  if ( 0 ==  hTimer->IsTimeOut( EV_WRITE_MBM) ) return false;
  if ( isMBmRequest( this->addr) )  
  {
    if ( 1 ==  hTimer->IsTimeOut( EV_REQUEST_TO) )
    {
      ClrMBmRequest( this->addr );
    }else return false;  // 
  }
  else;
  if (0 == b_write_en_dis) return false;
  bool ret = 0;
  static eMBMasterEventType   	eEvent = EV_MASTER_INIT;
  
  if ( gActiveReg.rg ) 
  {    
    if ( gActiveReg.rgCNTRL)
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr )  {gActiveReg.rgCNTRL = 0;}      
      else{
        this->addr  = static_cast<mb_addr_t>(saddr);
        uint16_t rgCntrl;      
        rgCntrl = GetMBRgM((uint16_t)this->addr, REG_W_CONTROL);
        rgCntrl |= (1<<0);
        SetMBRgM((uint16_t)this->addr, REG_W_CONTROL, rgCntrl );
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_CONTROL), GetMBRgS(REG_W_CNTRL) );
        WR_DEBUG("Addr = %i \r\n", this->addr );
        return 1;      
      }
    }else; 
    
    if ( gActiveReg.rgPWM )  
    {    
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr ){gActiveReg.rgPWM = 0;}
      else {
        this->addr  = static_cast<mb_addr_t>(saddr);  
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_SET_OUT_PWM), GetMBRgS(REG_W_PWM) );
        return 1;
      }
    }
    if ( gActiveReg.rgCURR ) 
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr )  { gActiveReg.rgCURR = 0; }
      else{
        this->addr  = static_cast<mb_addr_t>(saddr);            
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_SET_OUT_CURRENT), GetMBRgS(REG_W_CURR) );
        return 1;
      }
    }
    if ( gActiveReg.rgSLOP_1)
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr( saddr );
      if ( 0 == saddr )  { gActiveReg.rgSLOP_1 = 0; }      
      else
      {
        this->addr  = static_cast<mb_addr_t>(saddr);                        
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_FIRST_DAC), GetMBRgS(REG_W_SLOP_1) );      
        return 1;
      }
    }
    if ( gActiveReg.rgSLOP_2)
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr( saddr );
      if ( 0 == saddr )  { gActiveReg.rgSLOP_2 = 0;}                  
      else
      {
        this->addr  = static_cast<mb_addr_t>(saddr);
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_LAST_DAC), GetMBRgS(REG_W_SLOP_2) );    
        return 1;
      }
    }
    if ( gActiveReg.rgP )
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr( saddr);
      if ( 0 == saddr )  {gActiveReg.rgP = 0;}            
      else{
        this->addr  = static_cast<mb_addr_t>(saddr);
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_PID_P), GetMBRgS(REG_W_P) );      
        return 1;
      }
    }
    if ( gActiveReg.rgI )
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr )  {gActiveReg.rgI = 0;}      
      {
        this->addr  = static_cast<mb_addr_t>(saddr);
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_PID_I), GetMBRgS(REG_W_I) );
        return 1;
      }
    }    
    if ( gActiveReg.rgD )
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr )  {gActiveReg.rgD = 0;}
      else{
        this->addr  = static_cast<mb_addr_t>(saddr);
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_PID_D), GetMBRgS(REG_W_D) );
        return 1;
      }
    } 
    if (gActiveReg.rgTimeSlopRise)
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr )  {gActiveReg.rgTimeSlopRise = 0;}
      else{
        this->addr  = static_cast<mb_addr_t>(saddr);
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_Ws_TIME_CUR_SLOPE_R), GetMBRgS(REG_W_TIME_CUR_SLOPE_R) );
        return 1;
      }      
    }    
    if (gActiveReg.rgTimeSlopFail)
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr )  {gActiveReg.rgTimeSlopFail = 0;}
      else{
        this->addr  = static_cast<mb_addr_t>(saddr);
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_Ws_TIME_CUR_SLOPE_F), GetMBRgS(REG_W_TIME_CUR_SLOPE_F) );
        return 1;
      }      
    }
    
    if (gActiveReg.rgProcess)
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr )  {gActiveReg.rgProcess = 0;}
      else{
        this->addr  = static_cast<mb_addr_t>(saddr);
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_W_CNTRL_PROCESS), (hCmd->num) ); // передаем номер техпроцесса
        return 1;
      }      
    }    
#if (1 == REG_DEBUG ) //  for testing Reg.
    if ( gActiveReg.c1)
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr )  {gActiveReg.c1 = 0;}
      else{
        this->addr  = static_cast<mb_addr_t>(saddr);
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_R_CURR_1s), GetMBRgS(REG_R_CURR_1) );
        return 1;
      }      
    }else;
    if ( gActiveReg.c2)
    {
      static uint16_t saddr = 0; saddr =  GetActualAddr(saddr);
      if ( 0 == saddr )  {gActiveReg.c2 = 0;}
      else{
        this->addr  = static_cast<mb_addr_t>(saddr);
        ret = Hr_write(this->addr, static_cast<eMBReg_t>(REG_R_CURR_2s), GetMBRgS(REG_R_CURR_2) );
        return 1;
      }      
    }else; 
#endif
  }  
  return ret;
}// Write()

ModBusCom::~ModBusCom(){}

#ifdef __cplusplus
}
#endif
/* --------------------------- End of file ------------------------------------ */



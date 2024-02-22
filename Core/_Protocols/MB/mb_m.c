/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbrtu_m.c,v 1.60 2013/08/20 11:18:10 Armink Add Master Functions $
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>
#include <string.h>

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/

#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"
                          
#include "modbus_m.h"
   
#include "mbport.h"
#if MB_MASTER_RTU_ENABLED == 1
#include "mbrtu.h"
#else 
eMBErrorCode    eMBMasterPoll( void ){return MB_ENOERR;}
#endif
#if MB_MASTER_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_MASTER_TCP_ENABLED == 1
#include "mbtcp.h"
#endif


#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

#ifndef MB_PORT_HAS_CLOSE
#define MB_PORT_HAS_CLOSE 0
#endif

#include "io_process.h"

eMBMasterEventType eQueuedEvent_m;
BOOL               xEventInQueue;
BOOL               xNeedPoll;

/* ----------------------- Static variables ---------------------------------*/

static UCHAR    ucMBMasterDestAddress;
static BOOL     xMBRunInMasterMode = FALSE;
static eMBMasterErrorEventType eMBMasterCurErrorType;

static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED,
    STATE_ESTABLISHED,
} eMBState = STATE_NOT_INITIALIZED;

/* Functions pointer which are initialized in eMBInit( ). Depending on the
 * mode (RTU or ASCII) the are set to the correct implementations.
 * Using for Modbus Master,Add by Armink 20130813
 */
static peMBFrameSend peMBMasterFrameSendCur;
static pvMBFrameStart pvMBMasterFrameStartCur;
static pvMBFrameStop pvMBMasterFrameStopCur;
static peMBFrameReceive peMBMasterFrameReceiveCur;
static pvMBFrameClose pvMBMasterFrameCloseCur;

/* Callback functions required by the porting layer. They are called when
 * an external event has happend which includes a timeout or the reception
 * or transmission of a character.
 * Using for Modbus Master,Add by Armink 20130813
 */
BOOL( *pxMBMasterFrameCBByteReceived ) ( void );
BOOL( *pxMBMasterFrameCBTransmitterEmpty ) ( void );
BOOL( *pxMBMasterPortCBTimerExpired ) ( void );

BOOL( *pxMBMasterFrameCBReceiveFSMCur ) ( void );
BOOL( *pxMBMasterFrameCBTransmitFSMCur ) ( void );

 
/* An array of Modbus functions handlers which associates Modbus function
 * codes with implementing functions.
 */
static xMBFunctionHandler xMasterFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
	//TODO Add Master function define
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBMasterFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBMasterFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBMasterFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBMasterFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBMasterFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBMasterFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBMasterFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBMasterFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBMasterFuncReadDiscreteInputs},
#endif
};

/* ----------------------- Start implementation -----------------------------*/
eMBErrorCode eMBMasterInit( eMBMode eMode, void *dHUART, ULONG ulBaudRate, void *dHTIM )
{
    eMBErrorCode    eStatus = MB_ENOERR;

	switch (eMode)
	{
#if MB_MASTER_RTU_ENABLED > 0
	case MB_RTU:
		pvMBMasterFrameStartCur = eMBMasterRTUStart;
		pvMBMasterFrameStopCur = eMBMasterRTUStop;
		peMBMasterFrameSendCur = eMBMasterRTUSend;
		peMBMasterFrameReceiveCur = eMBMasterRTUReceive;
		pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;
		pxMBMasterFrameCBByteReceived = xMBMasterRTUReceiveFSM;
		pxMBMasterFrameCBTransmitterEmpty = xMBMasterRTUTransmitFSM;
		pxMBMasterPortCBTimerExpired = xMBMasterRTUTimerExpired;

		eStatus = eMBMasterRTUInit(dHUART, ulBaudRate, dHTIM);
		break;
#endif
#if MB_MASTER_ASCII_ENABLED > 0
		case MB_ASCII:
		pvMBMasterFrameStartCur = eMBMasterASCIIStart;
		pvMBMasterFrameStopCur = eMBMasterASCIIStop;
		peMBMasterFrameSendCur = eMBMasterASCIISend;
		peMBMasterFrameReceiveCur = eMBMasterASCIIReceive;
		pvMBMasterFrameCloseCur = MB_PORT_HAS_CLOSE ? vMBMasterPortClose : NULL;
		pxMBMasterFrameCBByteReceived = xMBMasterASCIIReceiveFSM;
		pxMBMasterFrameCBTransmitterEmpty = xMBMasterASCIITransmitFSM;
		pxMBMasterPortCBTimerExpired = xMBMasterASCIITimerT1SExpired;

		eStatus = eMBMasterASCIIInit(dHUART, ulBaudRate, dHTIM );
		break;
#endif
	default:
		eStatus = MB_EINVAL;
		break;
	}

	if (eStatus == MB_ENOERR)
	{
		if (!xMBMasterPortEventInit())
		{
			/* port dependent event module initalization failed. */
			eStatus = MB_EPORTERR;
		}
		else
		{
			eMBState = STATE_DISABLED;
		}
		/* initialize the OS resource for modbus master. */
		///vMBMasterOsResInit();
	}
	return eStatus;
}

eMBErrorCode
eMBMasterClose( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        if( pvMBMasterFrameCloseCur != NULL )
        {
            pvMBMasterFrameCloseCur(  );
        }
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterEnable( void )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    if( eMBState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        pvMBMasterFrameStartCur(  );
        eMBState = STATE_ENABLED;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

eMBErrorCode
eMBMasterDisable( void )
{
    eMBErrorCode    eStatus;

    if(( eMBState == STATE_ENABLED ) || ( eMBState == STATE_ESTABLISHED))
    {
        pvMBMasterFrameStopCur(  );
        eMBState = STATE_DISABLED;
        eStatus = MB_ENOERR;
    }
    else if( eMBState == STATE_DISABLED )
    {
        eStatus = MB_ENOERR;
    }
    else
    {
        eStatus = MB_EILLSTATE;
    }
    return eStatus;
}

BOOL
eMBMasterIsEstablished( void )
{
    if(eMBState == STATE_ESTABLISHED)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL
eMBMasterIsEnabled( void )
{
    if ( eMBState == STATE_ENABLED || eMBState == STATE_ESTABLISHED )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static struct {
    uint16_t EILLSTATE;
    uint16_t ready;
    uint16_t rcvd;
    uint16_t execute;

    uint16_t frame_send;
    uint16_t error;
    uint16_t timeout; // TODO
    uint16_t def;
    uint16_t ev_false;
}MBMcnt = { 0,0,0,0, 0,0,0,0,0 };

void MBMcntIncTO()
{
  MBMcnt.timeout++;
}

int32_t CntSucsses[ MB_MASTER_TOTAL_SLAVE_NUM ] = {0,};

eMBErrorCode
eMBMasterPoll( void )
{
    static UCHAR   *ucMBFrame;
    static UCHAR    ucRcvAddress;
    static UCHAR    ucFunctionCode;
    static USHORT   usLength;
    static eMBException eException;

    int             i , j;
    eMBErrorCode    		eStatus = MB_ENOERR;
    eMBMasterEventType   	eEvent;
    eMBMasterErrorEventType errorType = eMBMasterGetErrorType();

    /* Check if the protocol stack is ready. */
    if(( eMBState != STATE_ENABLED ) && ( eMBState != STATE_ESTABLISHED))
    {
    	MBMcnt.EILLSTATE++;
        return MB_EILLSTATE;
    }else;

	static int cntErr = MB_CNT_ERROR;
    /* Check if there is a event available. If not return control to caller.
     * Otherwise we will handle the event. */
    if( xMBMasterPortEventGet( &eEvent ) == TRUE )
    { /// TODO  BUG зависание  всегда FALSE
        switch ( eEvent )
        {
        case EV_MASTER_READY:MBMcnt.ready++;
            eMBState = STATE_ESTABLISHED;
            break;

        case EV_MASTER_FRAME_RECEIVED: 
            eStatus = peMBMasterFrameReceiveCur( &ucRcvAddress, &ucMBFrame, &usLength );
            /* Check if the frame is for us. If not ,send an error process event. */
            if ( ( eStatus == MB_ENOERR ) && ( ucRcvAddress == ucMBMasterGetDestAddress() ) )
            {
                xMasterEventFix(ucRcvAddress, EV_MASTER_EXECUTE ) ;
                ( void ) xMBMasterPortEventPost( EV_MASTER_EXECUTE ); 
                cntErr = MB_CNT_ERROR;
            	//xMBMasterPortEventGet( &eEvent );
            	eStatus = MB_ENOERR;  
                MBMcnt.rcvd++;
            }
            else
            {   
                CntSucsses[ ucMBMasterGetDestAddress()-1 ]--;
                MBMcnt.error++;
                vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
                ( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
            }
            break;

        case EV_MASTER_EXECUTE:
            MBMasterRecieved(ucMBMasterGetDestAddress()-1);
            ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
            eException = MB_EX_ILLEGAL_FUNCTION;
            /* If receive frame has exception .The receive function code highest bit is 1.*/
            if(ucFunctionCode >> 7) {
            	eException = (eMBException)ucMBFrame[MB_PDU_DATA_OFF+1];
            }
			else
			{
				for (i = 0; i < MB_FUNC_HANDLERS_MAX; i++)
				{
					/* No more function handlers registered. Abort. */
					if (xMasterFuncHandlers[i].ucFunctionCode == 0)	{
						break;
					}
					else if (xMasterFuncHandlers[i].ucFunctionCode == ucFunctionCode) {
						vMBMasterSetCBRunInMasterMode(TRUE);
						/* If master request is broadcast,
						 * the master need execute function for all slave.
						 */
						if ( xMBMasterRequestIsBroadcast() ) {
							usLength = usMBMasterGetPDUSndLength();
							for(j = 1; j <= MB_MASTER_TOTAL_SLAVE_NUM; j++){
								vMBMasterSetDestAddress(j);
								eException = xMasterFuncHandlers[i].pxHandler(ucMBFrame, &usLength);
							}
						}
						else {
							eException = xMasterFuncHandlers[i].pxHandler(ucMBFrame, &usLength);   // Ошибка обработки ответа на принятый подчиненным пакет
						}
						vMBMasterSetCBRunInMasterMode(FALSE);
						break;
					}
				}
			}
            /* If master has exception ,Master will send error process.Otherwise the Master is idle.*/
            if (eException != MB_EX_NONE) {
            	vMBMasterSetErrorType(EV_ERROR_EXECUTE_FUNCTION);
            	( void ) xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );
                CntSucsses[ucMBMasterGetDestAddress()-1]--;

            }
            else {
                xMasterEventGet(ucMBMasterGetDestAddress());
            	vMBMasterCBRequestSucess( );
            	vMBMasterRunResRelease( );
            	//xMBMasterPortEventGet( &eEvent );
            	eStatus = MB_ENOERR;
            	MBMasterExec();
                MBMcnt.execute++;
                CntSucsses[ucMBMasterGetDestAddress()-1]++;
            }
            break;

        case EV_MASTER_FRAME_SENT: MBMcnt.frame_send++;  // BUG после ресета сразу посылка кадра ???
        	/* Master is busy now. */                    // BUG 6 посылок 2 выполнения 2 таймаута (должно быть 4)
        	vMBMasterGetPDUSndBuf( &ucMBFrame );
			eStatus = peMBMasterFrameSendCur( ucMBMasterGetDestAddress(), ucMBFrame, usMBMasterGetPDUSndLength() );
			MBMasterTransmite(ucMBMasterGetDestAddress()-1);
            break;
            
        case EV_MASTER_ERROR_RESPOND_TIMEOUT:
        {
          int16_t addr = ucMBMasterGetDestAddress()-1;
          xMasterEventFix( addr, EV_MASTER_ERROR_RESPOND_TIMEOUT );
          xMBMasterPortEventPost(EV_MASTER_READY);
          MBMasterErrorTO( addr );
          //MBMcnt.timeout++;
          eStatus = MB_ETIMEDOUT;
          
          if ( addr >= 0 ) 
          {
            CntSucsses[ addr ]--;
            CntrCellsStatus( addr, RESET );//
          }
        }
          break;
          
        case EV_MASTER_ERROR_PROCESS:
        {
        	mb_cnt.error++;
        	MBMasterError();
            int16_t addr = ucMBMasterGetDestAddress()-1;
            if ( addr > 0 ) CntSucsses[ addr ]--;
            CntrCellsStatus( addr, RESET );//
        	/* Execute specified error process callback function. */
			errorType = eMBMasterGetErrorType();
			vMBMasterGetPDUSndBuf( &ucMBFrame );
			switch (errorType) {
			case EV_ERROR_RESPOND_TIMEOUT:
				vMBMasterErrorCBRespondTimeout(ucMBMasterGetDestAddress(),
						ucMBFrame, usMBMasterGetPDUSndLength());
                MBMcnt.timeout++; // TODO
				return MB_ETIMEDOUT;

			case EV_ERROR_RECEIVE_DATA:
				vMBMasterErrorCBReceiveData(ucMBMasterGetDestAddress(),
						ucMBFrame, usMBMasterGetPDUSndLength()); 
				return MB_EIO;

			case EV_ERROR_EXECUTE_FUNCTION:
				vMBMasterErrorCBExecuteFunction(ucMBMasterGetDestAddress(),
						ucMBFrame, usMBMasterGetPDUSndLength());
				return MB_EILLSTATE;
			}
			vMBMasterRunResRelease();            
        }
        	break;
        default:MBMcnt.def++;
            break;
        }
    }else
    {    	
		if ( cntErr-- < 0 )
		{   
            MBMcnt.ev_false++;
			eMBMasterEnable( );
			cntErr = MB_CNT_ERROR;
			MBMasterErrorTO(ucMBMasterGetDestAddress()-1 );
            //MBMasterRecieved();
            MBMasterError();
			SetRcvIdleState( );
		}else{}
    }

    switch (errorType)
    {
    case EV_NO_ERROR_EVENT:    		eStatus = MB_ENOERR;
    	break;
    case EV_ERROR_RESPOND_TIMEOUT:  eStatus = MB_ETIMEDOUT;
    	break;
    case EV_ERROR_RECEIVE_DATA:		eStatus = MB_EIO;
    	break;
    case EV_ERROR_EXECUTE_FUNCTION:	eStatus = MB_EIO;
    	break;
    default:;
    }
    return eStatus;
}//eMBMasterPoll()

/* Get whether the Modbus Master is run in master mode.*/
BOOL xMBMasterGetCBRunInMasterMode( void )
{
	return xMBRunInMasterMode;
}
/* Set whether the Modbus Master is run in master mode.*/
void vMBMasterSetCBRunInMasterMode( BOOL IsMasterMode )
{
	xMBRunInMasterMode = IsMasterMode;
}
/* Get Modbus Master send destination address. */
UCHAR ucMBMasterGetDestAddress( void )
{
	return ucMBMasterDestAddress;
}
/* Set Modbus Master send destination address. */
void vMBMasterSetDestAddress( UCHAR Address )
{
	ucMBMasterDestAddress = Address;
}
/* Get Modbus Master current error event type. */
eMBMasterErrorEventType eMBMasterGetErrorType( void )
{
	return eMBMasterCurErrorType;
}
/* Set Modbus Master current error event type. */
void vMBMasterSetErrorType( eMBMasterErrorEventType errorType )
{
	eMBMasterCurErrorType = errorType;
}

eMBErrorCode MBErrorCodConvert( eMBMasterReqErrCode err_code )
{
	switch ( err_code )
	{
	case MB_MRE_NO_ERR       :		return MB_ENOERR;
	case MB_MRE_NO_REG       :		return MB_ENOREG;
	case MB_MRE_ILL_ARG      :		return MB_EINVAL;
	case MB_MRE_REV_DATA     :		return MB_EIO;
	case MB_MRE_TIMEDOUT	 :		return MB_ETIMEDOUT;
	case MB_MRE_MASTER_BUSY  :		return MB_EIO;
	case MB_MRE_EXE_FUN      :		return MB_EIO;
	default:;
	}
    return MB_EIO;
}//MBErrorCodConvert()

/*
 *
 */
uint16_t GetDataMBRg( USHORT slave_addr, uint16_t reg )
{
	if ((slave_addr <= MB_cell_6) && (slave_addr >= MB_cell_1 ) )
		return usMRegHoldBuf[ slave_addr-1 ][ reg ];
	else return -1;
} // GetDataMBRg()

void vMBMasterErrorCBExecuteFunction(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength)
{
	xMBMasterPortEventPost( EV_MASTER_ERROR_EXECUTE_FUNCTION );
}

BOOL xMBMasterPortEventGet( eMBMasterEventType * eEvent )
{
	BOOL xEventHappened = FALSE;

	if( xEventInQueue )
	{
		*eEvent = (eMBMasterEventType)eQueuedEvent_m;
		xEventInQueue = FALSE;
		xEventHappened = TRUE;
	}
	return xEventHappened;
} 

typedef struct
{
  UCHAR ucSndAddr             ;
  eMBMasterEventType eEvent   ;
} CellEvent_t;

/* !!! ОТлавить событие EV_MASTER_PROCESS_SUCESS */
// для каждой ячейки пишем события 
eMBMasterEventType CellEvent[ MB_cell_end ][ EVENTS_MASTER ];
uint8_t CntEvent[MB_cell_end ]  = {0,};

/*
**
*/
eMBMasterEventType xMasterEventGet( UCHAR ucSndAddr )
{
  eMBMasterEventType ret;
  if ( CntEvent[ucSndAddr] > 0 ) CntEvent[ucSndAddr]--;
  else ret = EV_MASTER_INIT;//ucSndAddr = EVENTS_MASTER-1;  
  
  ret = CellEvent[ucSndAddr][ CntEvent[ucSndAddr] ];
  return  ret;
}// 
/* 
**
*/
void xMasterEventFix( UCHAR ucSndAddr, eMBMasterEventType eEvent )
{
  if ( MB_ADDRESS_BROADCAST == ucSndAddr ) return; // ignory broadcast
  
  if ( CntEvent[ucSndAddr] < EVENTS_MASTER ) 
  {
    CellEvent[ucSndAddr][ CntEvent[ucSndAddr] ] = eEvent;
    CntEvent[ucSndAddr]++;
  }
  else 
  {
    //CntEvent[ucSndAddr] = 0;
  }  
}// xMasterEventFix()

BOOL xMBMasterPortEventPost( eMBMasterEventType eEvent )
{
	xEventInQueue = TRUE;
	eQueuedEvent_m = (eMBMasterEventType)eEvent;
    if (EV_MASTER_PROCESS_SUCESS == eQueuedEvent_m ) 
      ;//WR_DEBUG("EV_MASTER_PROCESS_SUCESS\r\n");
	return TRUE;
}

void vMBMasterCBRequestSucess( void )
{
	xMBMasterPortEventPost(EV_MASTER_PROCESS_SUCESS);    // TODO  Если нет ответа -> vMBMasterErrorCBRespondTimeout
}

void vMBMasterRunResRelease( void )
{
	xNeedPoll = TRUE;
}
void vMBMasterErrorCBRespondTimeout(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength)
{
	//RS485_Dir_m(tx);
	xMBMasterPortEventPost(EV_MASTER_ERROR_RESPOND_TIMEOUT);
	//HAL_Delay(1);
    WR_DEBUG("RespondTimeout Addr= %i\r\n", ucDestAddress);
	//RS485_Dir_m(rx);
    
}
void vMBMasterErrorCBReceiveData(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength)
{
	xMBMasterPortEventPost(EV_MASTER_ERROR_RECEIVE_DATA);
    //
}


BOOL xMBMasterPortEventInit( void )
{
	xEventInQueue = FALSE;
	return TRUE;
}

#endif

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

#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbfunc.h"

#include "mb_com.hpp"

#include "mbport.h"
#if MB_MASTER_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_MASTER_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_MASTER_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

ModBusCom MB_cntrl(ModBusCom::master); // связь с чопперами
ModBusCom MB_hl(ModBusCom::slave);  // связь с ПК

ModBusCom *pMBcntrl = &MB_cntrl;
ModBusCom *pMBhl = &MB_hl;

ModBusCom :: ModBusCom( type_t t )
{
  type = t;
} // ModBusCom

void ModBusCom::Init(void )
{
  if (type == slave )
  {
    eMBInit( MB_RTU,10, &huart3, SLAVE_BAUD_RATE, &htim3 );
  }
  else 
  if (type == master)
  {
    eMBMasterInit( MB_RTU, &huart4, MASTER_BAUD_RATE, &htim4 );
  }  
}// Init()

void ModBusCom::Loop()
{
  if (type == slave )
  {
    gMBErrorCode = eMBPoll();
  }
  else 
  if (type == master)
  {
    gMBErrorCode = eMBMasterPoll();
  }
} // Loop()


ModBusCom::~ModBusCom(){}
/* --------------------------- End of file ------------------------------------ */



/*
 * mb_com.hpp
 *
 *  Created on: 23 сент. 2022 г.
 *      Author: luch
 */


#ifndef _MB_COM_H
#define _MB_COM_H

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>
#include <string.h>

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "mb_m.h"
/* ----------------------- Modbus includes ----------------------------------*/

#define MB_RG_OFF_SET  (eMBReg_t)1 // все адреса пришлось уменьшить на 1
  
class ModBusCom
{  
public:
  typedef enum {
    slave = 0,
    master = 1
  } type_t;
  
  ModBusCom( type_t  t);
  virtual ~ModBusCom();
  
  void Init(void);
  void Loop(void); 
  
  bool connected;
  eMBMasterReqErrCode   gMBMasterReqErrCode;
  eMBErrorCode		    gMBErrorCode;
  eMBMasterEventType	gMBEvent; 
  bool Hr_query( mb_addr_t mb_addr, eMBRegM_t saddr_rg );
  
private:
  type_t type;  
};

#ifdef __cplusplus
}
#endif

#endif
/* --------------------------- End of file ------------------------------------ */



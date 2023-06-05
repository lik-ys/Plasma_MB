/*
 * mb_com.hpp
 *
 *  Created on: 23 сент. 2022 г.
 *      Author: luch
 */

/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>
#include <string.h>

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "mb_m.h"

/* ----------------------- Modbus includes ----------------------------------*/

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
  
private:
  type_t type;  
};


/* --------------------------- End of file ------------------------------------ */



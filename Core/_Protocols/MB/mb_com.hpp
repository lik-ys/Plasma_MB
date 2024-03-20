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

  using namespace std;
  
#define MB_RG_OFF_SET  (eMBReg_t)0
#define MB_TIME_OUT     20  // NOT USE
  
class ModBusCom
{  
public:
  typedef enum {
    slave = 0,
    master = 1
  } type_t;

  // debugs bits
  bool b_read_en_dis;
  bool b_write_en_dis;
  bool b_en_dis;
  
  mb_addr_t addr; // slave MB_addr
  uint32_t counter;
  
  ModBusCom( type_t  t);
  virtual ~ModBusCom();
  
  void Init(void);
  bool Loop(void); 
  
  bool connected;
  eMBMasterReqErrCode   gMBMasterReqErrCode;
  eMBErrorCode		    gMBErrorCode;
  eMBMasterEventType	gMBEvent; 
  bool Hr_query( mb_addr_t mb_addr, eMBReg_t saddr_rg );
  bool Hr_write( mb_addr_t mb_addr, eMBReg_t rg, uint16_t data);
  bool Read(void);
  bool Write(void);
  
private:
  type_t type;  
};

// Special behavior for ++_MB_ADDR
inline _MB_ADDR operator++( _MB_ADDR c ) {

  const int i = static_cast<int>(c);
  return static_cast<_MB_ADDR>(i+1);
}

void TestCntrl( void );

// Special behavior for _MB_ADDR++
//inline _MB_ADDR operator++( _MB_ADDR &c, int ) {
//
//  _MB_ADDR res = c;
//  ++c;
//  return res;
//}

#ifdef __cplusplus
}
#endif

#endif
/* --------------------------- End of file ------------------------------------ */



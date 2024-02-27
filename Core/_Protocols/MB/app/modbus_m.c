
#include "modbus_m.h" 
#include "user_mb_app.h"
#include "user_mb_app_m.h"

RegStatusM_t         RgStatus[ MB_MASTER_TOTAL_SLAVE_NUM];
RegStatusProces_t   RgProcess[MB_MASTER_TOTAL_SLAVE_NUM ];
static int CntMBRg[6] = {0,};

#define CNT_CELL_STATUS 7
/***
**
*/
void CntrCellsStatus( uint16_t addr, FlagStatus st )
{
  static int16_t cnt = CNT_CELL_STATUS;
  if ( st )
  {
    gCells.reg |=   1 << (addr + 1);
  }else
  {
    if ( --cnt < 0) {
      gCells.reg &= ~(1 << (addr + 1)); 
      cnt = CNT_CELL_STATUS;
    }
  }
  SetMBRgS( REG_R_ST_CELLS, gCells.reg );  
}//CntrCellsStatus()

/**
  * @brief  апдейт регистров чтени€ €чеек 
  * @param
  * @retval
  */
void UpDateReadRgM( uint16_t addr, eMBReg_t numRg )
{
  CntrCellsStatus( addr, SET );
  CntMBRg[addr]++;
  switch( numRg )    // 
  {
    case REG_R_CURR_1s:   
      //SetMBRgM(addr,addr,0);
      break;
    case REG_R_CURR_2s        : break;    
    case REG_R_IN_VOLTs       : break;
    case REG_R_STATE_UNIT     : break;   
    case REG_R_STATE_PROCESS  : break;   
    case REG_R_PWM           : break;   
 
    default:
      break;
  } // switch(  )
}

/**
  * @brief  апдейт регистров записи
  * @param
  * @retval
  */
void UpDateWriteRgM( uint16_t addr, eMBReg_t numRg )
{
  CntrCellsStatus( addr, SET );
  switch( numRg )    // 
  {
    case REG_W_PID_P:break;
    case REG_W_PID_I:break;
    case REG_W_PID_D:break;
    case REG_W_SET_OUT_PWM     : break;  
    case REG_W_SET_OUT_CURRENT : break;
    case REG_W_CONTROL         : break;  
    case REG_W_CNTRL_PROCESS   : break;  
    case REG_W_FIRST_DAC       : break;
    case REG_W_LAST_DAC        : break;    
    default:
      break;
  } // switch(  )
}

//

#include "modbus_m.h" 
#include "user_mb_app.h"
#include "user_mb_app_m.h"

RegStatusM_t         RgStatus[ MB_MASTER_TOTAL_SLAVE_NUM];
RegStatusProces_t   RgProcess[MB_MASTER_TOTAL_SLAVE_NUM ];
/**
  * @brief  апдейт регистров чтения
  * @param
  * @retval
  */
void UpDateReadRgM( uint16_t addr, eMBReg_t numRg )
{
  switch( numRg )    // 
  {
    case REG_R_STATE_UNIT:
      break;
    case REG_R_STATE_PROCESS:
      break;    
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
  
  gCells.reg |= 1 << (addr + 1) ; // TODO reset bit  
  SetMBRgS( REG_R_ST_CELLS, gCells.reg);
  
  switch( numRg )    // 
  {
    case REG_R_CURR_1s:
      SetMBRgM(0,1,0);
      break;
    case REG_R_CURR_2s:
      break;    
    case REG_R_IN_VOLTs       : break;
    case REG_R_STATE_UNIT     : 
      break;   
    case REG_R_STATE_PROCESS  : 
      break;   
    case REG_R_PWM1           : break;   
    case REG_R_PWM2           : break;         
    default:
      break;
  } // switch(  )
}

//
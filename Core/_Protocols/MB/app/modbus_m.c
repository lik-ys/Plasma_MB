
#include "modbus_m.h" 
#include "user_mb_app_m.h"

/**
  * @brief  апдейт регистров чтения
  * @param
  * @retval
  */
void UpDateReadRgM( eMBReg_t numRg )
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
void UpDateWriteRgM( eMBReg_t numRg )
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

//
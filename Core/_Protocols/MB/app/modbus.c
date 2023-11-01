
#include "modbus.h" 


/**
  * @brief  апдейт регистров чтения
  * @param
  * @retval
  */
void UpDateReadRg( eMBReg_t numRg )
{
  switch( numRg )    // 
  {
    case REG_W_CNTRL:
      break;
    case REG_R_STATUS:
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
void UpDateWriteRg( eMBReg_t numRg )
{
  switch( numRg )    // 
  {
    case REG_W_CNTRL:
      break;
    case REG_R_STATUS:
      break;    
    default:
      break;
  } // switch(  )
}
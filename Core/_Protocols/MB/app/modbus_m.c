
#include "modbus_m.h" 
#include "user_mb_app.h"
#include "user_mb_app_m.h"

RegStatusM_t         RgStatus[ MB_MASTER_TOTAL_SLAVE_NUM];
RegStatusProces_t   RgProcess[MB_MASTER_TOTAL_SLAVE_NUM ];
static int CntMBRg[6] = {0,};

#define CNT_CELL_STATUS 25 // 15 при средней оптимизации
/***
**  addr = 0,1...5
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
  * @brief  апдейт регистров чтения ячеек 
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
    case REG_R_STATE_UNIT     : // получили регистр от ячейки - передать его в рг МБ мат.платы
    {
      RgCntrl_t tmpRg; 
      tmpRg.reg = gMbStatus.reg;      
      gMbStatus.reg = GetMBRgM( addr, numRg ); 
      gMbSlaveSt[addr].reg = gMbStatus.reg;
      //tmpRg.bit.bShortCircuit = 0;
      //gMbStatus.bit.bShortCircuit = 0;
      // Биты МБ сохраняем
      if (tmpRg.bit.bShortCircuit){
        gMbStatus.bit.bShortCircuit = tmpRg.bit.bShortCircuit;
      }                 
      if (tmpRg.bit.bPilotArc){
        gMbStatus.bit.bPilotArc = tmpRg.bit.bPilotArc;
      }
      if (tmpRg.bit.bFireStart){
        gMbStatus.bit.bFireStart = tmpRg.bit.bFireStart;
      }
      if (tmpRg.bit.bStartCNC)
      {
        gMbStatus.bit.bStartCNC = tmpRg.bit.bStartCNC;
      }
      if (tmpRg.bit.bChillerFail)
      {
        gMbStatus.bit.bChillerFail = tmpRg.bit.bChillerFail;
      }    
      //if (tmpRg.bit.bOnOffPwr) gMbStatus.bit.bOnOffPwr =1; // статус не должен моргать 
      SetMBRgS( REG_R_STATUS, gMbStatus.reg );
    }
      break;   
    case REG_R_STATE_PROCESS : break;  
     // TODO BUG: addr == 1 - чтение всех нулей ????
    case REG_R_PWM           : 
      break;
    case REG_R_FIRST_DAC     : 
      break;
    case REG_R_LAST_DAC      : 
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
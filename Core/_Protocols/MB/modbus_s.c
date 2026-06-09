/*******************************************************************************
*
* Copyright (C) 2023 Sharopin Yuri
*
* File              : modbus_s.c
* Compiler          : IAR EWA 8.32
* Version           : 0.0
* Created File      : 30.08.2023
* Last modified     : 30.08.2023
*
* Support mail      : yshar@ngs.ru
*
* Target MCU        : MCU: f407 @ (Xtal = MHz | CPUclk = MHz)
* Description       : 
*                   : 
* Hardware          : .sch .pcb
*
********************************************************************************/
/**
  * @verbatim Описание
  *
  * @endverbatim
*/
/** @addtogroup ....
  * @{
  */
/** @defgroup Name 
  * @brief bla-bla-bla
  * @{
  */ 
  
/*===============================[ SPECIAL ]===================================*/ 
                      //  Условная компиляция
 
/*===============================[ IMPORT DECLARATIONS ]=======================*/ 
                      // Все системные, библиотечные и заголовочные файлы 
/* ----------------------------- [ System includes   ]-------------------------*/
//#include   <> //  MCS 
/*-------------------------------[ Platform level    ]-------------------------*/
//#include   <> //  IDE, OS
/*-------------------------------[ Standart libs     ]-------------------------*/
//#include   <> // 
/*-------------------------------[ Application Level ]-------------------------*/
#include    "main.h"
#include    "mb.h"    
#include    "modbus_s.h"          //
/*===============================[ PRIVATE CONSTANTS ]=========================*/ 
// Все частные #defines и constants должны быть объявлены в данном разделе. 
/*===============================[ PRIVATE TYPES ]=============================*/ 
// Все частные типы, которые используются в данном исходном файле, должны быть объявлены в данном разделе.  
/*===============================[ PRIVATE VARIABLES ]=========================*/ 
// В этом разделе объявляются все частные переменные.  
/*===============================[ PUBLIC  VARIABLES ]=========================*/ 
// Все глобальные переменные должны быть заданы в этом разделе.  
/*===============================[ PRIVATE PSEUDO FUNCTIONS ]==================*/ 
// Все макросы должны быть объявлены в этом разделе.  
/*===============================[ PRIVATE FUNCTIONS ]=========================*/ 
// Все частные функции, используемые в данном исходном файле.
/** @defgroup 
  * @{
  */ 
/**
  * @}
  */
/*===============================[ PUBLIC FUNCTIONS ]==========================*/
// Все экспортируемые фукнции.
/**
  * @brief
  * @param  
  * @retval 
  */
/**
  * @brief  апдейт регистров чтения
  * @param
  * @retval
  */
void UpDateReadRg( eMBRegS_t numRg )
{

           
} // UpDateReadRg()

/*--------------[ Обработка регистров записи и управления ]-------------------*/
#define  SIZE_MBR_FIFO 128

       
/**
  * @brief  апдейт регистров записи
  * @param
  * @retval
  */
void UpDateWriteRg( eMBRegS_t numRg )
{

 
} // UpDateWriteRg( void )

/* ----------------------- Define CallBack functions  -----------------------*/

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
} // eMBRegInputCB()

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  // TODO - пока не поддерживается
    return MB_ENOREG;
} // eMBRegDiscreteCB ()

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNCoils = ( short )usNCoils;
    unsigned short  usBitOffset;

    /* Check if we have registers mapped at this block. */
    if( ( usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
        switch ( eMode )
        {
                /* Read current values and pass to protocol stack. */
            case MB_REG_READ:
                while( iNCoils > 0 )
                {
                    *pucRegBuffer++ =
                        xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                        ( unsigned char )( iNCoils >
                                                           8 ? 8 :
                                                           iNCoils ) );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;

                /* Update current register values. */
            case MB_REG_WRITE:
                while( iNCoils > 0 )
                {
                    xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
                                    ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
                                    *pucRegBuffer++ );
                    iNCoils -= 8;
                }
                break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
} // eMBRegCoilsCB ()

///////////////////////////////////////////////////////////////////////////////
// Callback функции библиотеки Modbus RTU
// Обработчик записи / чтения информации регистров обмена

eMBErrorCode
eMBRegHoldingCB( uint8_t * pucRegBuffer, uint16_t usAddress, uint16_t usNRegs, eMBRegisterMode eMode )
{
  eMBErrorCode eStatus = MB_ENOERR;
  short iRegIndex;
  // Проверка на попадание в диапазон регистров MB
  if( ( usAddress >= REG_HOLDING_START ) &&
    ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    iRegIndex = ( short )( usAddress - usRegHoldingStart );
    switch ( eMode )
    {
    case MB_REG_READ:                // Чтение Регистров

      while( usNRegs > 0 )
      {
        UpDateReadRg( (eMBReg_t)iRegIndex );  //

        *pucRegBuffer++ = ( unsigned char ) ( usRegHoldingBuf[iRegIndex] >> 8 );
        *pucRegBuffer++ = ( unsigned char ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
        usNRegs--;
        iRegIndex++;
      }
      break;

    case MB_REG_WRITE:                // Запись Регистров
      {
        uint16_t mbData;
        while( usNRegs > 0 )
        {
          mbData   = *pucRegBuffer++ << 8;
          mbData  |= *pucRegBuffer++;

          //mbData   = TestInputValue( (eMBReg_t)iRegIndex, mbData );

          usRegHoldingBuf[iRegIndex] = mbData;
          UpDateWriteRg( (eMBReg_t)iRegIndex );   // передать новые данные если они не принадлежат данному МК

          usNRegs--;

          // MB_DEBUG(" REG_W_REGULATOR_SET %i = %i \t", iRegIndex, usRegHoldingBuf[iRegIndex]);

          iRegIndex++;
          // StData |= (1<<NEW_DATA);  // пришли новые данные
        }// while()

      } // case MB_REG_WRITE
    }// if в дипазоне
  } else eStatus = MB_ENOREG;
  return eStatus;
} // eMBRegHoldingCB()
/**
  * @}
  */
/**
  * @}
  */
 
/** (END OF FILE  : modbus_s.c.c) 
*******************************/ 

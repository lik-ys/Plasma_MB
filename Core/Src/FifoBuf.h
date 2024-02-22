/*******************************************************************************
* Fifo bufer
*
********************************************************************************/

/*===============================[ REDEFINITION DEFENCE ]======================*/ 
#ifndef __FIFO_BUFFER_H__ 
#define __FIFO_BUFFER_H__
/*===============================[ SPECIAL ]===================================*/ 
                       // Ѕлок операторов условной компил¤ции

/*===============================[ IMPORT DECLARATIONS ]=======================*/

/* ----------------------------- [ System includes   ]-------------------------*/
//#include   <> // MCS
/* ------------------------------[ Platform includes ]-------------------------*/
//#include   <> // IDE, OS
/*-------------------------------[ Standart libs     ]-------------------------*/
//#include   <> // 
/* ------------------------------[ Application level ]-------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#include "Include_common.h"

/*===============================[ PUBLIC CONSTANTS ]=======================*/ 
// ¬се частные #defines и constants должны быть объ¤влены в данном разделе.
#define     FIFO_COUNTS     ( 1 << 8 )          // 
#define     FIFO_MASK       ( FIFO_COUNTS - 1 )
// TODO проверка на степень двойки
/*===============================[ TYPE DEFINITIONS ]==========================*/ 
typedef enum FIFO_STATE
{
  FIFO_FULL         , // - не принимаем
  FIFO_FREE         , // - принимаем
  FIFO_EMPTY        , // - EMPTY - буфер пуст
  FIFO_ERROR_INPUT  
  //FIFO_ERROR_BFULL      
}FifoState_t;

#pragma pack(16)
typedef struct FIFO_BUF_
{
  FifoState_t state;  // 
  int16_t     count;  //  число записанных элементов в буфере
  uint16_t    size;   //  общий размер в байтах
  uint16_t    head;   //  индекс на запись
  uint16_t    tail;   //  индекс на чтение
  uint16_t    len;    //  длина записи // //  длина элемента буфера
  void*       pBuf;   //  указатель на буфер
  uint16_t    mask;   //  маска для циклического индекса

} FifoBuf_t;
#pragma pack()
/*===============================[ EXTERN VAR ]================================*/ 

//extern    int ext_var;

/*===============================[ PSEUDO/INLINE FUNCTIONS ]===================*/ 
//inline                 // Inline функции (методы класса)
                       
/*===============================[ EXPORTED FUNCTIONS ]========================*/ 

FifoState_t PushFIFO( FifoBuf_t* pFifo, void* pInData  );
FifoState_t PopFIFO ( FifoBuf_t* pFifo, void* pOutData );
FifoState_t FlushFIFO ( FifoBuf_t* pFifo );

void TestFifo( void );
void TestFifoDac( void );

/*===============================[ END REDEFINITION DEFENCE]===================*/ 
                         // окончание однократно включаемого h-файла

#ifdef __cplusplus
}
#endif

#endif
/** (END OF FILE  : FifoBuf.h) 
*******************************/
 

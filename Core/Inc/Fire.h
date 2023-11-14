#ifndef __FIRE_H__ 
#define __FIRE_H__

#ifdef __cplusplus
extern "C" {
#endif  
  
#define TIME_OUT_PWR_ON     1000
#define TIME_HOLD           1000  // время удержавния
#define TIME_OUT_FIRE_OFF   1500 // время паузы между включениями ОГНЯ

#include "io_process.h"  
/***
* выходы управления поджигом
*/ 
typedef enum eCNTR_FIRE
{
  cmd_pwr  , // 
  cmd_lock , // == cmd_fire // работают вместе
  cmd_fire , // 

  fire_last
} eFire_t;

/***
* входы управления поджигом и стартом
*/
typedef enum eIN_FIRE
{
  fire_in       ,    // PE0 IRQ  
  comm_start    ,    // PE2 IRQ
  metal_contact ,    // PE3 IRQ
  com_fire_in   ,    // PE4 IRQ  
  
  in_fire_last
}eInFire_t;

void FireProcess( void );
void CommandProcess( void );

extern port_t gFireO[ fire_last ];
extern port_t gFireI[ in_fire_last ];

#ifdef __cplusplus
}
#endif

#endif
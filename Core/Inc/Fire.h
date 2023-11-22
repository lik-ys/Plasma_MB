#ifndef __FIRE_H__ 
#define __FIRE_H__

#ifdef __cplusplus
extern "C" {
#endif  
  
#define TIME_OUT_PWR_ON     1000
#define TIME_HOLD           1000  // врем€ удержавни€
#define TIME_OUT_FIRE_OFF   1500 // врем€ паузы между включени€ми ќ√Ќя

#include "io_process.h"  
/***
* выходы управлени€ поджигом
*/ 
typedef enum eCNTR_FIRE
{
  cmd_pwr  , // 
  cmd_lock , // == cmd_fire // работают вместе
  cmd_fire , // 
  //cmd_pilot_arc,    // выход дежурной дуги
  fire_last
} eFire_t;

/***
* входы управлени€ поджигом и стартом
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
void CmdProc( void );
void PilotArcPort( FlagStatus st );


extern port_t gFireO[ fire_last ];
extern port_t gFireI[ in_fire_last ];

#ifdef __cplusplus
}
#endif

#endif
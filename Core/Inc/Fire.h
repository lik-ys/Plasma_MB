#ifndef __FIRE_H__ 
#define __FIRE_H__

#ifdef __cplusplus
extern "C" {
#endif  
  
#define TIME_OUT_PWR_ON     800
#define TIME_HOLD           1000 // врем€ удержавни€
#define TIME_OUT_FIRE_OFF   1500 // врем€ паузы между включени€ми ќ√Ќя
#define TIME_START          1000 //
#define PA_TIME_OUT         7000
#define TIME_FIRE_START     800
#define PILOT_ARC_OFF_TO    4500 // EV_IGNITION - ждем  PILOT_ARC_OFF_TO выключаем деж. дугу.
#define PILOT_ARC_OFF_TO1   2500 //
#define SHORT_CURR_TO       1000 //

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
void ReadStart( void );

extern port_t gFireO[ fire_last ];
extern port_t gFireI[ in_fire_last ];

#ifdef __cplusplus
}
#endif

#endif
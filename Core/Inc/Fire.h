#ifndef __FIRE_H__ 
#define __FIRE_H__

#ifdef __cplusplus
extern "C" {
#endif  
  

#define TIME_OUT_PWR_ON     1000
#define TIME_HOLD           3000  // время удержавния
#define TIME_OUT_FIRE_OFF   1500 // время паузы между включениями ОГНЯ


void FireProcess( void );
void CommandProcess( void );

#ifdef __cplusplus
}
#endif

#endif
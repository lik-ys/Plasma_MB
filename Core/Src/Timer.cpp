
#include "StateMachine.h"
#include "Timer.hpp"


uint32_t Ev_timeout[ EV_COUNTS ]; // timeout for each events
uint32_t MLmsec[EV_COUNTS];		 // current time of events
uint32_t St_tim[EV_COUNTS];

Timer::~Timer()
{
}
/*
 *
 */
Timer::Timer( )
{
	ev_timeout 	= Ev_timeout;
	msec 		= MLmsec;
	st_tim 		= St_tim;
	max = EV_COUNTS;

	for (eProcessCom_t i = EV_IDLE; i < max; i = static_cast<eProcessCom_t>(i+1)  )
		st_tim[ (eProcessCom_t)i ] = not_start;

}// Timer()

/*
 *
 */
bool Timer::IsTimeOut(eProcessCom_t to )
{
	Time_Out( test, ev_timeout[to], (eProcessCom_t)to);
	if ( time_out == st_tim[ to ] )
	{
		Time_Out( start, ev_timeout[to], (eProcessCom_t)to);
		return 1;
	}else;

	return 0;
}//IsTimeOut()


/*
 *
 */
void Timer::Waiting( void )
{
	static eProcessCom_t i = (eProcessCom_t)0;
	Time_Out(test, ev_timeout[i], i);
	i = static_cast<eProcessCom_t>(i+1);
	if (  i >= EV_COUNTS ) i = (eProcessCom_t)0;
}// Waiting()


/*
 *
 */
uint32_t Timer::IsTime( eProcessCom_t ev )
{
	return ((int)HAL_GetTick() -  (int)msec[(uint8_t)ev] );
}
/*
 *
 */
uint8_t Timer::GetState( eProcessCom_t ev )
{
	if (ev > max ) return 0;
	return (uint8_t)st_tim[ ev ];
}

/*
 *
 */
uint32_t Timer::GetTime( eProcessCom_t ev )
{
	uint32_t d;
	d = delta; delta =0;
	if ( st_tim[ ev ] == not_start )
		return 0;
	else
		return  d;// ((int)HAL_GetTick() - (int)msec[(uint8_t)ev] );
}// GetTime();


/*
 *
 */
void   Timer::Time_Out( cntrl_timer_t ct, uint32_t time , eProcessCom_t ev )
{
	uint32_t tick;
	switch ( ct )
	{
	case start:
		st_tim[ ev ] = (st_tim_t)2;//(st_tim_t)time_start;
		ev_timeout[ ev ] = time;
		msec[ ev ] = HAL_GetTick();
		break;
	case test:
		if ( (0 == ev_timeout[ev] ) || (st_tim[ ev ] == not_start ))  { return;}

		tick = HAL_GetTick();
		if ( tick >= msec[(uint8_t)ev] ){
			delta  = tick -  msec[(uint8_t)ev];
		}else delta = 0;

		if (ev_timeout[(uint8_t)ev] <= delta )
		{
		  	 st_tim[ ev ] = time_out;
		  	 return;
		}else {
			 st_tim[ ev ] = time_ok; return;
		}
		break;
	case stop:
    case clr:
		ev_timeout[ (uint8_t)ev ] = 0;
		st_tim[ ev ] = not_start; 
        return;
		break;
	default:
		st_tim[ ev ] =  not_start; return;
		break;
	}
}// Time_Out()


/*
 * timer.hpp
 *
 *  Created on: 6 мая 2021 г.
 *      Author: luch
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include "main.h"

/* процесс обработки событий и комманд (звершена, подтверждение, выполняется)
 *
 */
class Timer
{
public:
	Timer(void);

	typedef enum {
		start = 0,
		test = 1,
		stop = 2,
		clr	 = 3
	}cntrl_timer_t;

	typedef enum {
		not_start,
		time_out,
		time_start,
		time_ok
	}st_tim_t;

	uint32_t IsTime(eProcessCom_t ev);
	void Time_Out( cntrl_timer_t , uint32_t time, eProcessCom_t ev );
	uint32_t GetTime( eProcessCom_t ev );
	uint8_t  GetState(eProcessCom_t ev);
	void Waiting();
	bool IsTimeOut(eProcessCom_t );

	//st_tim_t st_tim[MAV_COMMAND_LAST];
	uint32_t *st_tim;

	uint32_t *ev_timeout;
	uint32_t *msec;
	uint32_t max; // MAV_COMMAND_LAST
	uint32_t delta;
    
    ~Timer();
private:
}; // Timer

#endif /* __TIMER_H__ */

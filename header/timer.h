#ifndef FIFO_H
#define FIFO_H
#include <fifo.h>
#endif

#define SIZE_TIMERS 512

#define FLAG_TIMER_FREE 	0x0000
#define FLAG_TIMER_ALLOC	0x0001
#define FLAG_TIMER_USING	0x0002

typedef struct Timer{
	uint id;
	uint timeout;
	FIFO32Ptr fifo;
	int flag;
	struct Timer *next;
}Timer, *TimerPtr;

typedef struct TimerCtl{
	uint count;
	Timer start;
	Timer timers[SIZE_TIMERS];
	Timer end;
	TimerPtr next;
}TimerCtl, *TimerCtlPtr;


extern TimerCtl timerCtl;

void Timer__construct(uint shift);
TimerPtr Timer_alloc();
TimerPtr Timer_set_timeout(TimerPtr timerPtr, uint id, uint timeout, FIFO32Ptr fifo);
void Timer_timeout();
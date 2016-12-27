#define SIZE_TIMERS 512

#define FLAG_TIMER_FREE 	0x0000
#define FLAG_TIMER_ALLOC	0x0001
#define FLAG_TIMER_USING	0x0002

typedef struct Timer{
	uint timeout;
	int flag;
	struct Timer *next;
}Timer, *TimerPtr;

typedef struct TimerCtl{
	uint count;
	uchar buf_timeout[8];
	Timer start;
	Timer timers[SIZE_TIMERS];
	Timer end;
	TimerPtr next;
}TimerCtl, *TimerCtlPtr;


extern TimerCtl timerCtl;

void Timer__construct(TimerCtlPtr this);
TimerPtr Timer_set_timeout(TimerCtlPtr this, uint timeout);
void Timer_timeout(TimerCtlPtr this);
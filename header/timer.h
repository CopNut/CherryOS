typedef struct Timer{
	uint count;
}Timer, *TimerPtr;

extern Timer timer;

void Timer__construct(TimerPtr this);
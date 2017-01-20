#include <stdio.h>
#include <timer.h>

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

FIFO32Ptr timerfifo;
extern TimerCtl timerCtl;
uint data_shift_timer;

void Timer__construct(uint shift)
{
	uint i;
	TimerPtr timerPtr;

	data_shift_timer = shift;
	io_8bits_out(PIT_CTRL, 0x34);
	io_8bits_out(PIT_CNT0, 0x9c);
	io_8bits_out(PIT_CNT0, 0x2e);
	timerCtl.count = 0;
	timerCtl.start.next = &(timerCtl.end);
	for (i = 0; i < SIZE_TIMERS; ++i)
	{
		timerPtr = &(timerCtl.timers[i]);
		timerPtr->timeout = 0;
		timerPtr->flag = FLAG_TIMER_FREE;
		timerPtr->next = NULL;
	}
	timerCtl.end.timeout = 0xffffffff;
	timerCtl.end.flag = FLAG_TIMER_USING;
	timerCtl.end.next = NULL;
	timerCtl.next = &(timerCtl.end);
	return;
}

TimerPtr Timer_alloc()
{
	int i;
	for (i = 0; i < SIZE_TIMERS; i++) {
		if (timerCtl.timers[i].flag == FLAG_TIMER_FREE) {
			timerCtl.timers[i].flag = FLAG_TIMER_ALLOC;
			return &timerCtl.timers[i];
		}
	}
	return 0;
}

TimerPtr Timer_set_timeout(TimerPtr timerPtr, uint id, uint timeout, FIFO32Ptr fifo){

	TimerPtr p, q;
	TimerPtr timerTab = timerCtl.timers;
	uint i = 0, e;

	timerPtr->id = id;
	timerPtr->flag = FLAG_TIMER_USING;
	timerPtr->fifo = fifo;
	timerPtr->timeout = timerCtl.count + timeout;

	e = io_load_eflags();
	io_cli();

	p = &(timerCtl.start);
	q = p->next;
	while(timerPtr->timeout >= q->timeout){
		p = p->next;
		q = q->next;
	}
	//p => timerPtr => q
	p->next = timerPtr;
	timerPtr->next = q;

	//timerCtl->next
	if (timerPtr->timeout > timerCtl.count && timerPtr->timeout < timerCtl.next->timeout)
	{
		timerCtl.next = timerPtr;
	}
	io_store_eflags(e);
	return timerPtr;
}


void Timer_timeout()
{
	TimerPtr timeoutedPtr = timerCtl.next;
	timeoutedPtr->flag = FLAG_TIMER_ALLOC;
	timerCtl.next = timeoutedPtr->next;
	
	return;
}
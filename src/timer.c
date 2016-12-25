#include <stdio.h>
#include <timer.h>

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

void Timer__construct(TimerCtlPtr this)
{
	uint i;
	TimerPtr timerPtr;

	io_8bits_out(PIT_CTRL, 0x34);
	io_8bits_out(PIT_CNT0, 0x9c);
	io_8bits_out(PIT_CNT0, 0x2e);
	this->count = 0;
	this->start.next = &(this->end);
	for (i = 0; i < SIZE_TIMERS; ++i)
	{
		timerPtr = &(this->timers[i]);
		timerPtr->timeout = 0;
		timerPtr->flag = FLAG_TIMER_FREE;
		timerPtr->next = NULL;
	}
	this->end.timeout = 0xffffffff;
	this->end.flag = FLAG_TIMER_USING;
	this->end.next = NULL;
	return;
}

TimerPtr Timer_set_timeout(TimerCtlPtr this, uint timeout){

	TimerPtr p, q, new;
	TimerPtr timerTab = this->timers;
	uint i = 0, e;

	e = io_load_eflags();
	io_cli();
	while(timerTab[i].flag){
		i++;
	}
	new = &timerTab[i];
	new->flag = FLAG_TIMER_USING;
	new->timeout = timeout;


	p = &(this->start);
	q = p->next;
	while(new->timeout >= q->timeout){
		p = p->next;
		q = q->next;
	}
	//p => new => q
	p->next = new;
	new->next = q;
	io_store_eflags(e);
	return new;
}
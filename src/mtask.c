#include <stdio.h>
#include <CherryOS.h>

TimerPtr timerPtr_mt;
int mt_tr;

void MT_init(void)
{
	timerPtr_mt = Timer_alloc();
	Timer_set_timeout(timerPtr_mt, 0xffff, 2, 0xffff); //meanless id & &fifo
	mt_tr = 3 * 8;
	return;
}

void MT_taskswitch(void)
{
	if (mt_tr == 3 * 8) {
		mt_tr = 4 * 8;
	} else {
		mt_tr = 3 * 8;
	}
	Timer_set_timeout(timerPtr_mt, 0xffff, 2, 0xffff); //meanless id & &fifo
	farjmp(0, mt_tr);
	return;
}
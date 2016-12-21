#include <stdio.h>
#include <timer.h>

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

void Timer__construct(TimerPtr this)
{
	io_8bits_out(PIT_CTRL, 0x34);
	io_8bits_out(PIT_CNT0, 0x9c);
	io_8bits_out(PIT_CNT0, 0x2e);
	this->count = 0;
	return;
}
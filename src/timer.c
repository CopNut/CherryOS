#include <stdio.h>
#include <timer.h>

uint data_shift_timer;

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerCtl;

#define TIMER_FLAGS_ALLOC		1
#define TIMER_FLAGS_USING		2

void init_pit(uint shift)
{
	int i;
	struct TIMER *t;
	io_8bits_out(PIT_CTRL, 0x34);
	io_8bits_out(PIT_CNT0, 0x9c);
	io_8bits_out(PIT_CNT0, 0x2e);
	data_shift_timer = shift;
	timerCtl.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerCtl.timers0[i].flags = 0;
	}
	t = timer_alloc();
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0;
	timerCtl.t0 = t;
	timerCtl.next = 0xffffffff;
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerCtl.timers0[i].flags == 0) {
			timerCtl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerCtl.timers0[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER *timer)
{
	timer->flags = 0;
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	int e;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerCtl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	t = timerCtl.t0;
	if (timer->timeout <= t->timeout) {
		
		timerCtl.t0 = timer;
		timer->next = t;
		timerCtl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	
	for (;;) {
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout) {
			
			s->next = timer;
			timer->next = t;
			io_store_eflags(e);
			return;
		}
	}
}



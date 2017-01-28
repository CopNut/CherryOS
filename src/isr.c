#include <stdio.h>
#include <CherryOS.h>

#define PORT_KEYDAT		0x0060
#define TIMER_FLAGS_ALLOC		1	/* Šm•Û‚µ‚½ó‘Ô */
#define TIMER_FLAGS_USING		2	/* ƒ^ƒCƒ}ì“®’† */


extern FIFO32Ptr keyfifo, mousefifo;
extern uint data_shift_key, data_shift_mouse, data_shift_timer;
extern TimerPtr timerPtr_task;
extern struct TIMERCTL timerCtl;


void init_pic(void)
{
	io_8bits_out(PIC0_IMR,  0xff  );
	io_8bits_out(PIC1_IMR,  0xff  );

	io_8bits_out(PIC0_ICW1, 0x11  );
	io_8bits_out(PIC0_ICW2, 0x20  );
	io_8bits_out(PIC0_ICW3, 1 << 2);
	io_8bits_out(PIC0_ICW4, 0x01  );

	io_8bits_out(PIC1_ICW1, 0x11  );
	io_8bits_out(PIC1_ICW2, 0x28  );
	io_8bits_out(PIC1_ICW3, 2     );
	io_8bits_out(PIC1_ICW4, 0x01  );

	io_8bits_out(PIC0_IMR,  0xfb  );
	io_8bits_out(PIC1_IMR,  0xff  );

	return;
}

void inthandler20(int *esp)
{
	struct TIMER *timer;
	char ts = 0;
	io_8bits_out(PIC0_OCW2, 0x60);	/* IRQ-00Žó•tŠ®—¹‚ðPIC‚É’Ê’m */
	timerCtl.count++;
	if (timerCtl.next > timerCtl.count) {
		return;
	}
	timer = timerCtl.t0; /* ‚Æ‚è‚ ‚¦‚¸æ“ª‚Ì”Ô’n‚ðtimer‚É‘ã“ü */
	for (;;) {
		/* timers‚Ìƒ^ƒCƒ}‚Í‘S‚Ä“®ì’†‚Ì‚à‚Ì‚È‚Ì‚ÅAflags‚ðŠm”F‚µ‚È‚¢ */
		if (timer->timeout > timerCtl.count) {
			break;
		}
		/* ƒ^ƒCƒ€ƒAƒEƒg */
		timer->flags = TIMER_FLAGS_ALLOC;
		if (timer != timerPtr_task) {
			FIFO32_put(timer->fifo, timer->data + data_shift_timer);
		} else {
			ts = 1; /* task_timer‚ªƒ^ƒCƒ€ƒAƒEƒg‚µ‚½ */
		}
		timer = timer->next; /* ŽŸ‚Ìƒ^ƒCƒ}‚Ì”Ô’n‚ðtimer‚É‘ã“ü */
	}
	timerCtl.t0 = timer;
	timerCtl.next = timer->timeout;
	if (ts != 0) {
		Task_switch();
	}
	return;
}

void inthandler21(int *esp)
/* PS/2 keyboard */
{
	unsigned char data;
	io_8bits_out(PIC0_OCW2, 0x61); /*CPU interrupt acknowledged*/
	data = io_8bits_in(PORT_KEYDAT); /*catch the keyboard data*/
	FIFO32_put(keyfifo, data + data_shift_key);
	
	return;
}

void inthandler2c(int *esp)
/* PS/2 mouse */
{
	unsigned char data;
	io_8bits_out(PIC1_OCW2, 0x64);
	io_8bits_out(PIC0_OCW2, 0x62);
	data = io_8bits_in(PORT_KEYDAT);
	FIFO32_put(mousefifo, data + data_shift_mouse);
	return;
}

void inthandler27(int *esp)
{
	io_8bits_out(PIC0_OCW2, 0x67); 
	return;
}
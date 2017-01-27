#include <stdio.h>
#include <keyboard.h>

FIFO32Ptr keyfifo;
uint data_shift_key;

void Keyboard__construct(Keyboard *this, FIFO32Ptr fifo, uint shift)
{
	keyfifo = fifo;
	data_shift_key = shift;
	this->aim = 0;
	Keyboard_wait_KBC_sendready();
	io_8bits_out(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	Keyboard_wait_KBC_sendready();
	io_8bits_out(PORT_KEYDAT, KBC_MODE);
	return;
}

void Keyboard_wait_KBC_sendready(void)
{
	for (;;) {
		if ((io_8bits_in(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

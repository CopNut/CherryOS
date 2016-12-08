#include <stdio.h>
#include <keyboard.h>

void Keyboard__construct(Keyboard *this)
{
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

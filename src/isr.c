#include <stdio.h>
#include <CherryOS.h>

#define PORT_KEYDAT		0x0060


struct FIFOB keyfifo, mousefifo;


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
//PIT timer
{
	io_8bits_out(PIC0_OCW2, 0x60);
	timerCtl.count++;
	return;
}

void inthandler21(int *esp)
/* PS/2 keyboard */
{
	unsigned char data;
	io_8bits_out(PIC0_OCW2, 0x61); /*CPU interrupt acknowledged*/
	data = io_8bits_in(PORT_KEYDAT); /*catch the keyboard data*/
	fifob_put(&keyfifo, data);
	
	return;
}

void inthandler2c(int *esp)
/* PS/2 mouse */
{
	unsigned char data;
	io_8bits_out(PIC1_OCW2, 0x64);
	io_8bits_out(PIC0_OCW2, 0x62);
	data = io_8bits_in(PORT_KEYDAT);
	fifob_put(&mousefifo, data);
	return;
}

void inthandler27(int *esp)
{
	io_8bits_out(PIC0_OCW2, 0x67); 
	return;
}
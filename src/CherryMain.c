#include <stdio.h>
#include "CherryOS.h"

extern struct FIFOB keyfifo, mousefifo;
Screen screen;
Keyboard keyboard = {
	.buf_code = {0}
};
Mouse mouse = {
	.buf_code = {0},
	.buf_dcode = {0},
	.bg = {0}
};
Memory memory = {
	.freeMemBlock = {0}
};


void CherryMain() {
	Bootinfo *binfo = (BootinfoPtr)ADDR_BOOTINFO;


	char str[100];
	char buf[160];
	unsigned char data;

	gdt_install();
	idt_install();
	init_pic();
	io_sti();

	fifob_init(&keyfifo, 32, keyboard.buf_code);
	fifob_init(&mousefifo, 128, mouse.buf_code);
	io_8bits_out(PIC0_IMR, 0xf9);
	io_8bits_out(PIC1_IMR, 0xef);

	Memory__construct(&memory, MEM_CHECK_START, MEM_CHECK_END);
	Memory_free(&memory, 0x00001000, 0x0009c000);
	Memory_free(&memory, 0x00400000, memory.physize - 0x00400000);

	Keyboard__construct(&keyboard);
	Screen__construct(&screen, binfo, BCOLOR);
	Mouse__construct(&mouse, binfo);

	get_box(binfo->vram, binfo->xsize, mouse.px, mouse.py, mouse.bg, mouse.xsize, mouse.ysize);
 	put_box(binfo->vram, binfo->xsize, mouse.px, mouse.py, mouse.cursor, mouse.xsize, mouse.ysize);

 	sprintf(str, "(%d,%d)", mouse.px, mouse.py);
 	put_string(binfo->vram, binfo->xsize, 0, 0, str, BLACK);

 	sprintf(str, "MEMSIZE_PHY = %dMB", memory.physize / 1024 / 1024);
 	put_string(binfo->vram, binfo->xsize, 0, 50, str, BLACK);
 	sprintf(str, "MEMSIZE_FREE = %dMB", memory.freesize / 1024 / 1024);
 	put_string(binfo->vram, binfo->xsize, 0, 70, str, BLACK);

	Mouse_enable();


	while(1)
	{
		io_cli();
		if (fifob_status(&keyfifo) + fifob_status(&mousefifo) == 0){
			io_stihlt();
		}else if(fifob_status(&keyfifo) != 0){
			data = fifob_get(&keyfifo);
			io_sti();
			sprintf(str, "%x", data);
			fill_box(binfo->vram, binfo->xsize, 0, 16, BCOLOR, 30, 16);
			put_string(binfo->vram, binfo->xsize, 0, 16, str, BLACK);
		}else if(fifob_status(&mousefifo) != 0){
			data = fifob_get(&mousefifo);
			io_sti();

			if(Mouse_dcode(&mouse, data)){
				sprintf(str, "%x, %x, %x", mouse.button, mouse.rx, mouse.ry);
				fill_box(binfo->vram, binfo->xsize, 30, 16, BCOLOR, 120, 16);
				put_string(binfo->vram, binfo->xsize, 30, 16, str, BLACK);

				sprintf(str, "(%d,%d)", mouse.px, mouse.py);
				fill_box(binfo->vram, binfo->xsize, 0, 0, BCOLOR, 120, 16);
				put_string(binfo->vram, binfo->xsize, 0, 0, str, BLACK);

				Mouse_move(&mouse, &screen);
			}

		}
		
	}
}















#include <stdio.h>
#include <CherryOS.h>

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
ShtCtl shtCtl = {
	.sheets = {0},
	.sheets0 = {0}
};
TimerCtl timerCtl;


void CherryMain() {
	Bootinfo *binfo = (BootinfoPtr)ADDR_BOOTINFO;
	Memory *memory = (Memory *)ADDR_MEMBUF;
	Sheet *sheetBg, *sheetMouse;
	TimerPtr timerPtr;

	char str[100];
	char buf[160];
	unsigned char data;


	gdt_install();
	idt_install();
	init_pic();
	io_sti();

	fifob_init(&keyfifo, 32, keyboard.buf_code);
	fifob_init(&mousefifo, 128, mouse.buf_code);

	Timer__construct(&timerCtl);



	timerPtr = Timer_set_timeout(&timerCtl, 1);



	io_8bits_out(PIC0_IMR, 0xf8);//change from 0xf9 & 0xfe = 0xf8 ,means open the PIT 
	io_8bits_out(PIC1_IMR, 0xef);

	Memory__construct(memory, MEM_CHECK_START, MEM_CHECK_END);
	Memory_free(memory, 0x00001000, 0x0009c000);
	Memory_free(memory, 0x00400000, memory->physize - 0x00400000);

	ShtCtl__construct(&shtCtl, binfo->vram, binfo->xsize, binfo->ysize);

	Keyboard__construct(&keyboard);

	uchar *buf_bg = Memory_alloc_4k(memory, (screen.xsize * screen.ysize));
	Screen__construct(&screen, binfo, buf_bg, BCOLOR);
	sheetBg = Sheet_alloc(&shtCtl);
	//------------------------------------put some info---------------------------------------
	sprintf(str, "(%d,%d)", mouse.px, mouse.py);
 	put_string(screen.buf_bg, binfo->xsize, 0, 0, str, BLACK);
 	sprintf(str, "MEMSIZE_PHY = %dMB", memory->physize / 1024 / 1024);
 	put_string(screen.buf_bg, binfo->xsize, 0, 50, str, BLACK);
 	sprintf(str, "MEMSIZE_FREE = %dMB", memory->freesize / 1024 / 1024);
 	put_string(screen.buf_bg, binfo->xsize, 0, 70, str, BLACK);

 	/****************************************/
	sprintf(str, "addr end %x", &(timerCtl.timers));
	fill_box(screen.buf_bg, binfo->xsize, 0, 100, BCOLOR, 100, 16);
	put_string(screen.buf_bg, binfo->xsize, 0, 100, str, BLACK);
	Sheet_refreshsub(&shtCtl, 0, 100, 100, 16);
	/****************************************/

	/****************************************/
	sprintf(str, "addr start next %x", timerPtr);
	fill_box(screen.buf_bg, binfo->xsize, 0, 120, BCOLOR, 100, 16);
	put_string(screen.buf_bg, binfo->xsize, 0, 120, str, BLACK);
	Sheet_refreshsub(&shtCtl, 0, 120, 100, 16);
	/****************************************/
 	//----------------------------------------------------------------------------------------
	Sheet_setbuf(sheetBg, screen.buf_bg, screen.xsize, screen.ysize, 0xff);
	Sheet_slide(&shtCtl, sheetBg, 0, 0);
	Sheet_updown(&shtCtl, sheetBg, 0);

	Mouse__construct(&mouse, binfo);
	sheetMouse = Sheet_alloc(&shtCtl);
	Sheet_setbuf(sheetMouse, mouse.cursor, mouse.xsize, mouse.ysize, 0xff);
	Sheet_slide(&shtCtl, sheetMouse, mouse.px, mouse.py);
	Sheet_updown(&shtCtl, sheetMouse, 1);

	Sheet_refresh(&shtCtl);

	Mouse_enable();


	while(1)
	{
		sprintf(str, "%d", timerCtl.count);
		fill_box(screen.buf_bg, binfo->xsize, 0, 32, BCOLOR, 100, 16);
		put_string(screen.buf_bg, binfo->xsize, 0, 32, str, BLACK);
		Sheet_refreshsub(&shtCtl, 0, 32, 100, 16);




		io_cli();
		if (fifob_status(&keyfifo) + fifob_status(&mousefifo) == 0){
			io_stihlt();
		}else if(fifob_status(&keyfifo) != 0){
			data = fifob_get(&keyfifo);
			io_sti();
			sprintf(str, "%x", data);
			fill_box(screen.buf_bg, binfo->xsize, 0, 16, BCOLOR, 30, 16);
			put_string(screen.buf_bg, binfo->xsize, 0, 16, str, BLACK);
			Sheet_refreshsub(&shtCtl, 0, 16, 30, 16);
		}else if(fifob_status(&mousefifo) != 0){
			data = fifob_get(&mousefifo);
			io_sti();

			if(Mouse_dcode(&mouse, data)){
				sprintf(str, "%x, %x, %x", mouse.button, mouse.rx, mouse.ry);
				fill_box(screen.buf_bg, binfo->xsize, 30, 16, BCOLOR, 120, 16);
				put_string(screen.buf_bg, binfo->xsize, 30, 16, str, BLACK);
				Sheet_refreshsub(&shtCtl, 30, 16, 120, 16);

				sprintf(str, "(%d,%d)", mouse.px, mouse.py);
				fill_box(screen.buf_bg, binfo->xsize, 0, 0, BCOLOR, 120, 16);
				put_string(screen.buf_bg, binfo->xsize, 0, 0, str, BLACK);
				Sheet_refreshsub(&shtCtl, 0, 0, 120, 16);

				Mouse_move(&mouse, &screen, sheetMouse);
			}

		}
		
	}
}















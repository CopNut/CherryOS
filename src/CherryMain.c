#include <stdio.h>
#include <CherryOS.h>

BootinfoPtr binfo;

FIFO32 fifo32;
extern uint data_shift_mouse, data_shift_key, data_shift_timer;

MemoryPtr memory;

Screen screen;
Fontinfo fontinfo;

Keyboard keyboard;

Mouse mouse;

ShtCtlPtr ctl;

TimerCtl timerCtl;


void CherryMain() {
	binfo = (BootinfoPtr)ADDR_BOOTINFO;
	memory = (MemoryPtr)ADDR_MEMBUF;
	SheetPtr sheetBg, sheetMouse, sheetWindow;
	TimerPtr timerPtr;

	char str[100];
	char buf[160];
	uint buf_fifo32[128];
	uint data;
	uchar *addr1, *addr2, *addr3;

	gdt_install();
	idt_install();
	init_pic();
	io_sti();

	FIFO32__construct(&fifo32, 128, buf_fifo32);

	Timer__construct(&timerCtl, &fifo32, 512);
	timerPtr = Timer_set_timeout(&timerCtl, 100);

	io_8bits_out(PIC0_IMR, 0xf8);//change from 0xf9 & 0xfe = 0xf8 ,means open the PIT 
	io_8bits_out(PIC1_IMR, 0xef);

	Memory__construct(memory, MEM_CHECK_START, MEM_CHECK_END);
	Memory_free(memory, 0x00001000, 0x0009c000);
	Memory_free(memory, 0x00400000, memory->physize - 0x00400000);

	ctl = ShtCtl__construct(binfo->vram, binfo->xsize, binfo->ysize, memory);

	Keyboard__construct(&keyboard, &fifo32, 0);

	Font__construct(FONT_HEIGHT, FONT_WIDTH, FONT_MARGIN_VERTICAL, FONT_MARGIN_PARALELL, BLACK);

	uchar *buf_bg = Memory_alloc_4k(memory, binfo->xsize * binfo->ysize);	
	Screen__construct(&screen, binfo, buf_bg, BCOLOR);
	sheetBg = Sheet_alloc();

	/**************************************************/
	/*                  Memory debug                  */
	/**************************************************/
	
	addr1 = Memory_alloc_4k(memory, 1000);
	addr2 = Memory_alloc_4k(memory, 100000);
	addr3 = Memory_alloc_4k(memory, 0x10000);

	sprintf(str, "addr1 = %x", addr1);
	put_string(screen.buf_bg, binfo->xsize, 0, 50, str, WHITE);
	sprintf(str, "addr2 = %x", addr2);
	put_string(screen.buf_bg, binfo->xsize, 0, 70, str, WHITE);
	sprintf(str, "addr3 = %x", addr3);
	put_string(screen.buf_bg, binfo->xsize, 0, 90, str, WHITE);
	sprintf(str, "buf_bg = %x", buf_bg);
	put_string(screen.buf_bg, binfo->xsize, 0, 110, str, WHITE);
	sprintf(str, "shtctl = %x %x %x", ctl, ctl->vram, ctl->map);
	put_string(screen.buf_bg, binfo->xsize, 0, 130, str, WHITE);

	Sheet_setbuf(sheetBg, screen.buf_bg, screen.xsize, screen.ysize, 0xff);
	Sheet_slide(sheetBg, 0, 0);
	Sheet_updown(sheetBg, 0);

	Mouse__construct(&mouse, &fifo32, 256);
	sheetMouse = Sheet_alloc();
	Sheet_setbuf(sheetMouse, mouse.cursor, mouse.xsize, mouse.ysize, 0xff);
	Sheet_slide(sheetMouse, mouse.px, mouse.py);
	Sheet_updown(sheetMouse, 1);

	// Sheet_refresh();

	Mouse_enable();

	while(1)
	{
		sprintf(str, "%d", timerCtl.count);
		Sheet_put_string(sheetBg, str, 0, 32, BCOLOR, BLACK);

		io_cli();
		if (FIFO32_status(&fifo32) == 0){
			io_stihlt();
		}else{
			data = FIFO32_get(&fifo32);
			io_sti();
			if (FIFO_KEY_START <= data && data <= FIFO_KEY_END){
				//data from keydoard
				sprintf(str, "%x", data - data_shift_key);
				Sheet_put_string(sheetBg, str, 0, 16, BCOLOR, BLACK);
			}else if (FIFO_MOUSE_START <= data && data <= FIFO_MOUSE_END){
				//data from mouse
				if(Mouse_dcode(&mouse, data - data_shift_mouse)){
					sprintf(str, "%x, %x, %x", mouse.button, mouse.rx, mouse.ry);
					Sheet_put_string(sheetBg, str, 30, 16, BCOLOR, BLACK);

					sprintf(str, "(%d,%d)", mouse.px, mouse.py);
					Sheet_put_string(sheetBg, str, 0, 0, BCOLOR, BLACK);

					Mouse_move(&mouse, &screen, sheetMouse);
				}
			}else{
				//data from timer
				sprintf(str, "%d timeout", data - data_shift_timer);
				Sheet_put_string(sheetBg, str, 0, 180, BCOLOR, BLACK);

				sprintf(str, "-ff = %d", atoi_hex("-ff"));
				Sheet_put_string(sheetBg, str, 120, 180, BCOLOR, BLACK);
			}
		}
	}
}















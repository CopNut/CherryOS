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

void task_b_main(void);

void CherryMain() {
	binfo = (BootinfoPtr)ADDR_BOOTINFO;
	memory = (MemoryPtr)ADDR_MEMBUF;
	SheetPtr sheetBg, sheetMouse, sheetWindow;
	TimerPtr timerPtr1, timerPtr2, timerPtr_ts;

	Window window;

	char str[100];
	char buf[160];
	uint buf_fifo32[128];
	uint data;

	gdt_install();
	idt_install();
	init_pic();
	io_sti();

	FIFO32__construct(&fifo32, 128, buf_fifo32);

	Timer__construct(512);
	timerPtr1 = Timer_alloc();
	Timer_set_timeout(timerPtr1, 0, 50, &fifo32);
	timerPtr2 = Timer_alloc();
	Timer_set_timeout(timerPtr2, 10, 100, &fifo32);
	timerPtr_ts = Timer_alloc();
	Timer_set_timeout(timerPtr_ts, 2, 2, &fifo32);

	io_8bits_out(PIC0_IMR, 0xf8);//change from 0xf9 & 0xfe = 0xf8 ,means open the PIT 
	io_8bits_out(PIC1_IMR, 0xef);

	Memory__construct(memory, MEM_CHECK_START, MEM_CHECK_END);
	Memory_free(memory, 0x00001000, 0x0009c000);
	Memory_free(memory, 0x00400000, memory->physize - 0x00400000);

	ctl = ShtCtl__construct(binfo->vram, binfo->xsize, binfo->ysize, memory);

	Keyboard__construct(&keyboard, &fifo32, 0);

	Font__construct(FONT_HEIGHT, FONT_WIDTH, FONT_MARGIN_VERTICAL, FONT_MARGIN_PARALELL, BLACK);

	uchar *buf_bg = (uchar *)Memory_alloc_4k(memory, binfo->xsize * binfo->ysize);
	Screen__construct(&screen, binfo, buf_bg, BCOLOR);
	sheetBg = Sheet_alloc();
	* ((int *) 0x8fffc) = (int) sheetBg;
	Sheet_setbuf(sheetBg, screen.buf_bg, screen.xsize, screen.ysize, 0xff);
	Sheet_slide(sheetBg, 0, 0);
	Sheet_updown(sheetBg, 0);

	Mouse__construct(&mouse, &fifo32, 256);
	sheetMouse = Sheet_alloc();
	Sheet_setbuf(sheetMouse, mouse.cursor, mouse.xsize, mouse.ysize, 0xff);
	Sheet_slide(sheetMouse, mouse.px, mouse.py);
	Sheet_updown(sheetMouse, 1);

/*************************************/
/*               TSS                 */
/*************************************/
	int task_b_esp;
	task_b_esp = Memory_alloc_4k(memory, 64 * 1024) + 64 * 1024;
	TSS tssA, tssB;
	tssA.ldtr = 0;
	tssA.iomap = 0x40000000;
	tssB.ldtr = 0;
	tssB.iomap = 0x40000000;
	tssB.eip = (int) &task_b_main - ADR_BOTPAK;
	tssB.eflags = 0x00000202;
	tssB.eax = 0;
	tssB.ecx = 0;
	tssB.edx = 0;
	tssB.ebx = 0;
	tssB.esp = task_b_esp;
	tssB.ebp = 0;
	tssB.esi = 0;
	tssB.edi = 0;
	tssB.es = 1 * 8;
	tssB.cs = 2 * 8;
	tssB.ss = 1 * 8;
	tssB.ds = 1 * 8;
	tssB.fs = 1 * 8;
	tssB.gs = 1 * 8;

    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
    set_segmdesc(gdt + 3, 103, (int) &tssA, AR_TSS32);
    set_segmdesc(gdt + 4, 103, (int) &tssB, AR_TSS32);

    load_tr(3 * 8);

	sprintf(str, "%x", (int)&tssA);
	Sheet_put_string(sheetBg, str, 320, 16, BCOLOR, BLACK);



	Window__construct(&window);
	sprintf(str, "buf_window at %x", window.buf_sheet);
 	Sheet_put_string(sheetBg, str, 0, 110, BCOLOR, BLACK);

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
			if (data == FIFO_TIMER_START + 2)//timerPtr_ts
			{
				farjmp(0, 4 * 8);
				Timer_set_timeout(timerPtr_ts, 2, 2, &fifo32);
			}
			if (FIFO_KEY_START <= data && data <= FIFO_KEY_END){
				//data from keydoard
				sprintf(str, "%x", data - data_shift_key);
				Sheet_put_string(sheetBg, str, 0, 16, BCOLOR, BLACK);
				if (data < 0x54 && data != 0)
				{
					str[0] = key_table[data];
					str[1] = 0;
					Sheet_put_string(window.sht, str, 10, 30, WHITE, PINK);
				}
			}else if (FIFO_MOUSE_START <= data && data <= FIFO_MOUSE_END){
				//data from mouse
				if(Mouse_dcode(&mouse, data - data_shift_mouse)){
					sprintf(str, "%x, %x, %x", mouse.button, mouse.rx, mouse.ry);
					Sheet_put_string(sheetBg, str, 30, 16, BCOLOR, BLACK);

					sprintf(str, "(%d,%d)", mouse.px, mouse.py);
					Sheet_put_string(sheetBg, str, 0, 0, BCOLOR, BLACK);

					if ((mouse.button & 0x01) != 0)
					{
						Sheet_slide(window.sht, window.sht->vx0 + mouse.rx, window.sht->vy0 + mouse.ry);
					}

					Mouse_move(&mouse, &screen, sheetMouse);
				}
			}else{
				//data from timer
				switch(data){
					case (FIFO_TIMER_START + 0) :
						Timer_set_timeout(timerPtr1, 1, 50, &fifo32);
						fill_box(window.buf_sheet, window.sht->bxsize, 20, 30, BLACK, fontinfo.width_box, fontinfo.height_box);
						Sheet_refreshsub(window.sht->vx0 + 20, window.sht->vy0 + 30, fontinfo.width_box, fontinfo.height_box, window.sht->height, window.sht->height);
						break;
					case (FIFO_TIMER_START + 1) :
						Timer_set_timeout(timerPtr1, 0, 50, &fifo32);
						fill_box(window.buf_sheet, window.sht->bxsize, 20, 30, WHITE, fontinfo.width_box, fontinfo.height_box);
						Sheet_refreshsub(window.sht->vx0 + 20, window.sht->vy0 + 30, fontinfo.width_box, fontinfo.height_box, window.sht->height, window.sht->height);
						break;
					case (FIFO_TIMER_START + 10) :
						sprintf(str, "10secs");
						Sheet_put_string(sheetBg, str, 320, 0, BCOLOR, BLACK);
				}
			}
		}
	}
}

void task_b_main(void){

	FIFO32 fifo;
	uint buf_fifo[128], data, count;
	char str[11];
	TimerPtr timerPtr_ts, timerPtr1;
	SheetPtr sheetBg = (SheetPtr) * ((int *) 0x8fffc);

	FIFO32__construct(&fifo, 128, buf_fifo);
	timerPtr_ts = Timer_alloc();
	Timer_set_timeout(timerPtr_ts, 2, 2, &fifo);
	timerPtr1 = Timer_alloc();
	Timer_set_timeout(timerPtr1, 1, 1, &fifo);


	for (;;)
	{
		count++;
		io_cli();
		if (FIFO32_status(&fifo) == 0)
		{
			io_sti();
		}
		else
		{
			data = FIFO32_get(&fifo);
			io_sti();

			if (data == FIFO_TIMER_START + 1)
			{
				sprintf(str, "%d", count);
				Sheet_put_string(sheetBg, str, 320, 32, BCOLOR, BLACK);
				Timer_set_timeout(timerPtr1, 1, 1, &fifo);
			}
			if (data == FIFO_TIMER_START + 2)
			{
				farjmp(0, 3 * 8);
				Timer_set_timeout(timerPtr_ts, 2, 2, &fifo);
			}
		}
	}
}
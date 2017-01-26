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

void task_b_main(SheetPtr sheetBg);

void CherryMain() {
	binfo = (BootinfoPtr)ADDR_BOOTINFO;
	memory = (MemoryPtr)ADDR_MEMBUF;
	SheetPtr sheetBg, sheetMouse, sheetWindow;
	struct TIMER *timerPtr1, *timerPtr2, *timerPtr3;


	char str[100];
	char buf[160];
	uint buf_fifo32[128];
	uint data, count = 0;

	gdt_install();
	idt_install();
	init_pic();
	io_sti();

	FIFO32__construct(&fifo32, 128, buf_fifo32);

	init_pit(512);
	timerPtr1 = timer_alloc();
	timer_init(timerPtr1, &fifo32, 0);
	timer_settime(timerPtr1, 50);
	timerPtr2 = timer_alloc();
	timer_init(timerPtr2, &fifo32, 1);
	timer_settime(timerPtr2, 100);
	timerPtr3 = timer_alloc();
	timer_init(timerPtr3, &fifo32, 2);
	timer_settime(timerPtr3, 100);

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
	Sheet_setbuf(sheetBg, screen.buf_bg, screen.xsize, screen.ysize, 0xff);
	Sheet_slide(sheetBg, 0, 0);
	Sheet_updown(sheetBg, 0);

	Mouse__construct(&mouse, &fifo32, 256);
	sheetMouse = Sheet_alloc();
	Sheet_setbuf(sheetMouse, mouse.cursor, mouse.xsize, mouse.ysize, 0xff);
	Sheet_slide(sheetMouse, mouse.px, mouse.py);
	Sheet_updown(sheetMouse, 1);

	Window__construct();

/*************************************/
/*               TSS                 */
/*************************************/
	TaskPtr task_a, task_b[4];
	SheetPtr window[4];

	task_a = Task_init();
	fifo32.task = task_a;
	Task_run(task_a, 1, 0);

	for (int i = 0; i < 3; ++i)
	{
		window[i] = Window_alloc("window", i * 170, 100);
		task_b[i] = Task_alloc();
		task_b[i]->tss.esp = Memory_alloc_4k(memory, 64 * 1024) + 64 * 1024 - 8;
		task_b[i]->tss.eip = (int) &task_b_main - ADR_BOTPAK;
		task_b[i]->tss.es = 1 * 8;
		task_b[i]->tss.cs = 2 * 8;
		task_b[i]->tss.ss = 1 * 8;
		task_b[i]->tss.ds = 1 * 8;
		task_b[i]->tss.fs = 1 * 8;
		task_b[i]->tss.gs = 1 * 8;
		*((int *)(task_b[i]->tss.esp + 4)) = (int)window[i];
		Task_run(task_b[i], 2, i + 1);		
	}

	SheetPtr window1 = Window_alloc("window1", 200, 200);

	Mouse_enable();

	while(1)
	{
		sprintf(str, "%d", timerCtl.count);
		Sheet_put_string(sheetBg, str, 0, 32, BCOLOR, BLACK);

		io_cli();
		if (FIFO32_status(&fifo32) == 0){
			Task_sleep(task_a);
			io_sti();
		}else{
			data = FIFO32_get(&fifo32);
			io_sti();
			if (FIFO_KEY_START <= data && data <= FIFO_KEY_END){
				//data from keydoard
				sprintf(str, "%x", data - data_shift_key);
				Sheet_put_string(sheetBg, str, 0, 16, BCOLOR, BLACK);
				if (data < 0x54 && data != 0)
				{
					str[0] = key_table[data];
					str[1] = 0;
					Sheet_put_string(window1, str, 10, 30, WHITE, PINK);
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
						Sheet_slide(window1, window1->vx0 + mouse.rx, window1->vy0 + mouse.ry);
					}

					Mouse_move(&mouse, &screen, sheetMouse);
				}
			}else{
				//data from timer
				switch(data){
					case (FIFO_TIMER_START + 0) :
						timer_settime(timerPtr1, 100);
						fill_box(window1->buf, window1->bxsize, 20, 30, BLACK, fontinfo.width_box, fontinfo.height_box);
						Sheet_refreshsub(window1->vx0 + 20, window1->vy0 + 30, fontinfo.width_box, fontinfo.height_box, window1->height, window1->height);
						break;
					case (FIFO_TIMER_START + 1) :
						timer_settime(timerPtr2, 100);
						fill_box(window1->buf, window1->bxsize, 20, 30, WHITE, fontinfo.width_box, fontinfo.height_box);
						Sheet_refreshsub(window1->vx0 + 20, window1->vy0 + 30, fontinfo.width_box, fontinfo.height_box, window1->height, window1->height);
						break;
					case (FIFO_TIMER_START + 2) :
						timer_settime(timerPtr3, 100);
						sprintf(str, "%d secs", ++count);
						Sheet_put_string(sheetBg, str, 320, 0, BCOLOR, BLACK);
				}
			}
		}
	}
}

void task_b_main(SheetPtr sheetBg){

	FIFO32 fifo;
	uint buf_fifo[128], data, count = 0, count0 = 0;
	char str[11];
	struct TIMER *timerPtr1, *timerPtr2;

	FIFO32__construct(&fifo, 128, buf_fifo);

	timerPtr1 = timer_alloc();
	timer_init(timerPtr1, &fifo, 2);
	timer_settime(timerPtr1, 1);
	timerPtr2 = timer_alloc();
	timer_init(timerPtr2, &fifo, 3);
	timer_settime(timerPtr2, 100);

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

			if (data == FIFO_TIMER_START + 2)
			{
				sprintf(str, "%d", count);
				Sheet_put_string(sheetBg, str, fontinfo.margin_parallel, fontinfo.height_box, WHITE, BLACK);
				timer_settime(timerPtr1, 1);
			}
			else if (data == FIFO_TIMER_START + 3)
			{
				sprintf(str, "%d", count - count0);
				Sheet_put_string(sheetBg, str, fontinfo.margin_parallel, 2 * fontinfo.height_box, WHITE, RED);
				count0 = count;
				timer_settime(timerPtr2, 100);
			}
		}
	}
}
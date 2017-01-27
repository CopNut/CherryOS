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
SheetPtr sht_focus, sht_unfocus;

TimerCtl timerCtl;

void task_b_main(SheetPtr sheetBg);
void console(SheetPtr sheetBg);

void CherryMain() {
	binfo = (BootinfoPtr)ADDR_BOOTINFO;
	memory = (MemoryPtr)ADDR_MEMBUF;
	SheetPtr sheetBg, sheetMouse;
	struct TIMER *timerPtr1, *timerPtr2, *timerPtr3;


	char str[100];
	char buf[160];
	uint buf_fifo32[128];
	uint data, count = 0;
	int key_to;

	gdt_install();
	idt_install();
	init_pic();
	io_sti();

	FIFO32__construct(&fifo32, 128, buf_fifo32, 0);

	init_pit(512);
	timerPtr1 = timer_alloc();
	timer_init(timerPtr1, &fifo32, 0);
	timer_settime(timerPtr1, 50);
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

	ushort cursor_x = fontinfo.width_box, cursor_y = fontinfo.height_box;
	char cursor_color = BLACK;

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

	sprintf(str, "%x, %x, %x", mouse.button, mouse.rx, mouse.ry);
	Sheet_put_string(sheetBg, str, 30, 16, BCOLOR, BLACK);
	sprintf(str, "(%d,%d)", mouse.px, mouse.py);
	Sheet_put_string(sheetBg, str, 0, 0, BCOLOR, BLACK);

	Window__construct();

/*************************************/
/*               TSS                 */
/*************************************/
	TaskPtr task_a, task_b[4], task_cons;
	SheetPtr sht_task_b[3], sht_console;

	task_a = Task_init();
	fifo32.task = task_a;
	Task_run(task_a, 1, 0);
	SheetPtr sht_task_a = Window_alloc("task_a", 0, 400, 200, 150);

	sht_console = Window_alloc("Console", 500, 400, 300, 200);
	task_cons = Task_alloc();
	task_cons->tss.esp = Memory_alloc_4k(memory, 64 * 1024) + 64 * 1024 - 8;
	task_cons->tss.eip = (int) &console - ADR_BOTPAK;
	task_cons->tss.es = 1 * 8;
	task_cons->tss.cs = 2 * 8;
	task_cons->tss.ss = 1 * 8;
	task_cons->tss.ds = 1 * 8;
	task_cons->tss.fs = 1 * 8;
	task_cons->tss.gs = 1 * 8;
	*((int *)(task_cons->tss.esp + 4)) = (int)sht_console;
	Task_run(task_cons, 2, 2);		

	for (int i = 0; i < 3; ++i)
	{
		sprintf(str, "task_b_%d", i);
		sht_task_b[i] = Window_alloc(str, i * 210, 100, 200, 150);
		task_b[i] = Task_alloc();
		task_b[i]->tss.esp = Memory_alloc_4k(memory, 64 * 1024) + 64 * 1024 - 8;
		task_b[i]->tss.eip = (int) &task_b_main - ADR_BOTPAK;
		task_b[i]->tss.es = 1 * 8;
		task_b[i]->tss.cs = 2 * 8;
		task_b[i]->tss.ss = 1 * 8;
		task_b[i]->tss.ds = 1 * 8;
		task_b[i]->tss.fs = 1 * 8;
		task_b[i]->tss.gs = 1 * 8;
		*((int *)(task_b[i]->tss.esp + 4)) = (int)sht_task_b[i];
		Task_run(task_b[i], 2, i + 1);		
	}


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
					if (sht_focus->height == sht_task_a->height)
					{
						if (cursor_x < sheetBg->bxsize)
						{
							str[0] = key_table[data];
							str[1] = 0;
							Sheet_put_string(sht_task_a, str, cursor_x, cursor_y, WHITE, PINK);
							cursor_x += fontinfo.width_box;
							fill_box(sht_task_a->buf, sht_task_a->bxsize, cursor_x, cursor_y, BLACK, fontinfo.width_box, fontinfo.height_box);
							Sheet_refreshmap(sht_task_a->vx0 + cursor_x, sht_task_a->vy0 + cursor_y, fontinfo.width_box, fontinfo.height_box, sht_task_a->height, ctl->top);						
							Sheet_refreshsub(sht_task_a->vx0 + cursor_x, sht_task_a->vy0 + cursor_y, fontinfo.width_box, fontinfo.height_box, sht_task_a->height, sht_task_a->height);
						}
					}else{
						FIFO32_put(&task_cons->fifo, key_table[data]);
					}
				}
				if (data == 0xe)
				{
					if (sht_focus->height == sht_task_a->height)
					{
						cursor_x -= fontinfo.width_box;
						if (cursor_x > fontinfo.width_box)
						{
							cursor_x -= fontinfo.width_box;	
							fill_box(sht_task_a->buf, sht_task_a->bxsize, cursor_x, cursor_y, BLACK, fontinfo.width_box, fontinfo.height_box);
							fill_box(sht_task_a->buf, sht_task_a->bxsize, cursor_x + fontinfo.width_box, cursor_y, WHITE, 2 * fontinfo.width_box, fontinfo.height_box);
							Sheet_refreshmap(sht_task_a->vx0 + cursor_x, sht_task_a->vy0 + cursor_y, 3 * fontinfo.width_box, fontinfo.height_box, sht_task_a->height, ctl->top);						
							Sheet_refreshsub(sht_task_a->vx0 + cursor_x, sht_task_a->vy0 + cursor_y, 3 * fontinfo.width_box, fontinfo.height_box, sht_task_a->height, sht_task_a->height);							
						}else{
							fill_box(sht_task_a->buf, sht_task_a->bxsize, cursor_x, cursor_y, BLACK, fontinfo.width_box, fontinfo.height_box);							
							fill_box(sht_task_a->buf, sht_task_a->bxsize, cursor_x + fontinfo.width_box, cursor_y, WHITE, fontinfo.width_box, fontinfo.height_box);
							Sheet_refreshmap(sht_task_a->vx0 + cursor_x, sht_task_a->vy0 + cursor_y, 2 * fontinfo.width_box, fontinfo.height_box, sht_task_a->height, ctl->top);						
							Sheet_refreshsub(sht_task_a->vx0 + cursor_x, sht_task_a->vy0 + cursor_y, 2 * fontinfo.width_box, fontinfo.height_box, sht_task_a->height, sht_task_a->height);							
						}
					}else{
						FIFO32_put(&task_cons->fifo, 0xe);
					}
				}
			}else if (FIFO_MOUSE_START <= data && data <= FIFO_MOUSE_END){
				//data from mouse
				if(Mouse_dcode(&mouse, data - data_shift_mouse)){
					sprintf(str, "%d", mouse.aim);
					Sheet_put_string(sheetBg, str, 50, 50, BCOLOR, BLACK);

					sprintf(str, "%x, %x, %x", mouse.button, mouse.rx, mouse.ry);
					Sheet_put_string(sheetBg, str, 30, 16, BCOLOR, BLACK);

					sprintf(str, "(%d,%d)", mouse.px, mouse.py);
					Sheet_put_string(sheetBg, str, 0, 0, BCOLOR, BLACK);

					if ((mouse.button & 0x01) != 0)
					{
						sht_focus = Sheet_fetch_sht(mouse.aim);
						Window_draw_frame(sht_focus, BLACK, PINK, "window");
						Sheet_updown(sht_focus, ctl->top - 1);

						for (int i = 0; i < MAX_SHEETS; i++) {
							sht_unfocus = &ctl->sheets0[i];
							if (sht_unfocus->flags != 0 && sht_unfocus != sheetBg && sht_unfocus != sheetMouse && sht_unfocus != sht_focus) {
								Window_draw_frame(&ctl->sheets0[i], DARK_GRAY, WHITE, "window");
								Sheet_refreshmap(sht_unfocus->vx0, sht_unfocus->vy0, sht_unfocus->bxsize, fontinfo.height_box, sht_unfocus->height, ctl->top);
								Sheet_refreshsub(sht_unfocus->vx0, sht_unfocus->vy0, sht_unfocus->bxsize, fontinfo.height_box, sht_unfocus->height, sht_unfocus->height);
							}
						}

						sprintf(str, "%d", sht_focus->height);
						Sheet_put_string(sheetBg, str, 70, 50, BCOLOR, WHITE);

						if (sht_focus != sheetBg)
						{
							Sheet_slide(sht_focus, sht_focus->vx0 + mouse.rx, sht_focus->vy0 + mouse.ry);
						}
					}
					Mouse_move(&mouse, &screen, sheetMouse);
				}
			}else{
				//data from timer
				switch(data){
					case (FIFO_TIMER_START + 0) :
					case (FIFO_TIMER_START + 1) :
						if (data == FIFO_TIMER_START + 0)
						{
							timer_init(timerPtr1, &fifo32, 1);
							cursor_color = WHITE;
						}
						else
						{
							timer_init(timerPtr1, &fifo32, 0);
							cursor_color = BLACK;
						}
						timer_settime(timerPtr1, 50);
						fill_box(sht_task_a->buf, sht_task_a->bxsize, cursor_x, cursor_y, cursor_color, fontinfo.width_box, fontinfo.height_box);
						Sheet_refreshmap(sht_task_a->vx0 + cursor_x, sht_task_a->vy0 + cursor_y, fontinfo.width_box, fontinfo.height_box, sht_task_a->height, ctl->top);												
						Sheet_refreshsub(sht_task_a->vx0 + cursor_x, sht_task_a->vy0 + cursor_y, fontinfo.width_box, fontinfo.height_box, sht_task_a->height, sht_task_a->height);
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

	uint buf_fifo[128], data, count = 0, count0 = 0;
	char str[11];
	TaskPtr task = Task_now();
	struct TIMER *timerPtr1, *timerPtr2;

	FIFO32__construct(&task->fifo, 128, buf_fifo, 0);

	timerPtr1 = timer_alloc();
	timer_init(timerPtr1, &task->fifo, 2);
	timer_settime(timerPtr1, 1);
	timerPtr2 = timer_alloc();
	timer_init(timerPtr2, &task->fifo, 3);
	timer_settime(timerPtr2, 100);

	for (;;)
	{
		count++;
		io_cli();
		if (FIFO32_status(&task->fifo) == 0)
		{
			io_sti();
		}
		else
		{
			data = FIFO32_get(&task->fifo);
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

void console(SheetPtr sheetBg){

	TimerPtr timerPtr1;
	TaskPtr task = Task_now();

	int data, buf_fifo[128];
	char str[100];
	ushort cursor_x = fontinfo.width_box, cursor_y = fontinfo.height_box;
	char cursor_color = WHITE;

	FIFO32__construct(&task->fifo, 128, buf_fifo, task);

	timerPtr1 = timer_alloc();
	timer_init(timerPtr1, &task->fifo, 1);
	timer_settime(timerPtr1, 50);

	for(;;){
		io_cli();
		if (FIFO32_status(&task->fifo) == 0)
		{
			Task_sleep(task);
			io_sti();
		}
		else
		{
			data = FIFO32_get(&task->fifo);
			io_sti();

			if (FIFO_KEY_START <= data && data <= FIFO_KEY_END){
				if (data == 0xe)
				{
					if (sht_focus->height == sheetBg->height)
					{
						cursor_x -= fontinfo.width_box;
						if (cursor_x > fontinfo.width_box)
						{
							cursor_x -= fontinfo.width_box;	
							fill_box(sheetBg->buf, sheetBg->bxsize, cursor_x, cursor_y, BLACK, fontinfo.width_box, fontinfo.height_box);
							fill_box(sheetBg->buf, sheetBg->bxsize, cursor_x + fontinfo.width_box, cursor_y, WHITE, 2 * fontinfo.width_box, fontinfo.height_box);
							Sheet_refreshmap(sheetBg->vx0 + cursor_x, sheetBg->vy0 + cursor_y, 3 * fontinfo.width_box, fontinfo.height_box, sheetBg->height, ctl->top);						
							Sheet_refreshsub(sheetBg->vx0 + cursor_x, sheetBg->vy0 + cursor_y, 3 * fontinfo.width_box, fontinfo.height_box, sheetBg->height, sheetBg->height);							
						}else{
							fill_box(sheetBg->buf, sheetBg->bxsize, cursor_x, cursor_y, BLACK, fontinfo.width_box, fontinfo.height_box);							
							fill_box(sheetBg->buf, sheetBg->bxsize, cursor_x + fontinfo.width_box, cursor_y, WHITE, fontinfo.width_box, fontinfo.height_box);
							Sheet_refreshmap(sheetBg->vx0 + cursor_x, sheetBg->vy0 + cursor_y, 2 * fontinfo.width_box, fontinfo.height_box, sheetBg->height, ctl->top);						
							Sheet_refreshsub(sheetBg->vx0 + cursor_x, sheetBg->vy0 + cursor_y, 2 * fontinfo.width_box, fontinfo.height_box, sheetBg->height, sheetBg->height);							
						}
					}
				}else{
					if (sht_focus->height == sheetBg->height)
					{
						if (cursor_x < sheetBg->bxsize)
						{
							str[0] = data;
							str[1] = 0;
							Sheet_put_string(sheetBg, str, cursor_x, cursor_y, WHITE, PINK);
							cursor_x += fontinfo.width_box;
							fill_box(sheetBg->buf, sheetBg->bxsize, cursor_x, cursor_y, BLACK, fontinfo.width_box, fontinfo.height_box);
							Sheet_refreshmap(sheetBg->vx0 + cursor_x, sheetBg->vy0 + cursor_y, fontinfo.width_box, fontinfo.height_box, sheetBg->height, ctl->top);						
							Sheet_refreshsub(sheetBg->vx0 + cursor_x, sheetBg->vy0 + cursor_y, fontinfo.width_box, fontinfo.height_box, sheetBg->height, sheetBg->height);
						}
					}
				}


			}else{
				switch(data){
					case (FIFO_TIMER_START + 0) :
					case (FIFO_TIMER_START + 1) :
						if (data == FIFO_TIMER_START + 0)
						{
							timer_init(timerPtr1, &task->fifo, 1);
							cursor_color = WHITE;
						}
						else
						{
							timer_init(timerPtr1, &task->fifo, 0);
							cursor_color = BLACK;
						}
						timer_settime(timerPtr1, 50);
						fill_box(sheetBg->buf, sheetBg->bxsize, cursor_x, cursor_y, cursor_color, fontinfo.width_box, fontinfo.height_box);
						Sheet_refreshmap(sheetBg->vx0 + cursor_x, sheetBg->vy0 + cursor_y, fontinfo.width_box, fontinfo.height_box, sheetBg->height, ctl->top);												
						Sheet_refreshsub(sheetBg->vx0 + cursor_x, sheetBg->vy0 + cursor_y, fontinfo.width_box, fontinfo.height_box, sheetBg->height, sheetBg->height);
						break;
				}

			}

		}
	}

	return;
}
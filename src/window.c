#include <stdio.h>
#include <CherryOS.h>

extern BootinfoPtr binfo;
extern MemoryPtr memory;
extern Fontinfo fontinfo;
extern ShtCtlPtr ctl;
extern Screen screen;

struct WindowIcon windowIcon;

void Window__construct(){

	int addr;
	const uchar min[16][16] = {
		"________________",
		"________________",
		"________________",
		"________________",
		"________________",
		"________________",
		"________________",
		"__XXXXXXXXXXXX__",
		"________________",
		"________________",
		"________________",
		"________________",
		"________________",
		"________________",
		"________________",
		"________________",
	};
	const uchar max[16][16] = {
		"________________",
		"________________",
		"__XXXXXXXXXXXX__",
		"__X__________X__",
		"__XXXXXXXXXXXX__",
		"__X__________X__",
		"__X__________X__",
		"__X__________X__",
		"__X__________X__",
		"__X__________X__",
		"__X__________X__",
		"__X__________X__",
		"__X__________X__",
		"__XXXXXXXXXXXX__",
		"________________",
		"________________",
	};
	const uchar close[16][16] = {
		"________________",
		"________________",
		"__X__________X__",
		"___X________X___",
		"____X______X____",
		"_____X____X_____",
		"______X__X______",
		"_______XX_______",
		"_______XX_______",
		"______X__X______",
		"_____X____X_____",
		"____X______X____",
		"___X________X___",
		"__X__________X__",
		"________________",
		"________________",
	};
	Window_decode(windowIcon.min, 16,16, min);
	Window_decode(windowIcon.max, 16, 16, max);
	Window_decode(windowIcon.close, 16, 16, close);
	return;
}

SheetPtr Window_alloc(struct TASK *task, ushort x, ushort y, ushort xsize, ushort ysize)
{
	SheetPtr sht = Sheet_alloc();
	uchar *buf_sheet = (uchar *)Memory_alloc_4k(memory, binfo->xsize * binfo->ysize);
	Sheet_setbuf(sht, buf_sheet, xsize, ysize, 0xff);
	sht->task = task;
	Window_draw_frame(sht, BLACK, PINK, task->name);
	fill_box(sht->buf, sht->bxsize, 0, fontinfo.height_box, WHITE, xsize, ysize - fontinfo.height_box);//window background color
	Sheet_slide(sht, x, y);
	Sheet_updown(sht, ctl->top);

	return sht;
}

void Window_decode(uchar *buf, int xsize, int ysize, uchar *src){

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			switch(src[i*xsize+j]){
				case '_':
					buf[i*xsize+j] = 0xff;
					break;
				case 'X':
					buf[i*xsize+j] = 0x01;
					break;
				default:
					break;
			}
		}
	}
	return;
}

void Window_draw_frame(SheetPtr sht, char backcolor, char fontcolor, char *title){

	uchar *buf = sht->buf;
	ushort xsize = sht->bxsize, ysize = sht->bysize;

	fill_box(buf, xsize, 0, 0, backcolor, xsize, fontinfo.height_box);
	Sheet_put_string(sht, title, 2, 0, backcolor, fontcolor);
	copy_box(buf, xsize, xsize - 16 * 3, fontinfo.margin_vertical, windowIcon.min, 16, 16, fontcolor);
	copy_box(buf, xsize, xsize - 16 * 2, fontinfo.margin_vertical, windowIcon.max, 16, 16, fontcolor);
	copy_box(buf, xsize, xsize - 16 * 1, fontinfo.margin_vertical, windowIcon.close, 16, 16, fontcolor);
	// fill_box(buf, xsize, 0, fontinfo.height_box, backcolor, 1, ysize - fontinfo.height_box);
	// fill_box(buf, xsize, xsize - 1, fontinfo.height_box, backcolor, 1, ysize - fontinfo.height_box);
	// fill_box(buf, xsize, 0, ysize - 1, backcolor, xsize, 1);

	return;
}

void Window_draw_textbox(SheetPtr sht, uint x, uint y, uint xsize, uint ysize)
{


	return;
}

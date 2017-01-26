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
	Window_decode(windowIcon.min, 16,16, min, PINK);
	Window_decode(windowIcon.max, 16, 16, max, PINK);
	Window_decode(windowIcon.close, 16, 16, close, PINK);
	return;
}

SheetPtr Window_alloc(char *title, ushort x, ushort y)
{
	SheetPtr sht = Sheet_alloc();
	uchar *buf_sheet = (uchar *)Memory_alloc_4k(memory, binfo->xsize * binfo->ysize);
	Sheet_setbuf(sht, buf_sheet, 160, 90, 0xff);
	Window_draw_frame(&windowIcon, sht->buf, sht->bxsize, sht->bysize);
	Sheet_slide(sht, x, y);
	Sheet_updown(sht, ctl->top);
	Sheet_put_string(sht, title, 2, 0, BLACK, PINK);

	return sht;
}

void Window_decode(uchar *buf, int xsize, int ysize, const uchar *src, char c){

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			switch(src[i*xsize+j]){
				case '_':
					buf[i*xsize+j] = BLACK;
					break;
				case 'X':
					buf[i*xsize+j] = c;
					break;
				default:
					break;
			}
		}
	}
	return;
}

void Window_draw_frame(struct WindowIcon *this, uchar *buf, ushort xsize, ushort ysize){

	fill_box(buf, xsize, 0, 0, BLACK, xsize, fontinfo.height_box);
	copy_box(buf, xsize, xsize - 16 * 3, fontinfo.margin_vertical, this->min, 16, 16);
	copy_box(buf, xsize, xsize - 16 * 2, fontinfo.margin_vertical, this->max, 16, 16);
	copy_box(buf, xsize, xsize - 16 * 1, fontinfo.margin_vertical, this->close, 16, 16);
	fill_box(buf, xsize, 0, fontinfo.height_box, BLACK, 1, ysize - fontinfo.height_box);
	fill_box(buf, xsize, xsize - 1, fontinfo.height_box, BLACK, 1, ysize - fontinfo.height_box);
	fill_box(buf, xsize, 0, ysize - 1, BLACK, xsize, 1);
	fill_box(buf, xsize, 1, fontinfo.height_box, WHITE, xsize - 2, ysize - fontinfo.height_box - 1);

	return;
}
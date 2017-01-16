#include <stdio.h>
#include <window.h>

extern BootinfoPtr binfo;
extern MemoryPtr memory;
extern Fontinfo fontinfo;
extern ShtCtlPtr ctl;
extern Screen screen;


void Window__construct(WindowPtr this){

	int addr;
	const uchar min[16][16] = {
		"                ",
		"                ",
		"                ",
		"                ",
		"                ",
		"                ",
		"                ",
		"  XXXXXXXXXXXX  ",
		"                ",
		"                ",
		"                ",
		"                ",
		"                ",
		"                ",
		"                ",
		"                "
	};
	const uchar max[16][16] = {
		"                ",
		"                ",
		"  XXXXXXXXXXXX  ",
		"  X          X  ",
		"  XXXXXXXXXXXX  ",
		"  X          X  ",
		"  X          X  ",
		"  X          X  ",
		"  X          X  ",
		"  X          X  ",
		"  X          X  ",
		"  X          X  ",
		"  X          X  ",
		"  XXXXXXXXXXXX  ",
		"                ",
		"                "	
	};
	const uchar close[16][16] = {
		"                ",
		"                ",
		"  X          X  ",
		"   X        X   ",
		"    X      X    ",
		"     X    X     ",
		"      X  X      ",
		"       XX       ",
		"       XX       ",
		"      X  X      ",
		"     X    X     ",
		"    X      X    ",
		"   X        X   ",
		"  X          X  ",
		"                ",
		"                "	
	};
	sprintf(this->title, "CherryWindow");
	Window_decode(this->min, 16, 16, min, PINK);
	Window_decode(this->max, 16, 16, max, PINK);
	Window_decode(this->close, 16, 16, close, PINK);
	this->buf_sheet = (uchar *)Memory_alloc_4k(memory, binfo->xsize * binfo->ysize);
	this->sht = Sheet_alloc();
	Sheet_setbuf(this->sht, this->buf_sheet, 100, 50, 0xff); //need more
	Window_draw_frame(this, this->buf_sheet, this->sht->bxsize, this->sht->bysize);
	Sheet_slide(this->sht, 50, 50); //need more
	Sheet_updown(this->sht, ctl->top);
	
	return;
}

void Window_decode(uchar *buf, int xsize, int ysize, const uchar *src, char c){

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			switch(src[i*xsize+j]){
				case ' ':
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

void Window_draw_frame(WindowPtr this, uchar *buf, ushort xsize, ushort ysize){

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
#include <stdio.h>
#include <bootinfo.h>
#include <memory.h>
#include <screen.h>

void Screen__construct(Screen *this, Bootinfo *binfo, uchar *buf_bg, unsigned char bcolor)
{
	this->vram = binfo->vram;
	this->buf_bg = buf_bg;
	this->xsize = binfo->xsize;
	this->ysize = binfo->ysize;
	this->memsize = this->xsize * this->ysize;
	this->bcolor = bcolor;
	Screen_init_palette(this);
	Screen_draw_bg(this);
	return;
}

void Screen_init_palette(Screen *this)
{
	int eflags;
	char i, j, byte;

	unsigned char rgbTable[16][3] = {
		0x00, 0x00, 0x00,
		0xff, 0x00, 0x00,
		0xff, 0xff, 0x00,
		0x00, 0xff, 0x00,
		0x00, 0xff, 0xff,
		0x00, 0x00, 0xff,
		0xff, 0x00, 0xff,
		0xff, 0xff, 0xff,
		0xd6, 0xd6, 0xd6,
		0x94, 0x00, 0x00,
		0x94, 0x94, 0x00,
		0x00, 0x94, 0x00,
		0x00, 0x94, 0x94,
		0x00, 0x00, 0x94,
		0x94, 0x00, 0x94,
		0x94, 0x94, 0x94
	};
	this->rgbTable = (unsigned char *)rgbTable;

	eflags = io_load_eflags();
	io_cli();
	io_8bits_out(0x03c8, PALETTE_START);
	for (i = PALETTE_START; i <= PALETTE_END; ++i)
	{
		for (j = 0; j <= 2; ++j)
		{
			byte = rgbTable[i][j];
			io_8bits_out(0x03c9, byte);
		}
	}
	io_store_eflags(eflags);

	return;
}


void Screen_draw_bg(Screen *this)
{

	for (int i = 0; i < this->memsize; ++i)
	{
		// vram[i] = i / 20 & 0x0f;
		this->buf_bg[i] = this->bcolor;
	}
	
	return;
}
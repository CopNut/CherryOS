#include <stdio.h>
#include <bootinfo.h>
#include <memory.h>
#include <screen.h>

void Screen__construct(Screen *this, Bootinfo *binfo, unsigned char bcolor)
{
	this->vram = binfo->vram;
	this->xsize = binfo->xsize;
	this->ysize = binfo->ysize;
	this->memsize = VGA_TOTAL_BYTES;
	this->bcolor = bcolor;
	Screen_init_palette(this);
	for (uchar i = 0; i < MAX_SHEET; ++i)
	{
		this->sheet[i].flag = UNUSR_SHEET; //unuse
		this->sheetPtr[i] = &this->sheet[i];
	}
	this->top = -1;
	//Screen_draw_bg(this);
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

SheetPtr Screen_alloc(Screen *this, uchar *buf, ushort xsize, ushort ysize, uchar color, uchar opacity){

	SheetPtr sheetPtr;
	for (uchar i = 0; i < MAX_SHEET; ++i)
	{
		if (this->sheet[i].flag == 0)
		{
			sheetPtr = &this->sheet[i];
			this->sheet[i].flag = USE_SHEET;
			this->sheet[i].height = -1;
			this->sheet[i].buf = buf;
			this->sheet[i].xsize = xsize;
			this->sheet[i].ysize = ysize;  
			this->sheet[i].color = color;
			this->sheet[i].opacity = opacity;

			return sheetPtr;// success
		}
	}

	return 0;//no more sheet
}

void Screen_height_shift(Screen *this, SheetPtr sheetPtr, short height_aim){

	short height_before_shift = sheetPtr->height;

	//revise the height
	sheetPtr->height = (height_aim > this->top + 1) ? this->top + 1 : height_aim;
	sheetPtr->height = (height_aim < -1) ? -1 : height_aim;

	if (height_before_shift > height_aim)
	{
		if (height_aim >= 0)
		{
			for (short h = height_before_shift; h > height_aim; --h)
			{
				this->sheetPtr[h] = this->sheetPtr[h -1];
				this->sheetPtr[h]->height = h;
			}
			this->sheetPtr[height_aim] = sheetPtr;
		}else{//height_aim = -1
			if (this->top > height_before_shift)
			{
				for (short h = height_before_shift; h < this->top; ++h)
				{
					this->sheetPtr[h] = this->sheetPtr[h + 1];
					this->sheetPtr[h]->height = h;
				}
			}
			this->top--;
		}
		Screen_refresh(this);

	}else if (height_before_shift < height_aim)
	{
		if (height_before_shift >= 0)
		{
			for (short h = height_before_shift; h < height_aim; ++h)
			{
				this->sheetPtr[h] = this->sheetPtr[h + 1];
				this->sheetPtr[h]->height = h;
			}
			this->sheetPtr[height_aim] = sheetPtr;
		}else{//height_before_shift = -1
			for (short h = this->top; h > height_aim; --h)
			{
				this->sheetPtr[h + 1] = this->sheetPtr[h];
				this->sheetPtr[h + 1]->height = h + 1;
			}
			this->sheetPtr[height_aim] = sheetPtr;
			this->top++;
		}
		Screen_refresh(this);
	}

	return;
}


void Screen_draw_bg(Screen *this)
{
	this->sheet[0].flag = 1;
	for (int i = 0; i < this->memsize; ++i)
	{
		// vram[i] = i / 20 & 0x0f;
		this->vram[i] = this->bcolor;
	}
	
	return;
}

void Screen_refresh(Screen *this){

	SheetPtr sheetPtr;
	short px, py;
	uchar *buf, color;

	for (short h = 0; h < this->top; ++h)
	{
		sheetPtr = this->sheetPtr[h];
		buf = sheetPtr->buf;

		for (short i = 0; i < sheetPtr->ysize; i++)
		{
			py = sheetPtr->py + i;
			for (short j = 0; j < sheetPtr->xsize; j++)
			{
				px = sheetPtr->px + j;
				color = buf[i * sheetPtr->xsize + j];
				this->vram[py * this->xsize + px] = color;
			}
		}
		
	}

	return;
}

void Screen_slide(Screen *this, SheetPtr sheetPtr, short sx, short sy) {

	sheetPtr->px = sx;
	sheetPtr->py = sy;
	if (sheetPtr->height >= 0) {
		Screen_refresh(this);
	}

	return;
}

void Screen_free(Screen *this, SheetPtr sheetPtr) {

	if (sheetPtr->height >= 0) {
		Screen_height_shift(this, sheetPtr, -1);
	}
	sheetPtr->flag = 0;

	return;
}
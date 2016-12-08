#include <stdio.h>
#include <bootinfo.h>
#include <screen.h>
#include <mouse.h>


void Mouse__construct(Mouse *this, Bootinfo *binfo) {
	this->phase = 0;
	this->xsize = MOUSE_XSIZE;
	this->ysize = MOUSE_YSIZE;
	this->px = (binfo->xsize - this->xsize) / 2;
	this->py = (binfo->ysize - this->ysize) / 2;
	this->rx = 0;
	this->ry = 0;
	this->button = 0;

	char cursor[16][10] = {
		{ "**........" },
		{ "*O*......." },
		{ "*O*......." },
		{ "*OO*......" },
		{ "*OO*......" },
		{ "*OOO*....." },
		{ "*OOO*....." },
		{ "*OOOO*...." },
		{ "*OOOO*...." },
		{ "*OOOOO*..." },
		{ "*OOOOO*..." },
		{ "*OOOOOO*.." },
		{ "*OOO*OO*.." },
		{ "*OO*.**O*." },
		{ "*O*....**." },
		{ "**........" }
	};

	for (int x = 0; x < 16; ++x) {

		for (int y = 0; y < 10; ++y) {

			switch (cursor[x][y]) {
			case '.':
				this->cursor[10 * x + y] = 0xff;//¿ÕÎ»±ê¼Ç
				break;

			case '*':
				this->cursor[10 * x + y] = BLACK;
				break;

			case 'O':
				this->cursor[10 * x + y] = WHITE;
				break;
			}
		}
	}

	return;
}

void Mouse_enable(void)
{
	Keyboard_wait_KBC_sendready();
	io_8bits_out(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	Keyboard_wait_KBC_sendready();
	io_8bits_out(PORT_KEYDAT, MOUSECMD_ENABLE);
	return;
}

int Mouse_dcode(Mouse *this, unsigned char data)
{
	char str[10];

	switch (this->phase) {
	case 1:
		if ((data & 0xc8) == 0x08)
		{
			this->buf_dcode[0] = data;
			this->phase = 2;
		}
		return 0;
	case 2:
		this->buf_dcode[1] = data;
		this->phase = 3;
		return 0;
	case 3:
		this->buf_dcode[2] = data;
		this->phase = 1;

		this->button = this->buf_dcode[0] & 0x07;
		this->rx = this->buf_dcode[1];
		this->ry = this->buf_dcode[2];
		this->ry = -this->ry;
		return 1;
	case 0:
		if (data == 0xfa)
		{
			this->phase = 1;
			return 0;
		}
	}
	return -1;
}

void Mouse_move(Mouse *this, Screen *scn)
{
 	put_box(scn->vram, scn->xsize, this->px, this->py, this->bg, this->xsize, this->ysize);
	this->px = ((this->px + this->rx < scn->xsize) && (this->px + this->rx >= 0)) ? this->px + this->rx : this->px;
	if (this->px + this->rx >= scn->xsize)
	{
		this->px = scn->xsize - 1;
	}else if (this->px + this->rx < 0)
	{
		this->px = 0;
	}

	this->py = ((this->py + this->ry < scn->ysize) && (this->py + this->ry >= 0)) ? this->py + this->ry : this->py;
	if (this->py + this->ry >= scn->ysize)
	{
		this->py = scn->ysize - 1;
	}else if (this->py + this->ry < 0)
	{
		this->py = 0;
	}
	get_box(scn->vram, scn->xsize, this->px, this->py, this->bg, this->xsize, this->ysize);
 	put_box(scn->vram, scn->xsize, this->px, this->py, this->cursor, this->xsize, this->ysize);


	return;
}
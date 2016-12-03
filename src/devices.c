#include <stdio.h>
#include "CherryOS.h"


//----------object screen----------
//typedef struct Screen {
//	char *vram;
//	short xsize, ysize;
//	int memsize;
//	unsigned char bcolor;
//	unsigned char *rgbTable;
//}Screen, *ScreenPtr;


void Screen__construct(Screen *this, Bootinfo *binfo, unsigned char bcolor)
{
	this->vram = binfo->vram;
	this->xsize = binfo->xsize;
	this->ysize = binfo->ysize;
	this->memsize = VGA_TOTAL_BYTES;
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
		this->vram[i] = this->bcolor;
	}
	return;
}



//----------object mouse----------
//typedef struct Mouse {
//	unsigned char buf_code[128], buf_dcode[3], phase;
//	char xsize, ysize;
//	short px, py;
//	char cursor[160];
//}Mouse, *MousePtr;


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



//----------object keyboard----------
//typedef struct Keyboard {
//	unsigned char buf_code[32]
//}Keyboard, *KeyboardPtr;

void Keyboard__construct(Keyboard *this)
{
	Keyboard_wait_KBC_sendready();
	io_8bits_out(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	Keyboard_wait_KBC_sendready();
	io_8bits_out(PORT_KEYDAT, KBC_MODE);
	return;
}

void Keyboard_wait_KBC_sendready(void)
{
	for (;;) {
		if ((io_8bits_in(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}



// //----------object memory----------
// typedef struct MemBlock{
// 	uint addr, size;
// }MemBlock, *MemBlockPtr;

// //32K
// #define MEM_BLOCK_MAX 		4092 //4096 * (4 + 4) - (4 * 8) / 8 = 4092
// typedef struct Memory{
// 	uint physize;
// 	uint freesize;
// 	uint frees;
// 	uint maxfrees;
// 	uint usesize;
// 	uint uses;
// 	uint lostsize;
// 	uint losts;	
// 	MemBlock freeMemBlock[MEM_BLOCK_MAX];
// }Memory, *MemoryPtr;

void Memory__construct(Memory *this, uint start, uint end){

	char flag486 = 0;
	uint eflag, cr0;

	eflag = io_load_eflags();
	eflag |= EFLAGE_AC_BIT; //AC-bit = 1;
	io_store_eflags(eflag);
	eflag = io_load_eflags();
	if (eflag == 1)
	{
		flag486 = 1; //if AC-bit doesn's roll back to 0, the machine is 486
	}
	eflag &= ~EFLAGE_AC_BIT; //make AC-bit back to 0
	io_store_eflags(eflag);

	if (flag486 == 1) //disable the cache for Memory_check()
	{
		cr0 = io_load_cr0();
		cr0 |= CR0_CACHE_DISABLE; //cr0 0x60000000 set to 1, cache will be disabled
		io_store_cr0(cr0);
	}

	this->physize = Memory_check(start, end);

	if (flag486 == 1)
	{
		cr0 = io_load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; //enable the cache
		io_store_cr0(cr0);
	}

	this->freesize = 0;
	this->frees = 0;
	this->maxfrees = 0;
	this->usesize = 0;
	this->uses = 0;
	this->lostsize = 0;
	this->losts = 0;

	return;
}

uint Memory_useable(Memory *this){

	uint total = 0;
	for (uint i = 0; i < this->frees; ++i)
	{
		total += this->freeMemBlock[i].size;
	}

	return total;
}

uint Memory_alloc(Memory *this, uint size){

	uint addr = 0; //memory pointer for the allocation result
	for (uint i = 0; i < this->frees; ++i)
	{
		if (this->freeMemBlock[i].size >= size)
		{
			//memory avilable
			addr = this->freeMemBlock[i].addr;
			this->freeMemBlock[i].addr += size;
			this->freeMemBlock[i].size -= size;

			if (this->freeMemBlock[i].size == 0)
			{
				//one block has been allocated, delete from memory->frees
				this->frees--;
				for (; i < this->frees; ++i)
				{
					//fill the blank field of freeMemBlock array
					this->freeMemBlock[i] = this->freeMemBlock[i + 1];
				}
			}
			this->freesize -= size;
			return addr;
		}
	}
	//no avilable memory
	return 0;
}

int Memory_free(Memory *this, uint addr, uint size){

	uint i;
	for (i = 0; i < this->frees; ++i)
	{
		if (this->freeMemBlock[i].addr > addr)
		{
			break;
		}
	}
	//freeMemBlock[i -1].addr < addr < freeMemBlock[i].addr
	
	if (i > 0)
	//there are blocks before this freeing block
	{
		if (this->freeMemBlock[i - 1].addr + this->freeMemBlock[i - 1].size == addr)
		//freeing block can make up with the one in the front of
		{
			this->freeMemBlock[i - 1].size += size;
			if (addr + size == this->freeMemBlock[i].addr)
			//freeing block can also make up with the one right behind
			{
				this->freeMemBlock[i -1].size += this->freeMemBlock[i].size;
				this->frees--;
				for (; i < this->frees; ++i)
				{
					this->freeMemBlock[i] = this->freeMemBlock[i + 1];
				}
			}
		//free complished
		this->freesize += size;
		return 0;
		}
	}

	if (i < this->frees)
	//there are blocks after this freeing block 
	{
		if (addr + size == this->freeMemBlock[i].addr)
		//freeing block can make up with the one right behind
		{
			this->freeMemBlock[i].addr += addr;
			this->freeMemBlock[i].size += size;
			//free complished
			this->freesize += size;
			return 0;
		}
	}

	if (this->frees < MEM_BLOCK_MAX)
	//isolated block
	{
		//right shift blocks from i to frees
		for (uint j = this->frees; j > i; --j)
		{
			this->freeMemBlock[j] = this->freeMemBlock[j - 1];
		}
		//frees increas
		this->frees++;
		if (this->frees > this->maxfrees)
		{
			this->maxfrees = this->frees;
		}

		this->freeMemBlock[i].addr = addr;
		this->freeMemBlock[i].size = size;
		this->freesize += size;
		return 0;
	}

	//block buffer overflow
	this->losts++;
	this->lostsize += size;
	return -1;
}
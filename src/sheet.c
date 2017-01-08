#include <stdio.h>
#include <sheet.h>


#define SHEET_USE		1
extern ShtCtlPtr ctl;

ShtCtlPtr ShtCtl__construct(unsigned char *vram, int xsize, int ysize, MemoryPtr memory)
{
	ShtCtlPtr shtctl;
	int i;

	shtctl = (ShtCtlPtr)Memory_alloc_4k(memory, sizeof(ShtCtl));
	//debug
	shtctl->vram = vram;
	shtctl->map = (uchar *)Memory_alloc_4k(memory, xsize * ysize);
	//debug
	shtctl->xsize = xsize;
	shtctl->ysize = ysize;
	shtctl->top = -1;
	for (i = 0; i < MAX_SHEETS; i++) {
		shtctl->sheets0[i].flags = 0;
	}
	return shtctl;
}

struct SHEET *Sheet_alloc()
{
	struct SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == 0) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE;
			sht->height = -1;
			return sht;
		}
	}
	return 0;
}

void Sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

void Sheet_updown(struct SHEET *sht, int height)
{
	int h, old = sht->height;

	
	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height; 

	
	if (old > height) {	
		if (height >= 0) {
			
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	
			if (ctl->top > old) {
				
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; 
		}
		Sheet_refresh(ctl); 
	} else if (old < height) {	
		if (old >= 0) {
			
			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	
			
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; 
		}
		Sheet_refresh(ctl); 
	}
	return;
}

void Sheet_refresh()
{
	int h, bx, by, vx, vy;
	unsigned char *buf, c, *vram = ctl->vram;
	struct SHEET *sht;
	for (h = 0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		buf = sht->buf;
		for (by = 0; by < sht->bysize; by++) {
			vy = sht->vy0 + by;
			for (bx = 0; bx < sht->bxsize; bx++) {
				vx = sht->vx0 + bx;
				c = buf[by * sht->bxsize + bx];
				if (c != sht->col_inv) {
					vram[vy * ctl->xsize + vx] = c;
				}
			}
		}
	}
	return;
}

void Sheet_refreshmap(int rfx0, int rfy0, int rfxsize, int rfysize)
{
	int h;
	//sheetheight
	int shtx0, shty0, shtx1, shty1;
	//sheet coordinate
	int rfx1, rfy1;
	//aim refresh area coordinate
	int vx0, vy0, vx1, vy1, tempvx0;
	//final refreshsub area coordinate
	unsigned char *buf, c, sid, *map = ctl->map;
	//sheetbuf & pixel_color & sheet_height & pointer to the map
	struct SHEET *sht;
	//pointer to the present sheetheight sheet

	rfx1 = rfx0 + rfxsize < ctl->xsize ? rfx0 + rfxsize : ctl->xsize - 1;
	rfy1 = rfy0 + rfysize < ctl->ysize ? rfy0 + rfysize : ctl->ysize - 1;
	//edge overflow fix

	for (h = 0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		sid = 
		shtx0 = sht->vx0;
		shty0 = sht->vy0;
		shtx1 = shtx0 + sht->bxsize - 1;
		shty1 = shty0 + sht->bysize - 1;
		if (rfy1 < shty0 || rfx0 > shtx1 || rfy0 > shty1 || rfx1 < shtx0)
		{
			continue;
			//if the refresh area have no intersection with the sheet area, then continue to next sheetheight
		}
		tempvx0 = vx0 = rfx0 > shtx0 ? rfx0 : shtx0;
		vy0 = rfy0 > shty0 ? rfy0 : shty0;
		vx1 = rfx1 < shtx1 ? rfx1 : shtx1;
		vy1 = rfy1 < shty1 ? rfy1 : shty1;
		//calculate the refreshsub area

		buf = sht->buf;
		//get sheetbuf

		for (int i = vy0 - shty0; vy0 <= vy1; vy0++, i++) {
			vx0 = tempvx0;
			for (int j = vx0 - shtx0; vx0 <= vx1; vx0++, j++) {
				
				c = buf[i * sht->bxsize + j];
				//i & j are the pointers for the sheetbuf
				if (c != sht->col_inv) {
					map[vy0 * ctl->xsize + vx0] = sid;
					//vy0 & vx0 are the pointers for the vram
				}
			}//vx loop
		}//vy loop
	}//height loop

	return;
}

void Sheet_refreshsub(int rfx0, int rfy0, int rfxsize, int rfysize)
{
	int h;
	//sheetheight
	int shtx0, shty0, shtx1, shty1;
	//sheet coordinate
	int rfx1, rfy1;
	//aim refresh area coordinate
	int vx0, vy0, vx1, vy1, tempvx0;
	//final refreshsub area coordinate
	unsigned char *buf, c, *vram = ctl->vram;
	//sheetbuf & pixel_color_buf & pointer to the vram
	struct SHEET *sht;
	//pointer to the present sheetheight sheet

	rfx1 = rfx0 + rfxsize < ctl->xsize ? rfx0 + rfxsize : ctl->xsize - 1;
	rfy1 = rfy0 + rfysize < ctl->ysize ? rfy0 + rfysize : ctl->ysize - 1;
	//edge overflow fix

	for (h = 0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		shtx0 = sht->vx0;
		shty0 = sht->vy0;
		shtx1 = shtx0 + sht->bxsize - 1;
		shty1 = shty0 + sht->bysize - 1;
		if (rfy1 < shty0 || rfx0 > shtx1 || rfy0 > shty1 || rfx1 < shtx0)
		{
			continue;
			//if the refresh area have no intersection with the sheet area, then continue to next sheetheight
		}
		tempvx0 = vx0 = rfx0 > shtx0 ? rfx0 : shtx0;
		vy0 = rfy0 > shty0 ? rfy0 : shty0;
		vx1 = rfx1 < shtx1 ? rfx1 : shtx1;
		vy1 = rfy1 < shty1 ? rfy1 : shty1;
		//calculate the refreshsub area

		buf = sht->buf;
		//get sheetbuf

		for (int i = vy0 - shty0; vy0 <= vy1; vy0++, i++) {
			vx0 = tempvx0;
			for (int j = vx0 - shtx0; vx0 <= vx1; vx0++, j++) {
				
				c = buf[i * sht->bxsize + j];
				//i & j are the pointers for the sheetbuf
				if (c != sht->col_inv) {
					vram[vy0 * ctl->xsize + vx0] = c;
					//vy0 & vx0 are the pointers for the vram
				}
			}//vx loop
		}//vy loop
	}//height loop

	return;
}

void Sheet_slide(struct SHEET *sht, int vx0, int vy0)
{
	int oldvx0 = sht->vx0;
	int oldvy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) { 
		Sheet_refreshsub(vx0, vy0, sht->bxsize, sht->bysize); 
		Sheet_refreshsub(oldvx0, oldvy0, sht->bxsize, sht->bysize);
	}
	return;
}

void Sheet_free(struct SHEET *sht)
{
	if (sht->height >= 0) {
		Sheet_updown(sht, -1); 
	}
	sht->flags = 0; 
	return;
}

void Sheet_put_string(Sheet *sht, char *str, int x, int y, char b, char c)
{
	int boxXsize = strlen(str) * (FONT_X_SIZE + 2 * FONT_X_MARGIN) - 1;
	int boxYsize = FONT_Y_SIZE + 2 * FONT_Y_MARGIN - 1;
	fill_box(sht->buf, sht->bxsize, x, y, b, boxXsize, boxYsize);
	put_string(sht->buf, sht->bxsize, x, y, str, c);
	Sheet_refreshsub(x, y, boxXsize, boxYsize);
	return;
}

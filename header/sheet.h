#ifndef MEMORY_H
#define MEMORY_H 1
#include <memory.h>
#endif



/* sheet.c */
#define MAX_SHEETS		256

typedef struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
}Sheet, *SheetPtr;
typedef struct SHTCTL {
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
}ShtCtl, *ShtCtlPtr;
ShtCtlPtr ShtCtl__construct(unsigned char *vram, int xsize, int ysize, MemoryPtr memory);
struct SHEET *sheet_alloc();
void Sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void Sheet_updown(struct SHEET *sht, int height);
void Sheet_refresh();
void Sheet_wtf();
void Sheet_refreshmap(int rfx0, int rfy0, int rfxsize, int rfysize, int h0, int h1);
void Sheet_refreshsub(int rfx0, int rfy0, int rfxsize, int rfysize, int h0, int h1);
void Sheet_slide(struct SHEET *sht, int vx0, int vy0);
void Sheet_free(struct SHEET *sht);
void Sheet_put_string(Sheet *sht, char *str, int x, int y, char b, char c);
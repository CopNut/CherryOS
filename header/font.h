#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#define FONT_HEIGHT				16
#define FONT_WIDTH 				8
#define FONT_MARGIN_VERTICAL	1
#define FONT_MARGIN_PARALELL 	1
#define FONT_COLOR				

typedef struct Fontinfo{
	ushort height;
	ushort width;
	ushort margin_vertical;
	ushort margin_parallel;
	ushort height_box;
	ushort width_box;
	uchar color;
}Fontinfo, *FontinfoPtr;

void put_font(char *vram, int xsize, int x, int y, char *font, char color);
void put_string(char *vram, int xsize, int x, int y, char *string, char color);
void fill_box(char *vram, int xsize, int x, int y, char color, int bxsize, int bysize);
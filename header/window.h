#ifndef STDIO_H
#define STDIO_H 
#include <stdio.h>
#endif

#ifndef FONT_H
#define FONT_H 1
#include <font.h>
#endif

#ifndef SHEET_H
#define SHEET_H
#include <sheet.h>
#endif

#ifndef SCREEN_H
#define SCREEN_H
#include <screen.h>
#endif

#ifndef MEMORY_H
#define MEMORY_H
#include <memory.h>
#endif

#define WINDOW_MAX 512

typedef struct { I16P x,y; } GUI_POINT;
typedef struct { I16 x0,y0,x1,y1; } LCD_RECT;
/*typedef struct { GUI_POINT P0, P1; } LCD_RECT; */

typedef struct Window{
	uchar title[64];
	uchar min[16][16];
	uchar max[16][16];
	uchar close[16][16];
	uchar *buf_sheet;
	SheetPtr sht;
}Window, *WindowPtr;

// void Window__construct(WindowPtr this);
void Window__construct(WindowPtr this);
void Window_draw_frame(WindowPtr this, uchar *buf, ushort xsize, ushort ysize);
void Window_decode(uchar *buf, int xszie, int ysize, const uchar *src, char c);
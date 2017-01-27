#ifndef FIFO_H
#define FIFO_H
#include <fifo.h>
#endif

#ifndef SHEET_H
#define SHEET_H 1
#include <sheet.h>
#endif

#ifndef SCREEN_H
#define SCREEN_H 1
#include <screen.h>
#endif

//----------object mouse----------
typedef struct Mouse {
	uint  buf_dcode[3], phase;
	unsigned char xsize, ysize;
	unsigned short px, py;
	char rx, ry;
	unsigned char button;
	char cursor[160];
	char bg[160];
	int aim;
}Mouse, *MousePtr;

//mousebuf[128],decodebuf[3],cursor[160]
void Mouse__construct(Mouse *this, FIFO32Ptr fifo, uint shift);

//
void Mouse_enable(void);

//decode function
int Mouse_dcode(Mouse *this, unsigned char data);

//move the mouse
void Mouse_move(Mouse *this, Screen *scn, Sheet *sht);

#define MOUSE_XSIZE 10
#define MOUSE_YSIZE 16
#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4
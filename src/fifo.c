#include <stdio.h>
#include "CherryOS.h"

#define FLAGS_OVERFLOW 0x0001

void fifob_init(struct FIFOB *fifo, int size, unsigned char *buf)
{
	fifo->buf = buf;
	fifo->wp = 0;
	fifo->rp = 0;
	fifo->size = size;
	fifo->free = size;
	fifo->flags = 0;
	return;
}

int fifob_put(struct FIFOB *fifo, unsigned char data)
{
	if (fifo->free == 0){
		fifo->flags |= FLAGS_OVERFLOW;
		return -1;
	}
	fifo->buf[fifo->wp] = data;
	fifo->wp = (fifo->wp + 1 < fifo->size) ? fifo->wp + 1 : 0;
	fifo->free--;

	return 0;
}

int fifob_get(struct FIFOB *fifo)
{
	int data;
	if (fifo->free == fifo->size){
		return -1;
	}
	data = fifo->buf[fifo->rp];
	fifo->rp = (fifo->rp + 1 < fifo->size) ? fifo->rp + 1 : 0;
	fifo->free++;

	return data;
}

int fifob_status(struct FIFOB *fifo)
{
	return fifo->size - fifo->free;
}
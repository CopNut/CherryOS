#include <stdio.h>
#include <CherryOS.h>

#define FLAGS_OVERFLOW 0x0001

void FIFO32__construct(struct FIFO32 *fifo, int size, uint *buf)
{
	fifo->buf = buf;
	fifo->wp = 0;
	fifo->rp = 0;
	fifo->size = size;
	fifo->free = size;
	fifo->flags = 0;
	return;
}

int FIFO32_put(struct FIFO32 *fifo, uint data)
{
	if (fifo->free == 0){
		fifo->flags |= FLAGS_OVERFLOW;
		return -1;
	}
	fifo->buf[fifo->wp] = data;
	fifo->wp = (fifo->wp + 1 < fifo->size) ? fifo->wp + 1 : 0;
	fifo->free--;

	if (fifo->task != 0)
	{
		if (fifo->task->flags != 2) /* task未处于运行状态 */
		{
			Task_run(fifo->task, -1, 0);
		}
	}

	return 0;
}

int FIFO32_get(struct FIFO32 *fifo)
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

int FIFO32_status(struct FIFO32 *fifo)
{
	return fifo->size - fifo->free;
}
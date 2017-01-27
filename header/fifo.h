

#define FIFO_KEY_START 		0x00
#define FIFO_KEY_END		0xff
#define FIFO_MOUSE_START	0x100
#define FIFO_MOUSE_END		0x1ff
#define FIFO_TIMER_START	0x200
#define FIFO_TIMER_END	0xffffffff
typedef struct FIFO32 {
	uint *buf;
	int wp, rp, size, free, flags;
	struct TASK *task;
}FIFO32, *FIFO32Ptr;

void FIFO32__construct(struct FIFO32 *fifo, int size, uint *buf, struct TASK *task);
int FIFO32_put(struct FIFO32 *fifo, uint data);
int FIFO32_get(struct FIFO32 *fifo);
int FIFO32_status(struct FIFO32 *fifo);
/*asmFunc.asm*/
//----------boot information----------
typedef struct Bootinfo{
	char cyls, leds;
	char vmode, reserve;
	short xsize, ysize;
	char *vram;
	char vga_installed, display_statue;
	short vga_characteristic_parameter;
	char hd0_data[16];
	char hd1_data[16];
}Bootinfo, *BootinfoPtr;

void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
unsigned char io_8bits_in(short port);
void io_8bits_out(short port,unsigned char data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
int io_load_cr0(void);
void io_store_cr0(int cr0);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

#define ADDR_BOOTINFO 0x90000



//----------object screen----------
typedef struct Screen {
	char *vram;
	unsigned short xsize, ysize;
	unsigned int memsize;
	unsigned char bcolor;
	unsigned char *rgbTable;
}Screen, *ScreenPtr;

//Parameter initialization, palette initialization and draw background with pure color(bcolor)
void Screen__construct(Screen *this, Bootinfo *binfo, unsigned char bcolor);

//Palette initialization
void Screen_init_palette(Screen *this);

//draw background
void Screen_draw_bg(Screen *this);

#define VGA_TOTAL_BYTES 0xfa00
#define PALETTE_START 0
#define PALETTE_END 15
#define BLACK 0
#define RED 1
#define YELLOW 2
#define GREEN 3
#define CYAN 4
#define BLUE 5
#define PINK 6
#define WHITE 7
#define GRAY 8
#define DARK_RED 9
#define DARK_YELLOW 10
#define DARK_GREEN 11
#define DARK_CYAN 12
#define DARK_BLUE 13
#define DARK_PINK 14
#define DARK_GRAY 15
#define BCOLOR PINK



//----------object mouse----------
typedef struct Mouse {
	unsigned char buf_code[128], buf_dcode[3], phase;
	unsigned char xsize, ysize;
	unsigned short px, py;
	char rx, ry;
	unsigned char button;
	char cursor[160];
	char bg[160];
}Mouse, *MousePtr;

//mousebuf[128],decodebuf[3],cursor[160]
void Mouse__construct(Mouse *this, Bootinfo *binfo);

//
void Mouse_enable(void);

//decode function
int Mouse_dcode(Mouse *this, unsigned char data);

//move the mouse
void Mouse_move(Mouse *this, Screen *scn);

#define MOUSE_XSIZE 10
#define MOUSE_YSIZE 16



//----------object keyboard----------
typedef struct Keyboard {
	unsigned char buf_code[32];
}Keyboard, *KeyboardPtr;

//
void Keyboard__construct(Keyboard *this);

//wait sendready
void Keyboard_wait_KBC_sendready(void);

#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4



//----------object memory----------
typedef struct MemBlock{
	uint addr, size;
}MemBlock, *MemBlockPtr;

//32K
#define MEM_BLOCK_MAX 		4092 //4096 * (4 + 4) - (4 * 8) / 8 = 4092
typedef struct Memory{
	uint physize;
	uint freesize;
	uint frees;
	uint maxfrees;
	uint usesize;
	uint uses;
	uint lostsize;
	uint losts;	
	MemBlock freeMemBlock[MEM_BLOCK_MAX];
}Memory, *MemoryPtr;


void Memory__construct(Memory *this, uint start, uint end);

//check how much RAM are useful from start to end
uint Memory_check(uint start, uint end);

uint Memory_alloc(Memory *this, uint size);

int Memory_free(Memory *this, uint addr, uint size);


#define ADDR_MEMBUF			0x003c0000
#define EFLAGE_AC_BIT 		0x00040000
#define CR0_CACHE_DISABLE 	0x60000000
#define MEM_CHECK_START 	0x00400000
#define MEM_CHECK_END 		0xffffffff


	void put_font(char *vram, int xsize, int x, int y, char *font, char color);
	void put_string(char *vram, int xsize, int x, int y, char *string, char color);
	void get_box(char *vram, int xsize, int x, int y, char *buffer, int bxsize, int bysize);
	void put_box(char *vram, int xsize, int x, int y, char *buffer, int bxsize, int bysize);
	void fill_box(char *vram, int xsize, int x, int y, char color, int bxsize, int bysize);





/*gdt.c idt.c*/
	struct SEGMENT_DESCRIPTOR {
		short limit_low, base_low;
		char base_mid, access_right;
		char limit_high, base_high;
	}__attribute__((packed));
	struct GATE_DESCRIPTOR {
		short offset_low, selector;
		char dw_count, access_right;
		short offset_high;
	}__attribute__((packed));

	void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
	void gdt_install();
	void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
	void idt_install();

	#define ADR_IDT			0x0026f800
	#define LIMIT_IDT		0x000007ff
	#define ADR_GDT			0x00270000
	#define LIMIT_GDT		0x0000ffff
	#define ADR_BOTPAK		0x00280000
	#define LIMIT_BOTPAK	0x0007ffff
	#define AR_DATA32_RW	0x4092
	#define AR_CODE32_ER	0x409a
	#define AR_INTGATE32	0x008e



/*pic.c*/
	void init_pic(void);
	void inthandler21(int *esp);
	void inthandler27(int *esp);
	void inthandler2c(int *esp);

	#define PIC0_ICW1		0x0020
	#define PIC0_OCW2		0x0020
	#define PIC0_IMR		0x0021
	#define PIC0_ICW2		0x0021
	#define PIC0_ICW3		0x0021
	#define PIC0_ICW4		0x0021
	#define PIC1_ICW1		0x00a0
	#define PIC1_OCW2		0x00a0
	#define PIC1_IMR		0x00a1
	#define PIC1_ICW2		0x00a1
	#define PIC1_ICW3		0x00a1
	#define PIC1_ICW4		0x00a1



/*fifo.c*/
	struct FIFOB {
		unsigned char *buf;
		int wp, rp, size, free, flags;
	};

	void fifob_init(struct FIFOB *fifo, int size, unsigned char *buf);
	int fifob_put(struct FIFOB *fifo, unsigned char data);
	int fifob_get(struct FIFOB *fifo);
	int fifob_status(struct FIFOB *fifo);
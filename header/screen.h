//----------object screen----------

//max sheet
#define MAX_SHEET 256

typedef struct Sheet{
	uchar *buf;
	ushort px, py, xsize, ysize;
	short height;
	uchar flag, color, opacity;
}Sheet, *SheetPtr;

typedef struct Screen {
	//basic
	char *vram;
	uchar *buf_bg;
	unsigned short xsize, ysize;
	unsigned int memsize;
	unsigned char bcolor;
	unsigned char *rgbTable;
	//sheet
	SheetPtr sheetPtr[MAX_SHEET];
	Sheet sheet[MAX_SHEET];
	short top;
}Screen, *ScreenPtr;

//Parameter initialization, palette initialization and draw background with pure color(bcolor)
void Screen__construct(Screen *this, Bootinfo *binfo, unsigned char bcolor);

//Palette initialization
void Screen_init_palette(Screen *this);

//draw background
void Screen_draw_bg(Screen *this);

SheetPtr Screen_alloc(Screen *this, uchar *buf, ushort xsize, ushort ysize, uchar color, uchar opacity);

void Screen_height_shift(Screen *this, SheetPtr sheetPtr, short height_aim);

void Screen_refresh(Screen *this);

void Screen_slide(Screen *this, SheetPtr sheetPtr, short sx, short sy);

void Screen_free(Screen *this, SheetPtr sheetPtr);

void put_font(char *vram, int xsize, int x, int y, char *font, char color);
void put_string(char *vram, int xsize, int x, int y, char *string, char color);
void get_box(char *vram, int xsize, int x, int y, char *buffer, int bxsize, int bysize);
void put_box(char *vram, int xsize, int x, int y, char *buffer, int bxsize, int bysize);
void fill_box(char *vram, int xsize, int x, int y, char color, int bxsize, int bysize);


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

#define USE_SHEET 1
#define UNUSR_SHEET 0
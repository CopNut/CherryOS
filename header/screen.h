//----------object screen----------
typedef struct Screen {
	char *vram;
	uchar *buf_bg;
	unsigned short xsize, ysize;
	unsigned int memsize;
	unsigned char bcolor;
	unsigned char *rgbTable;
}Screen, *ScreenPtr;

//Parameter initialization, palette initialization and draw background with pure color(bcolor)
void Screen__construct(Screen *this, Bootinfo *binfo, uchar *buf_bg, unsigned char bcolor);

//Palette initialization
void Screen_init_palette(Screen *this);

//draw background
void Screen_draw_bg(Screen *this);



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
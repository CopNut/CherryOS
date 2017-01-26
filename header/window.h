#define WINDOW_MAX 512

typedef struct { I16P x,y; } GUI_POINT;
typedef struct { I16 x0,y0,x1,y1; } LCD_RECT;
/*typedef struct { GUI_POINT P0, P1; } LCD_RECT; */

typedef struct Window{
	uchar title[64];
	SheetPtr sht;
}Window, *WindowPtr;

typedef struct WindowIcon{
	uchar min[16][16];
	uchar max[16][16];
	uchar close[16][16];
};

// void Window__construct(WindowPtr this);
void Window__construct(void);
SheetPtr Window_alloc(char *title, ushort x, ushort y);
void Window_draw_frame(struct WindowIcon *this, uchar *buf, ushort xsize, ushort ysize);
void Window_decode(uchar *buf, int xszie, int ysize, const uchar *src, char c);
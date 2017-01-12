#include <stdio.h>
#include <font.h>

extern Fontinfo fontinfo;

void Font__construct(ushort height, ushort width, ushort margin_vertical, ushort margin_parallel, uchar color){

	fontinfo.height 			= height;
	fontinfo.width				= width;
	fontinfo.margin_vertical 	= margin_vertical;
	fontinfo.margin_parallel 	= margin_parallel;
	fontinfo.color 				= color;
	fontinfo.height_box 		= height + 2 * margin_vertical;
	fontinfo.width_box 			= width + 2 * margin_parallel;
	return;
}

void put_font(char *buf, int xsize, int x, int y, char *font, char color){

	int fx0 = x + fontinfo.margin_parallel;
	int fy0 = y + fontinfo.margin_vertical;

	for (int i = 0; i < fontinfo.height; i++) {
	
		for (int j = 0; j < fontinfo.width; ++j){
			buf[(fy0 + i) * xsize + (fx0 + j)] = (font[i] & (1 << (7 - j))) == 0 ? buf[(fy0 + i) * xsize + (fx0 + j)] : color;
		}
	}

	return;
}

void put_string(char *buf, int xsize, int x, int y, char *string, char color){

	int height_box = fontinfo.height_box;
	int width_box = fontinfo.width_box;

	for(int i = 0; string[i] != 0x00; i++){
		put_font(buf, xsize, x, y, asc_table + string[i] * 16, color);
		y = ((x + width_box) > (xsize - width_box)) ? y + height_box : y;
		x = ((x + width_box) > (xsize - width_box)) ? x : x + width_box;

	}
	return;
}

void fill_box(char *buf, int xsize, int x, int y, char color, int bxsize, int bysize){

	for (int i = 0; i < bysize; ++i)
	{
		for (int j = 0; j < bxsize; ++j)
		{
			buf[(y + i) * xsize + x + j] = color;

		}
	}
	return;
}
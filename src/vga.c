#include <stdio.h>
#include <CherryOS.h>


//draw
void put_font(char *vram, int xsize, int x, int y, char *font, char color){

	for (int i = 0; i < 16; i++) {
	
		for (int j = 0; j < 8; ++j){
			vram[(y + i) * xsize + (x + j)] = (font[i] & (1 << (7 - j))) == 0 ? vram[(y + i) * xsize + (x + j)] : color;
		}
	}

	return;
}

void put_string(char *vram, int xsize, int x, int y, char *string, char color){

	for(int i = 0; string[i] != 0x00; i++){
		put_font(vram, xsize, x, y, asc_table + string[i] * 16, color);
		y = ((x + 10) > (320 - 10)) ? y + 20 : y;
		x = ((x + 10) > (320 - 10)) ? 0 : x + 10;

	}
	return;
}

void put_box(char *vram, int xsize, int x, int y, char *buffer, int bxsize, int bysize){

	for (int i = 0; i < bysize; ++i)
	{
		for (int j = 0; j < bxsize; ++j)
		{
			vram[(y + i) * xsize + x + j] = (buffer[bxsize * i + j] == (char)0xff) ? vram[(y + i) * xsize + x + j] : buffer[bxsize * i + j];

		}
	}

	return;
}

void get_box(char *vram, int xsize, int x, int y, char *buffer, int bxsize, int bysize) {

	for (int i = 0; i < bysize; ++i)
	{
		for (int j = 0; j < bxsize; ++j)
		{
			buffer[bxsize * i + j] = vram[(y + i) * xsize + x + j];

		}
	}

	return;
}



void fill_box(char *vram, int xsize, int x, int y, char color, int bxsize, int bysize){

	for (int i = 0; i < bysize; ++i)
	{
		for (int j = 0; j < bxsize; ++j)
		{
			vram[(y + i) * xsize + x + j] = color;

		}
	}
	return;
}
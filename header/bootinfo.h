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

#define ADDR_BOOTINFO 0x90000

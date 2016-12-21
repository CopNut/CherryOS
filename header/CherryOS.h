

#ifndef BOOTINFO_H
#define BOOTINFO_H 1
#include <bootinfo.h>
#endif


#ifndef SCREEN_H
#define SCREEN_H 1
#include <screen.h>
#endif


#ifndef MOUSE_H
#define MOUSE_H 1
#include <mouse.h>
#endif


#ifndef KEYBOARD_H
#define KEYBOARD_H 1
#include <keyboard.h>
#endif


#ifndef MEMORY_H
#define MEMORY_H 1
#include <memory.h>
#endif


#ifndef SHEET_H
#define SHEET_H 1
#include <sheet.h>
#endif

#ifndef TIMER_H
#define TIMER_H
#include <timer.h>
#endif



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

	void asm_inthandler20(void);
	void asm_inthandler21(void);
	void asm_inthandler27(void);
	void asm_inthandler2c(void);

	void inthandler20(int *esp);
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
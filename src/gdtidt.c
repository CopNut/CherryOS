#include <stdio.h>
#include "CherryOS.h"

extern void gdt_flush(int limit, int base);

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
    if (limit > 0xfffff) {
        ar |= 0x8000; /* G_bit = 1 */
        limit /= 0x1000;
    }
    sd->limit_low    = limit & 0xffff;
    sd->base_low     = base & 0xffff;
    sd->base_mid     = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high    = (base >> 24) & 0xff;
    return;
}

void gdt_install()
{

    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;

    for (int i = 0; i <= LIMIT_GDT / 8; i++) {
        set_segmdesc(gdt + i, 0, 0, 0);
    }
    set_segmdesc(gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW);
    set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);

    gdt_flush(LIMIT_GDT, ADR_GDT);
}


extern void idt_load(int limit, int base);

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
    gd->offset_low   = offset & 0xffff;
    gd->selector     = selector;
    gd->dw_count     = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high  = (offset >> 16) & 0xffff;
    return;
}

/* Installs the IDT */
void idt_install()
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */

    struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *)ADR_IDT;

    for (int i = 0; i <= LIMIT_IDT / 8; i++) {
        set_gatedesc(idt + i, 0, 0, 0);
    }
    set_gatedesc(idt + 0x20, ((int) asm_inthandler20 - ADR_BOTPAK), 2 * 8, AR_INTGATE32);
    set_gatedesc(idt + 0x21, ((int) asm_inthandler21 - ADR_BOTPAK), 2 * 8, AR_INTGATE32);
    set_gatedesc(idt + 0x27, ((int) asm_inthandler27 - ADR_BOTPAK), 2 * 8, AR_INTGATE32);
    set_gatedesc(idt + 0x2c, ((int) asm_inthandler2c - ADR_BOTPAK), 2 * 8, AR_INTGATE32);

    idt_load(LIMIT_IDT, ADR_IDT);
}
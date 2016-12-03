
ENTRY(_CherryMain)
phys = 0x00280000;
SECTIONS
{
  .text phys : AT(phys) {
    code = .;
    *(.text)
    *(.rdata)
    *(.drectve)
    *(.data)
    *(.bss)
    . = ALIGN(4096);
  }
  
}
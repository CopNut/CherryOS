TOOLPATH 	= ../tools/
HEADERPATH 	= ../header/
LIBPATH 	= ../lib/
LINKPATH	= ../link/



MAKE     = $(TOOLPATH)make.exe -r
NASM     = $(TOOLPATH)nasm.exe
DD 		 = $(TOOLPATH)dd.exe
GCC		 = gcc
LD		 = ld
AS 		 = as
OBJCOPY  = objcopy
COPY     = copy
DEL      = del

GCC_OPTION = -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc
DATA_IN_TEXT = -Wa,-R
OBJ_CHERRYKERNEL = CherryMain.o asmFunc.o asFunc.o isr.o gdtidt.o fifo.o screen.o font.o keyboard.o mouse.o memory.o sheet.o timer.o window.o $(LIBPATH)ascTable.o $(LIBPATH)cherryLib.o

# 1.  $@     表示要生成的目标
# 2.  $^     表示全部的依赖文件
# 3.  $<     表示第一个依赖文件

default :
	$(MAKE) img


# 一般规则
%.bin : %.asm Makefile
	$(NASM) $*.asm -o $*.bin

%.o : %.c Makefile $(HEADERPATH)stdio.h  $(HEADERPATH)CherryOS.h
	$(GCC) -c -o $*.o $*.c -I $(HEADERPATH) $(GCC_OPTION)

asmFunc.o : asmFunc.asm Makefile
	$(NASM) asmFunc.asm -o asmFunc.o -f coff

asFunc.o : asFunc.asm Makefile
	$(AS) asFunc.asm -o asFunc.o

CherryKernel.tmp : $(OBJ_CHERRYKERNEL) $(LINKPATH)link.ld Makefile
	$(LD) -T $(LINKPATH)link.ld -o CherryKernel.tmp $(OBJ_CHERRYKERNEL) -Map CherryKernel.map

CherryKernel.bin : CherryKernel.tmp Makefile
	$(OBJCOPY) -O binary -j .text CherryKernel.tmp CherryKernel.bin

CherryOS.tmp : ipl.bin asmHead.bin CherryKernel.bin $(LINKPATH)blankFloppyDisk.img Makefile
	$(COPY) /B ipl.bin+asmHead.bin+CherryKernel.bin+..\link\blankFloppyDisk.img CherryOS.tmp

CherryOS.img : CherryOS.tmp Makefile
	$(DD) bs=512 count=2880 if=CherryOS.tmp of=CherryOS.img
	$(DEL) CherryOS.tmp



img :
	$(MAKE) CherryOS.img

vb:
	$(MAKE) img
	$(COPY) CherryOS.img ..\link\CherryOS.img
	C:\Program Files\Oracle\VirtualBox\VBoxManage.exe startvm CherryOS
vm:
	$(MAKE) img
	$(COPY) CherryOS.img ..\link\CherryOS.img
	"C:\Program Files (x86)\VMware\VMware Workstation\vmrun" -T ws start "D:\My Documents\Virtual Machines\CherryOS\CherryOS.vmx"

clean :
	-$(DEL) *.bin
	-$(DEL) *.o
	-$(DEL) *.tmp


src_only :
	$(MAKE) clean
	-$(DEL) *.img
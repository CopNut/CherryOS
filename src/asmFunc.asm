[BITS 32]
;CPU 486
	GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
	GLOBAL  _io_8bits_in, _io_16bits_in, _io_32bits_in
	GLOBAL  _io_8bits_out, _io_16bits_out, _io_32bits_out
	GLOBAL  _io_load_eflags, _io_store_eflags, _io_load_cr0, _io_store_cr0
	GLOBAL  _gdt_flush, _idt_load
	GLOBAL  _Memory_check

[SECTION .text]

_io_hlt:								;void io_hlt(void);
	hlt
	ret

_io_cli:								;void io_cli(void);
	cli
	ret

_io_sti: 								;void io_sti(void);
	sti
	ret

_io_stihlt:								;void io_stihlt(void);
	sti
	hlt
	ret

_io_8bits_in: 							;int io_8bits_in(short port);
	mov 	edx, [esp+4]
	mov 	eax, 0
	in 		al, dx
	ret

_io_16bits_in: 							;int io_16bits_in(short port);
	mov 	edx, [esp+4]
	mov 	eax, 0
	in 		ax, dx
	ret

_io_32bits_in: 							;int io_32bits_in(short port);
	mov 	edx, [esp+4]
	in 		eax, dx
	ret

_io_8bits_out:							;void io_8bits_out(short port, unsigned char data);
	mov 	edx, [esp+4]
	mov 	al, [esp+8]
	out 	dx, al
	ret	

_io_16bits_out:							;void io_16bits_out(short port, unsigned short data);
	mov 	edx, [esp+4]
	mov 	ax, [esp+8]
	out 	dx, ax
	ret

_io_32bits_out:							;void io_32bits_out(short port, unsigned int data);
	mov 	edx, [esp+4]
	mov 	eax, [esp+8]
	out 	dx, eax
	ret

_io_load_eflags:						;int io_load_eflags(void);
	pushfd
	pop 	eax
	ret

_io_store_eflags:						;void io_store_eflags(int eflags);
	mov 	eax, [esp+4]
	push 	eax
	popfd
	ret

_io_load_cr0:							;int io_load_cr0(void);
	mov 	eax, cr0
	ret

_io_store_cr0:							;void io_store_cr0(int cr0);
	mov 	eax, [esp+4]
	mov 	cr0, eax
	ret


; This will set up our new segment registers. We need to do
; something special in order to set CS. We do what is called a
; far jump. A jump that includes a segment as well as an offset.
; This is declared in C as extern void gdt_flush();

_gdt_flush:

		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_idt_load:
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_Memory_check:;uint Memory_check(uint start, uint end);
		push 	edi
		push 	esi
		push 	ebx
		mov 	esi, 0xaa55aa55
		mov 	edi, 0x55aa55aa
		mov 	eax, [esp+12+4] 			;i = start
Memory_loop:
		mov 	ebx, eax
		add 	ebx, 0xffc					;p = i + 0xffc
		mov 	edx, [ebx] 					;old = *p
		mov 	[ebx], esi 					;*p = 0xaa55aa55
		xor 	dword [ebx], 0xffffffff		;*p ^= 0xffffffff
		cmp 	edi, [ebx]					;if(*p != 0x55aa55aa)
		jne 	Memory_fin
		xor 	dword [ebx], 0xffffffff 	;*p ^= 0xffffffff
		cmp 	esi, [ebx] 					;if(*p != 0xaa55aa55)
		jne 	Memory_fin
		mov 	[ebx], edx 					;*p = old
		add 	eax, 0x1000					;i += 0x1000
		cmp 	eax, [esp+12+8] 			;if(i <= end) goto Memory_loop
		jbe 	Memory_loop

		pop 	ebx
		pop 	esi
		pop 	edi
		ret
Memory_fin:
		mov 	[ebx], edx
		pop 	ebx
		pop 	esi
		pop 	edi
		ret
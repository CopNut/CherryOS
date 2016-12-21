	.GLOBAL  _asm_inthandler20, _asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
	.EXTERN  _inthandler20, _inthandler21, _inthandler27, _inthandler2c

_asm_inthandler20:
		PUSH	%ES
		PUSH	%DS
		PUSHA
		MOV		%ESP,%EAX
		PUSH	%EAX
		MOV		%SS,%AX
		MOV		%AX,%DS
		MOV		%AX,%ES
		CALL	_inthandler20
		POP		%EAX
		POPA
		POP		%DS
		POP		%ES
		IRET

_asm_inthandler21:
		PUSH	%ES
		PUSH	%DS
		PUSHA
		MOV		%ESP,%EAX
		PUSH	%EAX
		MOV		%SS,%AX
		MOV		%AX,%DS
		MOV		%AX,%ES
		CALL	_inthandler21
		POP		%EAX
		POPA
		POP		%DS
		POP		%ES
		IRET

_asm_inthandler27:
		PUSH	%ES
		PUSH	%DS
		PUSHA
		MOV		%ESP,%EAX
		PUSH	%EAX
		MOV		%SS,%AX
		MOV		%AX,%DS
		MOV		%AX,%ES
		CALL	_inthandler27
		POP		%EAX
		POPA
		POP		%DS
		POP		%ES
		IRET

_asm_inthandler2c:
		PUSH	%ES
		PUSH	%DS
		PUSHA
		MOV		%ESP,%EAX
		PUSH	%EAX
		MOV		%SS,%AX
		MOV		%AX,%DS
		MOV		%AX,%ES
		CALL	_inthandler2c
		POP		%EAX
		POPA
		POP		%DS
		POP		%ES
		IRET


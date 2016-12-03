; haribote-os boot asm
; TAB=4

BOTPAK	EQU		0x00280000		; bootpackのロード先
DSKCAC	EQU		0x00100000		; ディスクキャッシュの場所
DSKCAC0	EQU		0x00010000		; ディスクキャッシュの場所（リアルモード）

; BOOT_INFO関係
CYLS	EQU		0x0000			; ブートセクタが設定する
LEDS	EQU		0x0001
VMODE	EQU		0x0002			; 色数に関する情報。何ビットカラーか？
SCRNX	EQU		0x0004			; 解像度のX
SCRNY	EQU		0x0006			; 解像度のY
VRAM	EQU		0x0008			; グラフィックバッファの開始番地
VGA_INSTALLED		EQU	0x000c
DISPLAY_STATUE		EQU	0x000d
VGA_CHARACTERISTIC	EQU	0x000e
HD0_DATA			EQU 0x0010
HD1_DATA 			EQU 0x0020		

		ORG		0x10200			; このプログラムがどこに読み込まれるのか

; 画面モードを設定

		MOV		AL,0x13			; VGAグラフィックス、320x200x8bitカラー
		MOV		AH,0x00
		INT		0x10

		;now DS = 0x9000, bootinfo are loaded in 0x90000~0x90200

		MOV 	AH, 0x12
		MOV 	BL, 0x10
		INT 	0x10

		MOV 	WORD [VGA_INSTALLED], BX
		MOV 	WORD [VGA_CHARACTERISTIC], CX
		MOV		BYTE [VMODE],8	; 画面モードをメモする（C言語が参照する）
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

;check for hd data
	;hd0
		MOV 	AX, 0x0000
		MOV 	DS, AX
		LDS 	SI, [4 * 0x41]
		MOV 	AX, 0x9000
		MOV 	ES, AX
		MOV 	DI, HD0_DATA
		MOV 	CX, 0x10
		REP 	MOVSB
	;hd1
		MOV 	AX, 0X0000
		MOV 	DS, AX
		LDS 	SI, [4 * 0x46]
		MOV 	AX, 0x9000
		MOV 	ES, AX
		MOV 	DI, HD1_DATA
		MOV 	CX, 0x10
		REP 	MOVSB


;キーボードのLED状態をBIOSに教えてもらう

		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

		mov 	ax, cs
		mov 	ds, ax
		mov 	ax, 0x0000


; PICが一切の割り込みを受け付けないようにする
;	AT互換機の仕様では、PICの初期化をするなら、
;	こいつをCLI前にやっておかないと、たまにハングアップする
;	PICの初期化はあとでやる

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; OUT命令を連続させるとうまくいかない機種があるらしいので
		OUT		0xa1,AL

		CLI						; さらにCPUレベルでも割り込み禁止

; CPUから1MB以上のメモリにアクセスできるように、A20GATEを設定

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; プロテクトモード移行
		

CPU 486				; 486の命令まで使いたいという記述
;[BITS 32] ;WTF!without this cant run!

		
		LGDT	[GDTR0]			; 暫定GDTを設定
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; bit31を0にする（ページング禁止のため）
		OR		EAX,0x00000001	; bit0を1にする（プロテクトモード移行のため）
		MOV		CR0,EAX

		JMP		pipelineflush

pipelineflush:
		MOV		AX,1*8			;  読み書き可能セグメント32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX
		;jmp print it is ok untill here
		;jmp 	bootpack

;bootpackの転送

		MOV		ESI,bootpack	; 転送元CherryMain位置
		MOV		EDI,BOTPAK		; 転送先
		MOV		ECX,512*1024/4 	; read 16 sectors
		CALL	memcpy
		;jmp 	go
; ついでにディスクデータも本来の位置へ転送

; まずはブートセクタから

		MOV		ESI,0x7c00		; 転送元
		MOV		EDI,DSKCAC		; 転送先
		MOV		ECX,512/4
		CALL	memcpy

; 残り全部

		MOV		ESI,DSKCAC0	+ 512; 転送元
		MOV		EDI,DSKCAC + 512	; 転送先
		MOV		ECX,0
		MOV		CL,10
		IMUL	ECX,512*18*2/4	; シリンダ数からバイト数/4に変換
		SUB		ECX,512/4		; IPLの分だけ差し引く
		CALL	memcpy

; asmheadでしなければいけないことは全部し終わったので、
;	あとはbootpackに任せる

; bootpackの起動

		;MOV		EBX,BOTPAK
		;MOV		ECX,[EBX+16]
		;ADD		ECX,3			; ECX += 3;
		;SHR		ECX,2			; ECX /= 4;
		;JZ		skip			; 転送するべきものがない
		;MOV		ESI,[EBX+20]	; 転送元
		;ADD		ESI,EBX
		;MOV		EDI,[EBX+12]	; 転送先
		;CALL	memcpy
go:
		MOV		ESP,0x310000	; スタック初期値
		JMP		DWORD 2*8:0x00000000
	
print:
		mov 	esi, 0x000a1000
		mov 	eax, 0x0f0f0000
		mov 	[esi], eax 	

lop:
	hlt
	jmp lop

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; ANDの結果が0でなければwaitkbdoutへ
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 引き算した結果が0でなければmemcpyへ
		RET
; memcpyはアドレスサイズプリフィクスを入れ忘れなければ、ストリング命令でも書ける

		ALIGNB	16
GDT0:
		RESB	8				; ヌルセレクタ
		DW		0xffff,0x0000,0x9200,0x00cf	; 読み書き可能セグメント32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 実行可能セグメント32bit（bootpack用）

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:

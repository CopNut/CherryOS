[BITS 16]
;C0H0S1(所加载的第一个扇区)从0x7c00加载至0x90000处
BOOTSEG        EQU 0x07c0
INITSEG        EQU 0x9000 ;a safe place
ASMHEADLEN     EQU 0x0011
CYLS           EQU 10
;org     07c00h

        JMP             entry
        DB              0x90
        DB              "HARIBOTE"     
        DW              512            
        DB              1              
        DW              1              
        DB              2              
        DW              224            
        DW              2880           
        DB              0xf0           
        DW              9              
        DW              18             
        DW              2              
        DD              0              
        DD              2880           
        DB              0,0,0x29       
        DD              0xffffffff     
        DB              "HARIBOTEOS "  
        DB              "FAT12   "     
        RESB    18  

entry:
        mov     ax, BOOTSEG
        mov     ds, ax
        mov     ax, INITSEG
        mov     es, ax
        mov     cx, 0x100                       ;256个word，即512byte
        cld
        sub     si, si                          ;source ds:si 0x07c0:0x0000
        sub     di, di                          ;destination es:di 0x9000:0x0000
        rep     movsw

        jmp     word INITSEG:go

go:
        mov     ax, cs
        mov     ds, ax
        mov     es, ax

        mov     ss, ax
        mov     sp, 0xff00                      ;stack ptr at 0x9000:0xff00

load_asmHead:
                MOV             AX,0x1020
                MOV             ES,AX
                MOV             CH,0                    ; シリンダ0
                MOV             DH,0                    ; ヘッド0
                MOV             CL,2                    ; セクタ2
readloop:
                MOV             SI,0                    ; 失敗回数を数えるレジスタ
retry:
                MOV             AH,0x02                 ; AH=0x02 : ディスク読み込み
                MOV             AL,1                    ; 1セクタ
                MOV             BX,0
                MOV             DL,0x00                 ; Aドライブ
                INT             0x13                    ; ディスクBIOS呼び出し
                JNC             next                    ; エラーがおきなければnextへ
                ADD             SI,1                    ; SIに1を足す
                CMP             SI,5                    ; SIと5を比較
                JAE             error                   ; SI >= 5 だったらerrorへ
                MOV             AH,0x00
                MOV             DL,0x00                 ; Aドライブ
                INT             0x13                    ; ドライブのリセット
                JMP             retry
next:
                MOV             AX,ES                   ; アドレスを0x200進める
                ADD             AX,0x0020
                MOV             ES,AX                   ; ADD ES,0x020 という命令がないのでこうしている
                ADD             CL,1                    ; CLに1を足す
                CMP             CL,18                   ; CLと18を比較
                JBE             readloop                ; CL <= 18 だったらreadloopへ
                MOV             CL,1
                ADD             DH,1
                CMP             DH,2
                JB              readloop                ; DH < 2 だったらreadloopへ
                MOV             DH,0
                ADD             CH,1
                CMP             CH,CYLS
                JB              readloop                ; CH < CYLS だったらreadloopへ

                ;MOV             byte [0x0030],CH             ; IPLがどこまで読んだのかをメモ
                JMP             word 0x1000:0x0200


;load_asmHead:
;        mov     dx, 0000h                       ;0号磁头,0号驱动器（软盘）
;        mov     cx, 0002h                       ;0号磁道,2号扇区（**第二扇区，第一个扇区编号是1，不是0**）
;        mov     bx, 0200h                       ;本例中es = 9000h,bx = 0200h,romAddress = 90200h
;        mov     ax, 0200h+ASMHEADLEN            ;2号读扇区功能,读入17个扇区,a200h + 2200h - 1 = c3ffh,a200h~c3ffh
;        int     13h
;        jnc     asmHead
;        mov     dx, 0000h
;        mov     ax, 0000h
;        int     13h
;        ;jmp     load_asmHead
;        jmp     error

lop:       
        hlt 
        jmp     $

error:
        mov     ax, errorMessage
        call    print
        call    lop  
ok:  
        mov     ax, okMessage
        call    print
        call    lop
print:  
        mov     bp, ax                  ; ES:BP = 串地址 
        mov     cx, 4                   ; CX = 串长度  
        mov     ax, 01301h              ; AH = 13,  AL = 01h  
        mov     bx, 000ch               ; 页号为0(BH = 0) 黑底红字(BL = 0Ch,高亮)  
        mov     dl, 0  
        int     10h                     ; 10h 号中断  
        ret
  
okMessage:      db "ok",13,10
errorMessage:            db      "er",13,10
 
times   510-($-$$)      db      0       
dw      0xaa55                          ; 结束标志  
asmHead:
        
        
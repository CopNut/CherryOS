//----------object keyboard----------
typedef struct Keyboard {
	unsigned char buf_code[32];
}Keyboard, *KeyboardPtr;

//
void Keyboard__construct(Keyboard *this);

//wait sendready
void Keyboard_wait_KBC_sendready(void);

#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4
#if (!defined(STDIO_H))

#define STDIO_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif


	// --------------------include-other-headers-----------------------
	#include "stdarg.h"
	#include "stddef.h"
	//----------------------define-macro-------------------------------
	/*NULL*/
	#if (!defined(NULL))
		#define NULL	((void *) 0)
	#endif
	//----------------------function-prompt----------------------------
	//asmFunc.asm
	void io_hlt(void);
	void io_cli(void);
	void io_sti(void);
	void io_stihlt(void);
	unsigned char io_8bits_in(short port);
	void io_8bits_out(short port,unsigned char data);
	int io_load_eflags(void);
	void io_store_eflags(int eflags);
	int io_load_cr0(void);
	void io_store_cr0(int cr0);

	/*tools\Cherry\cherryLib.c*/
	size_t strlen(const char *str);
	char *strcpy(char *dst,const char *src);
	void memset(void *dst, char src, size_t count);
	void *memcpy(void *dst, const void *src, size_t count);
	size_t sprintf(char *buffer, const char *format, ...);
	unsigned int write_dec(char* buffer,int value);
	unsigned int write_hex(char* buffer,int value);
	size_t itoa(char *string, int *intPtr);
	int atoi_dec(const uchar *string);
	int atoi_hex(const uchar *string);
	size_t dec_intlen(int integer);

	/*tools\Cherry\ascTable.c*/
	extern char asc_table[4096];
	extern char key_table[0x54];
<<<<<<< HEAD
=======

>>>>>>> 16fb828e5185bb3043d0545ba7b0fc2181dc5107

#if (defined(__cplusplus))
	}
#endif

#endif

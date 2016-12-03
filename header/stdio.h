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
	/*tools\Cherry\cherryLib.c*/
	size_t strlen(const char *str);
	char *strcpy(char *dst,const char *src);
	void memset(void *dst, char src, size_t count);
	void *memcpy(void *dst, const void *src, size_t count);
	size_t sprintf(char *buffer, const char *format, ...);
	unsigned int write_dec(char* buffer,int value);
	unsigned int write_hex(char* buffer,int value);
	size_t itoa(char *string, int *intPtr);
	size_t dec_intlen(int integer);

	/*tools\Cherry\ascTable.c*/
	extern char asc_table[4096];


#if (defined(__cplusplus))
	}
#endif

#endif

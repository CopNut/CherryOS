/* copyright(C) 2003 H.Kawai (under KL-01). */

#if (!defined(STDDEF_H))

#define STDDEF_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

typedef unsigned int size_t;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long  ulong;

#define I8    signed char
#define U8  unsigned char     /* unsigned 8  bits. */
#define I16   signed short    /*   signed 16 bits. */
#define U16 unsigned short    /* unsigned 16 bits. */
#define I32   signed long  	  /*   signed 32 bits. */
#define U32 unsigned long  	  /* unsigned 32 bits. */
#define I16P I16              /*   signed 16 bits OR MORE ! */
#define U16P U16              /* unsigned 16 bits OR MORE ! */

#if (defined(__cplusplus))
	}
#endif

#endif

/* copyright(C) 2003 H.Kawai (under KL-01). */

#if (!defined(STDARG_H))

#define STDARG_H	1

#if (defined(__cplusplus))
	extern "C" {
#endif

#ifdef STDARG_H
// #define va_list char *
// #define va_start(v,l)	__builtin_va_start(v,l)
// #define va_end(v)	__builtin_va_end(v)
// #define va_arg(v,l)	__builtin_va_arg(v,l)
// #if !defined(__STRICT_ANSI__) || __STDC_VERSION__ + 0 >= 199900L || defined(__GXX_EXPERIMENTAL_CXX0X__)
// #define va_copy(d,s)	__builtin_va_copy(d,s)
// #endif
// #define __va_copy(d,s)	__builtin_va_copy(d,s)

typedef char * va_list;   
#define _INTSIZEOF(n) \
((sizeof(n)+sizeof(int)-1)&~(sizeof(int) - 1) )   
#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )   
#define va_arg(ap,t) \
( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )   
#define va_end(ap) ( ap = (va_list)0 )
#endif

#if (defined(__cplusplus))
	}
#endif

#endif

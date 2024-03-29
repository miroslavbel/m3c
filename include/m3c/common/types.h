#ifndef _M3C_INCGUARD_TYPES_H
#define _M3C_INCGUARD_TYPES_H

#ifndef m3c_u8
#    ifndef FORBIT_USE_OF_STDTYPES
#        include <limits.h>
#        if UCHAR_MAX >= 0xff
typedef unsigned char m3c_u8;
#        elif USHRT_MAX >= 0xff
typedef unsigned short m3c_u8;
#        elif UINT_MAX >= 0xff
typedef unsigned int m3c_u8;
#        elif ULONG_MAX >= 0xff
typedef unsigned long m3c_u8;
#        elif __STDC_VERSION__ >= 199901L && ULLONG_MAX >= 0xff
typedef unsigned long long m3c_u8;
#        else
#            error "Can't find type for m3c_u8 with <limits.h>"
#        endif
#    else
#        error "m3c_u8 is not defined and FORBIT_TO_USE_STDTYPES is defined"
#    endif
#endif

#ifndef m3c_u16
#    ifndef FORBIT_USE_OF_STDTYPES
#        include <limits.h>
#        if UCHAR_MAX >= 0xffff
typedef unsigned char m3c_u16;
#        elif USHRT_MAX >= 0xffff
typedef unsigned short m3c_u16;
#        elif UINT_MAX >= 0xffff
typedef unsigned int m3c_u16;
#        elif ULONG_MAX >= 0xffff
typedef unsigned long m3c_u16;
#        elif __STDC_VERSION__ >= 199901L && ULLONG_MAX >= 0xffff
typedef unsigned long long m3c_u16;
#        else
#            error "Can't find type for m3c_u16 with <limits.h>"
#        endif
#    else
#        error "m3c_u16 is not defined and FORBIT_TO_USE_STDTYPES is defined"
#    endif
#endif

#define M3C_U16_MAX 0xFFFF

#ifndef m3c_u32
#    ifndef FORBIT_USE_OF_STDTYPES
#        include <limits.h>
#        if UCHAR_MAX >= 0xffffffff
typedef unsigned char m3c_u32;
#        elif USHRT_MAX >= 0xffffffff
typedef unsigned short m3c_u32;
#        elif UINT_MAX >= 0xffffffff
typedef unsigned int m3c_u32;
#        elif ULONG_MAX >= 0xffffffff
typedef unsigned long m3c_u32;
#        elif __STDC_VERSION__ >= 199901L && ULLONG_MAX >= 0xffffffff
typedef unsigned long long m3c_u32;
#        else
#            error "Can't find type for m3c_u32 with <limits.h>"
#        endif
#    else
#        error "m3c_u32 is not defined and FORBIT_TO_USE_STDTYPES is defined"
#    endif
#endif

#ifndef m3c_i32
#    ifndef FORBIT_USE_OF_STDTYPES
#        include <limits.h>
#        if SCHAR_MAX >= 0x7fffffff
#            define M3C_I32_MAX SCHAR_MAX
typedef signed char m3c_i32;
#        elif SHRT_MAX >= 0x7fffffff
#            define M3C_I32_MAX SHRT_MAX
typedef signed short m3c_i32;
#        elif INT_MAX >= 0x7fffffff
#            define M3C_I32_MAX INT_MAX
typedef signed int m3c_i32;
#        elif LONG_MAX >= 0x7fffffff
#            define M3C_I32_MAX LONG_MAX
typedef signed long m3c_i32;
#        elif __STDC_VERSION__ >= 199901L && LLONG_MAX >= 0x7fffffff
#            define M3C_I32_MAX LLONG_MAX
typedef signed long long m3c_i32;
#        else
#            error "Can't find type for m3c_i32 with <limits.h>"
#        endif
#    else
#        error "m3c_i32 is not defined and FORBIT_TO_USE_STDTYPES is defined"
#    endif
#endif

#ifndef m3c_size_t
#    ifndef FORBIT_USE_OF_STDTYPES
#        include <stddef.h>
typedef size_t m3c_size_t;
#        ifndef M3C_SIZE_MAX
#            if __STDC_VERSION__ >= 199901L
#                include <stdint.h>
#                define M3C_SIZE_MAX SIZE_MAX
#            else
#                error "M3C_SIZE_MAX is not manually defined and there is no SIZE_MAX before C99"
#            endif
#        endif
#    else
#        error "m3c_size_t is not defined and FORBIT_TO_USE_STDTYPES is defined"
#    endif
#endif

#ifndef m3c_ptrdiff_t
#    ifndef FORBIT_USE_OF_STDTYPES
#        include <stddef.h>
typedef ptrdiff_t m3c_ptrdiff_t;
#    else
#        error "m3c_ptrdiff_t is not defined and FORBIT_TO_USE_STDTYPES is defined"
#    endif
#endif

#ifndef m3c_bool
#    if __STDC_VERSION__ >= 199901L && !defined(FORBIT_USE_OF_STDTYPES)
#        include <stdbool.h>
typedef bool m3c_bool;
#    else
typedef int m3c_bool;
#    endif
#endif

#ifndef m3c_false
#    if __STDC_VERSION__ >= 199901L && !defined(FORBIT_USE_OF_STDTYPES)
#        include <stdbool.h>
#        define m3c_false false
#    else
#        define m3c_false 0
#    endif
#endif

#ifndef m3c_true
#    if __STDC_VERSION__ >= 199901L && !defined(FORBIT_USE_OF_STDTYPES)
#        include <stdbool.h>
#        define m3c_true true
#    else
#        define m3c_true 1
#    endif
#endif

#ifndef m3c_max_align_t
#    if __STDC_VERSION__ >= 201112L && !defined(FORBIT_USE_OF_STDTYPES)
#        include <stddef.h>
#        define m3c_max_align_t max_align_t
#    endif
#endif

#ifndef M3C_NULL
#    ifndef FORBIT_USE_OF_STDTYPES
#        include <stddef.h>
#        define M3C_NULL NULL
#    else
#        define M3C_NULL ((void *)0)
#    endif
#endif

#endif /* _M3C_INCGUARD_TYPES_H */

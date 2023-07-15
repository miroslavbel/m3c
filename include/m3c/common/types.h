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

#ifndef m3c_size_t
#    ifndef FORBIT_USE_OF_STDTYPES
#        include <stddef.h>
typedef size_t m3c_size_t;
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

#ifndef M3C_NULL
#    ifndef FORBIT_USE_OF_STDTYPES
#        include <stddef.h>
#        define M3C_NULL NULL
#    else
#        define M3C_NULL ((void *)0)
#    endif
#endif

#endif /* _M3C_INCGUARD_TYPES_H */

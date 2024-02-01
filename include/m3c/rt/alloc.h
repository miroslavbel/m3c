#ifndef _M3C_INCGUARD_RT_ALLOC_H
#define _M3C_INCGUARD_RT_ALLOC_H

#ifdef M3C_FEATURE_API_STD

#    include <stdlib.h>

#    define m3c_malloc(size) malloc(size)
#    define m3c_realloc(ptr, size) realloc(ptr, size)
#    define m3c_free(ptr) free(ptr)

#elif defined(M3C_FEATURE_API_SYSCALLS)

#    include <m3c/rt/runtime.h>

#    define m3c_malloc(size) __M3C_Runtime_Malloc((size))
#    define m3c_realloc(ptr, size) __M3C_Runtime_Realloc((ptr), (size))
#    define m3c_free(ptr) __M3C_Runtime_Free((ptr))

#endif /* M3C_FEATURE_API_? */

#endif /* _M3C_INCGUARD_RT_ALLOC_H */

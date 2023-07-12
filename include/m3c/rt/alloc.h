#ifndef _M3C_INCGUARD_RT_ALLOC_H
#define _M3C_INCGUARD_RT_ALLOC_H

#include <stdlib.h>

#define m3c_malloc(size) malloc(size)

#define m3c_realloc(ptr, size) realloc(ptr, size)

#define m3c_free(ptr) free(ptr)

#endif /* _M3C_INCGUARD_RT_ALLOC_H */

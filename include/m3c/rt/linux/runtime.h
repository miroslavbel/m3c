#ifndef _M3C_INCGUARD_RT_LINUX_RUNTIME_H
#define _M3C_INCGUARD_RT_LINUX_RUNTIME_H

#include <m3c/rt/syscalls.h>

/**
 * \brief Number of bytes in one kibibyte.
 */
#define M3C_KiB 1024U

/**
 * \brief Number of bytes in one mebibyte.
 */
#define M3C_MiB M3C_KiB * 1024

/**
 * \brief Number of bytes in one gibibyte.
 */
#define M3C_GiB M3C_MiB * 1024

/**
 * \brief Inits runtime.
 *
 * \return
 * + on success - zero
 * + on error - negative number
 */
int M3C_Runtime_New(void);

void *__M3C_Runtime_Malloc(m3c_size_t size);

void *__M3C_Runtime_Realloc(void *ptr, m3c_size_t new_size);

void __M3C_Runtime_Free(void *ptr);

#endif /* _M3C_INCGUARD_RT_LINUX_RUNTIME_H */

#ifndef _M3C_INCGUARD_RT_MEM_H
#define _M3C_INCGUARD_RT_MEM_H

#include <m3c/common/env.h>
#include <m3c/common/types.h>
#include <m3c/common/babel.h>

#ifdef M3C_FEATURE_USE_COMPILER_BUILTIN_FUNCTIONS

#    if defined(M3C_GNUC) || defined(M3C_CLANG)

/* See:
 * + https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
 * + https://clang.llvm.org/docs/LanguageExtensions.html#builtin-functions
 */

#        define m3c_memcpy(dest, src, count) __builtin_memcpy(dest, src, count)
#        define m3c_memmove(dest, src, count) __builtin_memmove(dest, src, count)

#    endif /* M3C_GNUC || M3C_CLANG */

#endif /* M3C_FEATURE_USE_COMPILER_BUILTIN_FUNCTIONS */

/**
 * \brief Fills `dest` with the contents of `src`. The specified `len` and `count` parameters
 * determine the length and number of iterations of the copying process, respectively.
 *
 * \param[out] dest  buffer to be filled
 * \param[in]  src   buffer to be copied from
 * \param      len   length of `src`
 * \param      count the number or iterations
 * \return           `dest` buffer
 *
 * \warning
 * + `src` length must be at least `len` bytes
 * + `dest` length must be at least `len * count` bytes
 * + `dest` and `src` must not overlap
 * + `src` and `dest` must not be an invalid or null pointer
 */
void *m3c_memfill(
    void *m3c_restrict dest, const void *m3c_restrict src, m3c_size_t len, m3c_size_t count
);

#endif /* _M3C_INCGUARD_RT_MEM_H */

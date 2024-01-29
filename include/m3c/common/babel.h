#ifndef _M3C_INCGUARD_BABEL_H
#define _M3C_INCGUARD_BABEL_H

#include <m3c/common/env.h>

#if __STDC_VERSION__ >= 202300L
#    define m3c_alignof(TYPE) alignof(TYPE)
#elif __STDC_VERSION__ >= 201112L
#    define m3c_alignof(TYPE) _Alignof(TYPE)
#elif defined(M3C_GNUC) || defined(M3C_CLANG)
#    define m3c_alignof(TYPE) __alignof__(TYPE)
#endif

#if __STDC_VERSION__ >= 199901L
#    define m3c_restrict restrict
#else
#    define m3c_restrict /* just ignore before C99 */
#endif

#if __STDC_VERSION__ >= 202311L
#    define m3c_unused [[maybe_unused]]
#elif defined(M3C_CLANG) || defined(M3C_CLANG)
#    define m3c_unused __attribute__((unused))
#else
#    define m3c_unused
#endif

#if defined(M3C_GNUC) || defined(M3C_CLANG)
#    define M3C_SYSV_ABI __attribute__((sysv_abi))
#endif

#endif /* _M3C_INCGUARD_BABEL_H */

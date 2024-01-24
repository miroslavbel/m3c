#ifndef _M3C_INCGUARD_COMMON_ENV
#define _M3C_INCGUARD_COMMON_ENV

/**
 * \file
 *
 * \brief Environment macros (kernel, os, arch).
 *
 * \see Macros are mainly taken from
 * <a href="https://sourceforge.net/projects/predef/">Pre-defined Compiler Macros</a> project.
 */

#ifdef __clang__
#    define M3C_CLANG 1
#elif defined(__GNUC__)
#    define M3C_GNUC 1
#endif

#if defined(__linux__) || defined(__linux) || defined(linux)
#    define M3C_KERNEL_LINUX 1
#endif

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) ||          \
    defined(_M_X64) || defined(_M_AMD64)
#    define M3C_ARCH_X86_64 1
#endif

#endif /* _M3C_INCGUARD_COMMON_ENV */

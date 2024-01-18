#ifndef _M3C_INCGUARD_RT_LINUX_X86_64_MAIN
#define _M3C_INCGUARD_RT_LINUX_X86_64_MAIN

#include <m3c/common/babel.h>

#if defined(M3C_KERNEL_LINUX) && defined(M3C_ARCH_X86_64)

int M3C_SYSV_ABI main(int argc, char *argv[]);

#endif /* M3C_KERNEL_LINUX && M3C_ARCH_X86_64 */

#endif /* _M3C_INCGUARD_RT_LINUX_X86_64_MAIN */

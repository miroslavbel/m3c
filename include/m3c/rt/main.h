#ifndef _M3C_INCGUARD_RT_MAIN
#define _M3C_INCGUARD_RT_MAIN

#include <m3c/common/env.h>

#ifdef M3C_KERNEL_LINUX

#    ifdef M3C_ARCH_X86_64
#        include <m3c/rt/linux/x86-64/main.h>
#    endif /* M3C_ARCH_X86_64 */

#endif /* M3C_KERNEL_LINUX */

#endif /* _M3C_INCGUARD_RT_MAIN */

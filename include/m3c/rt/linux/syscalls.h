#ifndef _M3C_INCGUARD_RT_LINUX_SYSCALLS_H
#define _M3C_INCGUARD_RT_LINUX_SYSCALLS_H

#include <m3c/common/babel.h>

/**
 * \brief Performs a syscall with 6 arguments.
 *
 * \param sysno syscall number
 * \param arg1  first argument
 * \param arg2  second argument
 * \param arg3  third argument
 * \param arg4  forth argument
 * \param arg5  fifth argument
 * \param arg6  sixth argument
 *
 * \return syscall result
 */
long M3C_SYSV_ABI
m3c_syscall6(long sysno, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6);

/**
 * \brief Performs a syscall with 5 arguments.
 *
 * \param sysno syscall number
 * \param arg1  first argument
 * \param arg2  second argument
 * \param arg3  third argument
 * \param arg4  forth argument
 * \param arg5  fifth argument
 *
 * \return syscall result
 */
#define m3c_syscall5(sysno, arg1, arg2, arg3, arg4, arg5)                                          \
    m3c_syscall6((sysno), (arg1), (arg2), (arg3), (arg4), (arg5), 0)

/**
 * \brief Performs a syscall with 4 arguments.
 *
 * \param sysno syscall number
 * \param arg1  first argument
 * \param arg2  second argument
 * \param arg3  third argument
 * \param arg4  forth argument
 *
 * \return syscall result
 */
#define m3c_syscall4(sysno, arg1, arg2, arg3, arg4)                                                \
    m3c_syscall5((sysno), (arg1), (arg2), (arg3), (arg4), 0)

/**
 * \brief Performs a syscall with 3 arguments.
 *
 * \param sysno syscall number
 * \param arg1  first argument
 * \param arg2  second argument
 * \param arg3  third argument
 *
 * \return syscall result
 */
#define m3c_syscall3(sysno, arg1, arg2, arg3) m3c_syscall4((sysno), (arg1), (arg2), (arg3), 0)

/**
 * \brief Performs a syscall with 2 arguments.
 *
 * \param sysno syscall number
 * \param arg1  first argument
 * \param arg2  second argument
 *
 * \return syscall result
 */
#define m3c_syscall2(sysno, arg1, arg2) m3c_syscall3((sysno), (arg1), (arg2), 0)

/**
 * \brief Performs a syscall with one argument.
 *
 * \param sysno syscall number
 * \param arg   argument
 *
 * \return syscall result
 */
#define m3c_syscall1(sysno, arg) m3c_syscall2((sysno), (arg), 0)

/**
 * \brief Performs a syscall with no arguments.
 *
 * \param sysno syscall number
 *
 * \return syscall result
 */
#define m3c_syscall0(sysno) m3c_syscall1((sysno), 0)

#endif /* _M3C_INCGUARD_RT_LINUX_SYSCALLS_H */

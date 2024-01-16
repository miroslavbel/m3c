#ifndef _M3C_INCGUARD_RT_SYSCALLS_H
#define _M3C_INCGUARD_RT_SYSCALLS_H

#include <m3c/common/babel.h>

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
long M3C_SYSV_ABI m3c_syscall3(long sysno, long arg1, long arg2, long arg3);

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
 * \brief Performs a syscall with one arguments.
 *
 * \param sysno syscall number
 * \param arg   argument
 *
 * \return syscall result
 */
#define m3c_syscall1(sysno, arg1) m3c_syscall3((sysno), (arg), 0, 0)

/**
 * \brief Performs a syscall with no arguments.
 *
 * \param sysno syscall number
 *
 * \return syscall result
 */
#define m3c_syscall0(sysno) m3c_syscall3((sysno), 0, 0, 0)

#endif /* _M3C_INCGUARD_RT_SYSCALLS_H */

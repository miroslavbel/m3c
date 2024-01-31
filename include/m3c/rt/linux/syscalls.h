#ifndef _M3C_INCGUARD_RT_LINUX_SYSCALLS_H
#define _M3C_INCGUARD_RT_LINUX_SYSCALLS_H

#include <m3c/common/babel.h>
#include <m3c/common/macros.h>

#include <syscall.h> /* for syscall numbers */
#include <errno.h>   /* for syscall errors */

/* some headers for convenient syscalls use */
#include <fcntl.h>    /* for open */
#include <sys/mman.h> /* for mmap */

/**
 * \brief Checks if result returned from syscall is an error.
 *
 * \see https://stackoverflow.com/a/18998521
 */
#define M3C_IsRawErrno(x) M3C_InRange((long)(x), -4095, -1)

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

#ifdef SYS_open
/**
 * \brief Raw wrapper for `open` syscall.
 *
 * \details See https://man7.org/linux/man-pages/man2/open.2.html
 *
 * \param[in] path  pathname
 * \param     flags flags
 *
 * \return
 * + on error - errno (see #M3C_IsRawErrno)
 * + on success - fd
 */
int m3c_syscall_open(const char *path, int flags);
#endif /* SYS_open */

#ifdef SYS_close
/**
 * \brief Raw wrapper for `close` syscall.
 *
 * \details See https://man7.org/linux/man-pages/man2/close.2.html
 *
 * \param fd file descriptor
 *
 * \return
 * + on error - errno (see #M3C_IsRawErrno)
 * + on success - `0`
 */
int m3c_syscall_close(int fd);
#endif /* SYS_close */

#ifdef SYS_mmap
/**
 * \brief Raw wrapper for `mmap` syscall.
 *
 * \details See https://man7.org/linux/man-pages/man2/mmap.2.html
 *
 * \param addr   mapping address
 * \param length mapping length
 * \param prot   mapping memory protection flags
 * \param flags  mapping update flags
 * \param fd     file descriptor
 * \param offset mapping offset
 *
 * \return
 * + on error - errno (see #M3C_IsRawErrno)
 * + on success - address of mapping
 */
void *m3c_syscall_mmap(void *addr, long length, int prot, int flags, int fd, long offset);
#endif /* SYS_mmap */

#endif /* _M3C_INCGUARD_RT_LINUX_SYSCALLS_H */

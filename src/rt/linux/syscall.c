#include <m3c/rt/linux/syscalls.h>

#ifdef SYS_open
int m3c_syscall_open(const char *path, int flags) {
    return (int)m3c_syscall2(SYS_open, (long)path, (long)flags);
}
#endif /* SYS_open */

#ifdef SYS_close
int m3c_syscall_close(int fd) { return (int)m3c_syscall1(SYS_close, (long)fd); }
#endif /* SYS_close */

#ifdef SYS_mmap
void *m3c_syscall_mmap(void *addr, long length, int prot, int flags, int fd, long offset) {
    return (void *)m3c_syscall6(
        SYS_mmap, (long)addr, length, (long)prot, (long)flags, (long)fd, offset
    );
}
#endif /* SYS_mmap */

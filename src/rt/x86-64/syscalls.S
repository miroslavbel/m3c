.text

.globl m3c_syscall6

/*
 * \brief Syscall with 3 arguments.
 *
 * \param RDI - sysno
 * \param RSI - arg1
 * \param RDX - arg2
 * \param RCX - arg3
 * \param r8  - arg4
 * \param r9  - arg5
 * \param on stack - arg6
 *
 * \destroys `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`, `r10`, `r11`
 *
 * \return RAX
 *
 * \note `syscall` instruction by itself destroys `rcx`, `r11`
 *
 * \see
 * <a href="https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf">AMD64 ABI Draft 0.99.6</a>:
 * + syscall calling convention - A.2.1 Calling Conventions
 * + sysv_abi calling convention - 3.2.3 Parameter Passing
 */
m3c_syscall6:
    mov       %rdi,  %rax  // sysno
    mov       %rsi,  %rdi  // arg1
    mov       %rdx,  %rsi  // arg2
    mov       %rcx,  %rdx  // arg3
    mov       %r8 ,  %r10  // arg4
    mov       %r9 ,  %r8   // arg5
    mov    8(%rsp),  %r9   // arg6
    syscall

    // %rax have been already set by syscall
    ret                // no need to realease stack params

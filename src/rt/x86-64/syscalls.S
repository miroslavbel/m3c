/*
 * \brief Syscall with 3 arguments.
 *
 * \param RDI - sysno
 * \param RSI - arg1
 * \param RDX - arg2
 * \param RCX - arg3
 * \destroys `rdi`, `rsi`, `rdx`, `rcx`, `r11`
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
m3c_syscall3:
    mov    %rdi, %rax  // sysno
    mov    %rsi, %rdi  // arg1
    mov    %rdx, %rsi  // arg2
    mov    %rcx, %rdx  // arg3
    syscall

    // %rax have been already set by syscall
    ret                // no need to realease stack params

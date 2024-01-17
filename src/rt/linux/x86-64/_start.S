.text

.globl _start

/*
 * \brief Simple `start` routine.
 *
 * \warning After `main` function finishes, it immediately syscalls `exit` with the result
 * of `main` function without any additional actions (e.g., stdlib requires to call functions
 * passed to stdlib `atexit` before terminating the process).
 *
 * /see <a href="https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf">AMD64 ABI Draft 0.99.6</a>
 * 3.4 Process Initialization
 */
_start:
    // "the user code should mark the deepest stack frame by setting the frame pointer to zero"
    xor %rbp, %rbp

    // setting `main` params. Ignoring envp and auxiliary vector
    pop %rdi       // argc -> %rdi
    mov %rsp, %rsi // argv -> %rsi

    /* NOTE: %rsp needs to be 16-byte aligned and it'is guaranteed to be so at process entry. But
     * we poped %rdi (8 bytes) we need to align it again. */
    and $-16, %rsp

    call main

// Do not separate! It's a sublabel of `_start`.
_exit:
    // syscalling exit
    mov %rax, %rdi // arg1 = result of main
    mov  $60, %rax // sysno. See `<asm/unistd.h>`
    syscall

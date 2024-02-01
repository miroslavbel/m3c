#include <m3c/rt/linux/runtime.h>

#include <m3c/rt/allocator/bump.h>

typedef struct __tagM3C_Runtime {
    /**
     * \brief File descriptor of `/dev/zero`.
     *
     * \details Used in `mmap` calls.
     */
    int zero;
    /**
     * \brief Heap address.
     */
    void *heap;
    /**
     * \brief Global allocator.
     */
    M3C_BumpAllocator globalAllocator;
} M3C_Runtime;

static M3C_Runtime __m3c_rt;

int M3C_Runtime_New(void) {
    /* NOTE: should be greater then zero
     * NOTE: we use 4GiB at the start as Linux doesn't **really** allocate it anyway
     */
    long heapSize = 4 * M3C_GiB;

    /* NOTE: we don't check the result here, as we called `mmap` later with the this fd */
    __m3c_rt.zero = m3c_syscall_open("/dev/zero", O_RDWR);

    __m3c_rt.heap =
        m3c_syscall_mmap(M3C_NULL, heapSize, PROT_READ | PROT_WRITE, MAP_SHARED, __m3c_rt.zero, 0);
    if (M3C_IsRawErrno(__m3c_rt.heap))
        return -1;

    M3C_BumpAllocator_New(
        &__m3c_rt.globalAllocator, __m3c_rt.heap, (char *)__m3c_rt.heap + heapSize - 1
    );

    return 0;
}

void *__M3C_Runtime_Malloc(m3c_size_t size) {
    return M3C_BumpAllocator_Alloc(&__m3c_rt.globalAllocator, size);
}

void *__M3C_Runtime_Realloc(void *ptr, m3c_size_t new_size) {
    return M3C_BumpAllocator_Realloc(&__m3c_rt.globalAllocator, ptr, new_size);
}

void __M3C_Runtime_Free(void *ptr) {
    return M3C_BumpAllocator_Free(&__m3c_rt.globalAllocator, ptr);
}

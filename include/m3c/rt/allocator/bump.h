#ifndef _M3C_INCGUARD_RT_ALLOCATOR_BUMP_H
#define _M3C_INCGUARD_RT_ALLOCATOR_BUMP_H

#include <m3c/common/types.h>
#include <m3c/common/babel.h>
#include <m3c/rt/mem.h>

/**
 * \brief Bump allocator.
 *
 * \details Bump allocator is probably the simplest allocator there can be. It does not store any
 * object headers (size, alignment data) in the memory, so it can effectively allocate all the
 * memory (except the last byte) that it is given. However, it is only able to allocate memory after
 * the end (of allocated objects). Therefore, allocation works by allocating memory sequentially at
 * the end of last allocation, reallocation allocates the new memory and copy the previous memory
 * content to the new location, and free is just a noop.
 *
 * \warning This implementation is **not** thread-safe!
 *
 * \note \ref M3C_BumpAllocator_Alloc "Alloc", \ref M3C_BumpAllocator_Calloc "Calloc", and \ref
 * M3C_BumpAllocator_Realloc "Realloc" are only available if `M3C_FUNDAMENTAL_ALIGN` is defined. If
 * defined, it must be set to *fundamental alignment*.
 *
 * \note In general case one only needs \ref M3C_BumpAllocator_Alloc "Alloc", \ref
 * M3C_BumpAllocator_Calloc "Calloc", \ref M3C_BumpAllocator_Realloc "Realloc", and \ref
 * M3C_BumpAllocator_Free "Free".
 *
 * Interface:
 * + new
 *   - \ref M3C_BumpAllocator_New "New"
 * + allocation (allocates the new memory)
 *   - \ref M3C_BumpAllocator_AllocAligned "AllocAligned"
 *   - \ref M3C_BumpAllocator_AllocUnaligned "AllocUnaligned" - as \ref
 * M3C_BumpAllocator_AllocAligned "AllocAligned" but with no alignment
 *   - \ref M3C_BumpAllocator_Alloc "Alloc" - as \ref M3C_BumpAllocator_AllocAligned "AllocAligned"
 * but with *fundamental alignment*
 * + callocation
 *   - \ref M3C_BumpAllocator_CallocAligned "CallocAligned"
 *   - \ref M3C_BumpAllocator_CallocUnaligned "CallocUnaligned" - as \ref
 * M3C_BumpAllocator_CallocAligned "CallocAligned" but with no alignment
 *   - \ref M3C_BumpAllocator_Calloc "Calloc" - as \ref M3C_BumpAllocator_CallocAligned
 * "CallocAligned" but with *fundamental alignment*
 * + reallocation
 *   - \ref M3C_BumpAllocator_ReallocAligned "ReallocAligned"
 *   - \ref M3C_BumpAllocator_ReallocUnaligned "ReallocUnaligned" - as
 * \ref M3C_BumpAllocator_ReallocAligned "ReallocAligned" but with no alignment
 *   - \ref M3C_BumpAllocator_Realloc "Realloc" - as \ref M3C_BumpAllocator_ReallocAligned
 * "ReallocAligned" but with *fundamental alignment*
 * + deallocation
 *   - \ref M3C_BumpAllocator_Free "Free"
 *   - \ref M3C_BumpAllocator_FreeSized "FreeSized"
 *   - \ref M3C_BumpAllocator_FreeAlignedSized "FreeAlignedSized"
 *
 * \note If the user is not interested in alignment, it is completely legal to use
 * different versions of functions (aligned and unaligned) to work with the same object (for example
 * allocate the object with \ref M3C_BumpAllocator_AllocAligned "AllocAligned" and `alignment` not
 * equal to `1`, then reallocate with \ref M3C_BumpAllocator_ReallocUnaligned "ReallocUnaligned"
 * which uses `1` as `alignment`), and finally free with \ref M3C_BumpAllocator_FreeAlignedSized
 * "FreeAlignedSized" with `alignment` not equal to `1`.
 */
typedef struct __tagM3C_BumpAllocator {
    /**
     * \brief Pointer to the first byte in the buffer.
     *
     * \note Must not be `NULL`.
     */
    void *first;
    /**
     * \brief Bump pointer.
     *
     * \details Pointer to the first free byte.
     *
     * \note Must not be `NULL`.
     *
     * \note If it points to the same byte as \ref M3C_BumpAllocator::last "::last", the allocator
     * has no free space as it can't allocate the last byte.
     */
    void *ptr;
    /**
     * \brief Pointer to the last byte in the buffer.
     */
    void *last;
} M3C_BumpAllocator;

/**
 * \brief Inits the bump allocator.
 *
 * \note `first` and `last` may point to not aligned memory.
 *
 * \param[in,out] ba    bump allocator
 * \param         first pointer to the first byte of the buffer. Must not be `NULL`
 * \param         last  pointer to the last byte of the buffer
 */
void M3C_BumpAllocator_New(M3C_BumpAllocator *ba, void *first, void *last);

/**
 * \brief Allocates `size` bytes of uninitialized storage whose alignment is specified by
 * `alignment`.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly reallocate or deallocate but provides
 * \ref M3C_BumpAllocator_ReallocAligned "ReallocAligned" and \ref M3C_BumpAllocator_Free "Free" for
 * compatibility.
 *
 * \note If the user is not interested in alignment, it is completely legal to use different
 * versions of functions (aligned and unaligned) to work with the same object.
 *
 * \note If `size` is zero, it will still try to allocate memory. But the resulting pointer should
 * not be dereferenced.
 *
 * \warning This implementation is not thread-safe!
 *
 * \param[in,out] ba        bump allocator
 * \param         alignment alignment. Must be greater then zero
 * \param         size      number of bytes to allocate. May be zero
 *
 * \return
 * + on failure - `NULL`
 * + on success - pointer to the beginning of newly allocated memory
 *
 * \sa \ref M3C_BumpAllocator_Alloc "Alloc", \ref M3C_BumpAllocator_AllocUnaligned "AllocUnaligned"
 */
void *M3C_BumpAllocator_AllocAligned(M3C_BumpAllocator *ba, m3c_size_t alignment, m3c_size_t size);

/**
 * \brief Allocates `size` bytes of uninitialized storage.
 *
 * \details It's a wrapper over function \ref M3C_BumpAllocator_AllocAligned "AllocAligned", using
 * `1` as `alignment`.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly reallocate or deallocate but
 * provides \ref M3C_BumpAllocator_ReallocUnaligned "ReallocUnaligned" and \ref
 * M3C_BumpAllocator_Free "Free" for compatibility.
 *
 * \note If the user is not interested in alignment, it is completely legal to use different
 * versions of functions (aligned and unaligned) to work with the same object.
 *
 * \note If `size` is zero, it will still try to allocate memory. But the resulting pointer should
 * not be dereferenced.
 *
 * \warning This implementation is not thread-safe!
 *
 * \param[in,out] ba   bump allocator
 * \param         size number of bytes to allocate. May be zero
 *
 * \return
 * + on failure - `NULL`
 * + on success - pointer to the beginning of newly allocated memory
 *
 * \sa \ref M3C_BumpAllocator_Alloc "Alloc", \ref M3C_BumpAllocator_AllocAligned "AllocAligned"
 */
#define M3C_BumpAllocator_AllocUnaligned(ba, size) M3C_BumpAllocator_AllocAligned((ba), 1, (size))

#ifdef M3C_FUNDAMENTAL_ALIGN
/**
 * \brief Allocates `size` bytes of uninitialized storage.
 *
 * \details It's a wrapper over function \ref M3C_BumpAllocator_AllocAligned "AllocAligned", using
 * `M3C_FUNDAMENTAL_ALIGN` as `alignment`.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly reallocate or deallocate but provides
 * \ref M3C_BumpAllocator_Realloc "Realloc" and \ref M3C_BumpAllocator_Free "Free" for
 * compatibility.
 *
 * \note If the user is not interested in alignment, it is completely legal to use different
 * versions of functions (aligned and unaligned) to work with the same object.
 *
 * \note If `size` is zero, it will still try to allocate memory. But the resulting pointer should
 * not be dereferenced.
 *
 * \warning This implementation is not thread-safe!
 *
 * \param[in,out] ba   bump allocator
 * \param         size number of bytes to allocate. May be zero
 *
 * \return
 * + on failure - `NULL`
 * + on success - pointer to the beginning of newly allocated memory
 *
 * \sa \ref M3C_BumpAllocator_AllocAligned "AllocAligned", \ref M3C_BumpAllocator_AllocUnaligned
 * "AllocUnaligned"
 */
#    define M3C_BumpAllocator_Alloc(ba, size)                                                      \
        M3C_BumpAllocator_AllocAligned((ba), M3C_FUNDAMENTAL_ALIGN, (size))
#endif /* M3C_FUNDAMENTAL_ALIGN */

/**
 * \brief Allocates memory for an array (whose alignment is specified by `alignment`) of `num`
 * objects of `size` and initializes all bytes in the allocated storage to zero.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly reallocate or deallocate but provides
 * \ref M3C_BumpAllocator_ReallocAligned "ReallocAligned" and \ref M3C_BumpAllocator_Free "Free" for
 * compatibility.
 *
 * \note If the user is not interested in alignment, it is completely legal to use different
 * versions of functions (aligned and unaligned) to work with the same object.
 *
 * \note If `num * size == 0` is zero, it will still try to allocate memory. But the resulting
 * pointer should not be dereferenced.
 *
 * \warning This implementation is not thread-safe!
 *
 * \param[in,out] ba        bump allocator
 * \param         num       number of objects. May be zero
 * \param         alignment alignment. Must be greater then zero
 * \param         size      size of each object. May be zero
 *
 * \return
 * + on failure - `NULL`
 * + on success - pointer to the beginning of newly allocated memory
 *
 * \sa \ref M3C_BumpAllocator_Calloc "Calloc", \ref M3C_BumpAllocator_CallocUnaligned
 * "CallocUnaligned"
 */
void *M3C_BumpAllocator_CallocAligned(
    M3C_BumpAllocator *ba, m3c_size_t num, m3c_size_t alignment, m3c_size_t size
);

/**
 * \brief Allocates memory for an array of `num` objects of `size` and initializes all bytes in the
 * allocated storage to zero.
 *
 * \details It's a wrapper over function \ref M3C_BumpAllocator_CallocAligned "CallocAligned", using
 * `1` as `alignment`.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly reallocate or deallocate but provides
 * \ref M3C_BumpAllocator_ReallocAligned "ReallocAligned" and \ref M3C_BumpAllocator_Free "Free" for
 * compatibility.
 *
 * \note If the user is not interested in alignment, it is completely legal to use different
 * versions of functions (aligned and unaligned) to work with the same object.
 *
 * \note If `num * size == 0` is zero, it will still try to allocate memory. But the resulting
 * pointer should not be dereferenced.
 *
 * \warning This implementation is not thread-safe!
 *
 * \param[in,out] ba   bump allocator
 * \param         num  number of objects. May be zero
 * \param         size size of each object. May be zero
 *
 * \return
 * + on failure - `NULL`
 * + on success - pointer to the beginning of newly allocated memory
 *
 * \sa \ref M3C_BumpAllocator_Calloc "Calloc", \ref M3C_BumpAllocator_CallocAligned "CallocAligned"
 */
#define M3C_BumpAllocator_CallocUnaligned(ba, num, size)                                           \
    M3C_BumpAllocator_CallocAligned((ba), (num), 1, (size))

#ifdef M3C_FUNDAMENTAL_ALIGN
/**
 * \brief Allocates memory for an array of `num` objects of `size` and initializes all bytes in the
 * allocated storage to zero.
 *
 * \details It's a wrapper over function \ref M3C_BumpAllocator_CallocAligned "CallocAligned", using
 * `M3C_FUNDAMENTAL_ALIGN` as `alignment`.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly reallocate or deallocate but provides
 * \ref M3C_BumpAllocator_ReallocAligned "ReallocAligned" and \ref M3C_BumpAllocator_Free "Free" for
 * compatibility.
 *
 * \note If the user is not interested in alignment, it is completely legal to use different
 * versions of functions (aligned and unaligned) to work with the same object.
 *
 * \note If `num * size == 0` is zero, it will still try to allocate memory. But the resulting
 * pointer should not be dereferenced.
 *
 * \warning This implementation is not thread-safe!
 *
 * \param[in,out] ba   bump allocator
 * \param         num  number of objects. May be zero
 * \param         size size of each object. May be zero
 *
 * \return
 * + on failure - `NULL`
 * + on success - pointer to the beginning of newly allocated memory
 *
 * \sa \ref M3C_BumpAllocator_CallocAligned "CallocAligned", \ref M3C_BumpAllocator_CallocUnaligned
 * "CallocUnaligned"
 */
#    define M3C_BumpAllocator_Calloc(ba, num, size)                                                \
        M3C_BumpAllocator_CallocAligned((ba), (num), M3C_FUNDAMENTAL_ALIGN, (size))
#endif /* M3C_FUNDAMENTAL_ALIGN */

/**
 * \brief Reallocates the given area of memory, guaranteeing alignment of the new block to
 * `alignment`.
 *
 * \details If `ptr` is not `NULL`, it must be previously allocated by any of \ref M3C_BumpAllocator
 * "BumpAllocator"'s `Malloc`, `Calloc` or `Realloc` functions and not yet reallocated with a
 * call to any of \ref M3C_BumpAllocator "BumpAllocator"'s `Realloc` functions. If `ptr` is
 * `NULL`, the behavior is the same as calling `AllocAligned(ba, alignment, new_size)`.
 *
 * If there is not enough memory, the old memory block is not freed and `NULL` is returned.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly free the old memory so it simply
 * allocates the new memory and copies the content of old memory to the newly allocated location.
 * This function is provided for compatibility.
 *
 * \note If the user is not interested in alignment, it is completely legal to use different
 * versions of functions (aligned and unaligned) to work with the same object.
 *
 * \warning This implementation is not thread-safe!
 *
 * \param[in,out] ba        bump allocator
 * \param         ptr       pointer to the memory area to be reallocated. May be `NULL`
 * \param         alignment alignment. Must be greater then zero
 * \param         new_size  new size of the array in bytes. May be `0`
 *
 * \return
 * + on failure - `NULL`
 * + on success - pointer to the beginning of newly allocated memory
 *
 * \sa \ref M3C_BumpAllocator_Realloc "Realloc", \ref M3C_BumpAllocator_ReallocUnaligned "Unaligned"
 */
void *M3C_BumpAllocator_ReallocAligned(
    M3C_BumpAllocator *ba, void *ptr, m3c_size_t alignment, m3c_size_t new_size
);

/**
 * \brief Reallocates the given area of memory.
 *
 * \details It's a wrapper over function \ref M3C_BumpAllocator_ReallocAligned "ReallocAligned",
 * using `1` as `alignment`.
 *
 * If `ptr` is not `NULL`, it must be previously allocated by any of \ref M3C_BumpAllocator
 * "BumpAllocator"'s `Malloc`, `Calloc` or `Realloc` functions and not yet reallocated with a
 * call to any of \ref M3C_BumpAllocator "BumpAllocator"'s `Realloc` functions. If `ptr` is
 * `NULL`, the behavior is the same as calling `AllocAligned(ba, alignment, new_size)`.
 *
 * If there is not enough memory, the old memory block is not freed and `NULL` is returned.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly free the old memory so it simply
 * allocates the new memory and copies the content of old memory to the newly allocated location.
 * This function is provided for compatibility.
 *
 * \note If the user is not interested in alignment, it is completely legal to use different
 * versions of functions (aligned and unaligned) to work with the same object.
 *
 * \warning This implementation is not thread-safe!
 *
 * \param[in,out] ba       bump allocator
 * \param         ptr      pointer to the memory area to be reallocated. May be `NULL`
 * \param         new_size new size of the array in bytes. May be `0`
 *
 * \return
 * + on failure - `NULL`
 * + on success - pointer to the beginning of newly allocated memory
 *
 * \sa \ref M3C_BumpAllocator_Realloc "Realloc", \ref M3C_BumpAllocator_ReallocAligned
 * "ReallocAligned"
 */
#define M3C_BumpAllocator_ReallocUnaligned(ba, ptr, new_size)                                      \
    M3C_BumpAllocator_ReallocAligned((ba), (ptr), 1, (new_size))

#ifdef M3C_FUNDAMENTAL_ALIGN
/**
 * \brief Reallocates the given area of memory.
 *
 * \details It's a wrapper over function \ref M3C_BumpAllocator_ReallocAligned "ReallocAligned",
 * using `M3C_FUNDAMENTAL_ALIGN` as `alignment`.
 *
 * If `ptr` is not `NULL`, it must be previously allocated by any of \ref M3C_BumpAllocator
 * "BumpAllocator"'s `Malloc`, `Calloc` or `Realloc` functions and not yet reallocated with a
 * call to any of \ref M3C_BumpAllocator "BumpAllocator"'s `Realloc` functions. If `ptr` is
 * `NULL`, the behavior is the same as calling `AllocAligned(ba, alignment, new_size)`.
 *
 * If there is not enough memory, the old memory block is not freed and `NULL` is returned.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly free the old memory so it simply
 * allocates the new memory and copies the content of old memory to the newly allocated location.
 * This function is provided for compatibility.
 *
 * \note If the user is not interested in alignment, it is completely legal to use different
 * versions of functions (aligned and unaligned) to work with the same object.
 *
 * \warning This implementation is not thread-safe!
 *
 * \param[in,out] ba       bump allocator
 * \param         ptr      pointer to the memory area to be reallocated. May be `NULL`
 * \param         new_size new size of the array in bytes. May be `0`
 *
 * \return
 * + on failure - `NULL`
 * + on success - pointer to the beginning of newly allocated memory
 *
 * \sa \ref M3C_BumpAllocator_ReallocAligned "ReallocAligned", \ref
 * M3C_BumpAllocator_ReallocUnaligned "ReallocUnaligned"
 */
#    define M3C_BumpAllocator_Realloc(ba, ptr, new_size)                                           \
        M3C_BumpAllocator_ReallocAligned((ba), (ptr), M3C_FUNDAMENTAL_ALIGN, (new_size))
#endif /* M3C_FUNDAMENTAL_ALIGN */

/**
 * \brief Noop.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly deallocate so this macro is a noop
 * provided for compatibility.
 */
#define M3C_BumpAllocator_Free(ba, ptr) /* noop */

/**
 * \brief Noop.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly deallocate so this macro is a noop
 * provided for compatibility.
 */
#define M3C_BumpAllocator_FreeSized(ba, ptr, size) M3C_BumpAllocator_Free((ba), (ptr))

/**
 * \brief Noop.
 *
 * \note \ref M3C_BumpAllocator "BumpAllocator" can't truly deallocate so this macro is a noop
 * provided for compatibility.
 */
#define M3C_BumpAllocator_FreeAlignedSized(ba, ptr, alignment, size)                               \
    M3C_BumpAllocator_Free((ba), (ptr))

#endif /* _M3C_INCGUARD_RT_ALLOCATOR_BUMP_H */

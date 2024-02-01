#include <m3c/rt/allocator/bump.h>

#include <m3c/common/macros.h>

void M3C_BumpAllocator_New(M3C_BumpAllocator *ba, void *first, void *last) {
    ba->first = first;
    ba->ptr = first;
    ba->last = last;
}

/**
 * \brief Tries to allocate a new object from given `ptr`.
 *
 * \details Checks that there is enough space for object to allocate. If so, allocates the object.
 *
 * \note One must ensure that `ba->ptr <= ptr <= ba->last`.
 *
 * \param[in,out] ba   bump allocator
 * \param         ptr  pointer
 * \param         size object size in bytes
 *
 * \return
 * + `NULL` - on error
 * + pointer to the newly allocated object on success
 */
void *__M3C_BumpAllocator_TryAllocFrom(M3C_BumpAllocator *ba, void *ptr, m3c_size_t size) {
    void *res;

    /* Note: In fact, the free space is one byte more. But we can't allocate the last byte */
    m3c_size_t free = (char *)ba->last - (char *)ptr;

    if (size > free)
        return M3C_NULL;

    res = ptr;
    ba->ptr = (char *)ptr + size;
    return res;
}

void *M3C_BumpAllocator_AllocAligned(M3C_BumpAllocator *ba, m3c_size_t alignment, m3c_size_t size) {
    void *alignedPtr;
    m3c_size_t remainder;
    m3c_size_t coremainder;

    /* GUARANTEED: (by caller) `alignment > 0` */
    remainder = (m3c_size_t)ba->ptr % alignment;

    if (remainder == 0)
        alignedPtr = ba->ptr;
    else {
        coremainder = alignment - remainder;

        if ((char *)ba->last - (char *)ba->ptr <= coremainder)
            return M3C_NULL;

        alignedPtr = (char *)ba->ptr + coremainder;
    }

    return __M3C_BumpAllocator_TryAllocFrom(ba, alignedPtr, size);
}

void *M3C_BumpAllocator_CallocAligned(
    M3C_BumpAllocator *ba, m3c_size_t num, m3c_size_t alignment, m3c_size_t size
) {
    void *addr;
    m3c_size_t bSize;

    /* calculating bSize */
    if (num == 0 || size == 0 || M3C_SIZE_MAX / num > size)
        bSize = 0;
    else
        bSize = size * num; /* no overflow */

    addr = M3C_BumpAllocator_AllocAligned(ba, alignment, bSize);

    if (addr)
        m3c_memset(addr, 0, bSize);

    return addr;
}

void *M3C_BumpAllocator_ReallocAligned(
    M3C_BumpAllocator *ba, void *ptr, m3c_size_t alignment, m3c_size_t new_size
) {
    void *res;
    m3c_size_t old_size_max;

    /* NOTE: as we don't know old size of the object so we simply allocate the new one and copy */
    res = M3C_BumpAllocator_AllocAligned(ba, alignment, new_size);

    /* NOTE: `ptr` can be `NULL` too */
    if (res && ptr) {
        /* NOTE: we can't retrieve actual size of object as the allocator doesn't use any headers
         * and it's not guarantee that this object is the last allocated one */
        old_size_max = (char *)ba->ptr - (char *)ptr;

        /* NOTE: we using `min()` here to ensure that the regions will not overlap and we don't
         * touch memory after the allocator buffer */
        m3c_memcpy(res, ptr, m3c_min(old_size_max, new_size));
    }

    return res;
}

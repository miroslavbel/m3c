#include <m3c/common/coltypes.h>

#include <m3c/rt/alloc.h>
#include <m3c/rt/mem.h>

M3C_ERROR
M3C_VEC_Push_impl(
    void **buf, m3c_size_t *len, m3c_size_t *cap, void const *elem, m3c_size_t elemSize
) {
    m3c_size_t newCap;
    void *newPtr;

    if (*len == *cap) {
        newCap = *cap + *cap;

        newPtr = m3c_realloc(*buf, elemSize * newCap);
        if (!newPtr)
            return M3C_ERROR_OOM;

        *cap = newCap;
        *buf = newPtr;
    }

    m3c_memcpy((char *)*buf + elemSize * (*len), elem, elemSize);
    ++(*len);
    return M3C_ERROR_OK;
}

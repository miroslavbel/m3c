#include <m3c/common/coltypes.h>

#include <m3c/rt/alloc.h>
#include <m3c/rt/mem.h>

M3C_ERROR
M3C_VEC_Push_impl(
    void **buf, m3c_size_t *len, m3c_size_t *cap, void const *elem, m3c_size_t elemSize
) {
    m3c_size_t newCap;

    if (*len == *cap) {
        newCap = *cap == 0 ? 1 : *cap + *cap;

        if (M3C_VEC_Reserve_impl(buf, cap, elemSize, newCap) == M3C_ERROR_OOM)
            return M3C_ERROR_OOM;
    }

    m3c_memcpy((char *)*buf + elemSize * (*len), elem, elemSize);
    ++(*len);
    return M3C_ERROR_OK;
}

M3C_ERROR
M3C_VEC_Reserve_impl(void **buf, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t newCap) {
    void *newPtr;

    if (*cap >= newCap)
        return M3C_ERROR_OK;

    newPtr = m3c_realloc(*buf, elemSize * newCap);
    if (!newPtr)
        return M3C_ERROR_OOM;
    *buf = newPtr;
    *cap = newCap;

    return M3C_ERROR_OK;
}

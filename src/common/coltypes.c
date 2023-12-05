#include <m3c/common/coltypes.h>
#include <m3c/common/macros.h>

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

M3C_ERROR M3C_ARR_BSearch_impl(
    void const *buf, m3c_size_t len, m3c_size_t elemSize, void const *elem,
    int (*cmp)(void const *, void const *), m3c_size_t *n
) {
    int cmpRes;
    m3c_size_t max;
    m3c_size_t min = 0;

    if (len == 0) {
        *n = 0;
        return M3C_ERROR_NOT_FOUND;
    }
    max = len - 1;

    M3C_LOOP {
        *n = (max - min) / 2 + min;

        cmpRes = cmp((m3c_u8 const *)buf + elemSize * *n, elem);
        if (cmpRes == 0)
            return M3C_ERROR_OK;
        else if (max == min) {
            if (max + 1 == len && cmpRes < 0)
                ++(*n);
            return M3C_ERROR_NOT_FOUND;
        } else if (cmpRes > 0)
            max = *n;
        else
            min = *n + 1;
    }
}

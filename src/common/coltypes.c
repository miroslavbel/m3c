#include <m3c/common/coltypes.h>
#include <m3c/common/macros.h>

void const *M3C_EchoFn(void const *obj, void const *arg) { return obj; }

void M3C_VEC_Init_impl(void **buf, m3c_size_t *len, m3c_size_t *cap) {
    *buf = M3C_NULL;
    *len = 0;
    *cap = 0;
}

M3C_ERROR M3C_VEC_NewWithCapacity_impl(
    void **buf, m3c_size_t *len, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t initCap
) {
    M3C_VEC_Init_impl(buf, len, cap);

    return M3C_VEC_ReserveExact_impl(buf, cap, elemSize, initCap);
}

void M3C_VEC_Clear_impl(void **buf, m3c_size_t *len, m3c_size_t *cap) {
    M3C_ARR_DeinitBoxed_impl(*buf);

    *buf = M3C_NULL;
    *cap = 0;
    *len = 0;
}

M3C_ERROR M3C_VEC_ReserveUnused_impl(
    void **buf, m3c_size_t const *len, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t n
) {
    m3c_size_t newCap;
    m3c_size_t doubledCap;
    m3c_size_t minCap; /* minimal capacity that can hold all new elements */

    /* NOTE: checking that `n + *len` won't overflow */
    if (n > M3C_SIZE_MAX - *len)
        return M3C_ERROR_OOM;
    minCap = n + *len;

    /* NOTE: avoiding overflow of `*cap + *cap`. Arithmetically it's equivalent to
     * `min(M3C_SIZE_MAX, *cap + *cap)` */
    doubledCap = *cap > M3C_SIZE_MAX - *cap ? M3C_SIZE_MAX : *cap + *cap;

    newCap = *cap == 0 ? minCap : m3c_max(doubledCap, minCap);

    return M3C_VEC_ReserveExact_impl(buf, cap, elemSize, newCap);
}

M3C_ERROR
M3C_VEC_ReserveExact_impl(void **buf, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t newCap) {
    void *newPtr;
    m3c_size_t byteCap; /* capacity in bytes */

    if (*cap >= newCap)
        return M3C_ERROR_OK;

    /* NOTE: checking overflow of `newCap * elemSize`.
     * The code is from https://stackoverflow.com/a/1815371 */
    byteCap = newCap * elemSize;
    if (newCap != 0 && byteCap / newCap != elemSize)
        return M3C_ERROR_OOM;

    newPtr = m3c_realloc(*buf, byteCap);
    if (!newPtr)
        return M3C_ERROR_OOM;
    *buf = newPtr;
    *cap = newCap;

    return M3C_ERROR_OK;
}

M3C_ERROR M3C_VEC_Insert_impl(
    void **buf, m3c_size_t *len, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t index,
    void const *elems, m3c_size_t n
) {

    if (M3C_VEC_ReserveUnused_impl(buf, len, cap, elemSize, n) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;
    (*len) += n;

    /* NOTE: ERROR_OOB - iff `index` is OOB */
    if (M3C_ARR_RShift_impl(*buf, *len, elemSize, index, n) != M3C_ERROR_OK)
        return M3C_ERROR_OOB;

    M3C_ARR_CopyUnsafe_impl(*buf, index, elems, 0, elemSize, n);

    return M3C_ERROR_OK;
}

M3C_ERROR M3C_ARR_BSearch_impl(
    void const *buf, m3c_size_t len, m3c_size_t elemSize, void const *elem, M3C_CMP_FN *cmpFn,
    m3c_size_t *n, M3C_KEY_FN keyFn, void *keyArg
) {
    void const *arrElem;
    int cmpRes;
    m3c_size_t max;
    m3c_size_t min = 0;

    if (len == 0) {
        *n = 0;
        return M3C_ERROR_NOT_FOUND;
    }
    max = len - 1;

    if (!keyFn)
        keyFn = (const void *(*)(const void *, void *))M3C_EchoFn;

    M3C_LOOP {
        *n = (max - min) / 2 + min;

        arrElem = (m3c_u8 const *)buf + elemSize * *n;

        cmpRes = cmpFn(keyFn(arrElem, keyArg), elem);
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

M3C_ERROR M3C_ARR_CopyWithin_impl(
    void *buf, m3c_size_t len, m3c_size_t elemSize, m3c_size_t dstI, m3c_size_t srcI, m3c_size_t n
) {
    /* NOTE: checking that
     * 1. dstI < len && dstI + n <= len
     * 2. srcI < len && srcI + n <= len
     */
    if (dstI >= len || n > len - dstI || srcI >= len || n > len - srcI)
        return M3C_ERROR_OOB;
    M3C_ARR_CopyWithinUnsafe_impl(buf, elemSize, dstI, srcI, n);

    return M3C_ERROR_OK;
}

M3C_ERROR M3C_ARR_Copy_impl(
    void *m3c_restrict dstBuf, m3c_size_t dstLen, m3c_size_t dstI,       /* dst */
    void const *m3c_restrict srcBuf, m3c_size_t srcLen, m3c_size_t srcI, /* src */
    m3c_size_t elemSize, m3c_size_t n
) {
    /* NOTE: checking that
     * 1. dstI < dstLen && dstI + n <= dstLen
     * 2. srcI < srcLen && srcI + n <= srcLen
     */
    if (dstI >= dstLen || n > dstLen - dstI || /* dst */
        srcI >= srcLen || n > srcLen - srcI)   /* src */
        return M3C_ERROR_OOB;
    M3C_ARR_CopyUnsafe_impl(dstBuf, dstI, srcBuf, srcI, elemSize, n);

    return M3C_ERROR_OK;
}

M3C_ERROR M3C_ARR_RShift_impl(
    void *buf, m3c_size_t len, m3c_size_t elemSize, m3c_size_t startI, m3c_size_t step
) {
    m3c_size_t dstI;
    m3c_size_t n;

    /* NOTE: checking that `startI < len` */
    if (startI >= len)
        return M3C_ERROR_OOB;

    /* NOTE: checking that
     * `dstI < len` as
     * `startI + step < len` */
    if (step >= len - startI)
        return M3C_ERROR_OK; /* noop */
    dstI = startI + step;

    /* NOTE: we already know that `dstI < len` so this won't overflow */
    n = len - dstI;

    return M3C_ARR_CopyWithin_impl(buf, len, elemSize, dstI, startI, n);
}

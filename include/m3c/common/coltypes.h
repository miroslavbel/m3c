#ifndef _M3C_INCGUARD_COLTYPES_H
#define _M3C_INCGUARD_COLTYPES_H

#include <m3c/rt/alloc.h>
#include <m3c/rt/mem.h>

#include <m3c/common/types.h>
#include <m3c/common/errors.h>

/**
 * \brief Default initial capacity for vector underlying buffer.
 */
#define M3C_VEC_DEFAULT_INITIAL_CAPACITY 16

/**
 * \brief Macro for defining a vector structure with a given type.
 *
 * \warning Only single-word types are supported.
 */
#define M3C_VEC(TYPE)                                                                              \
    struct __tagM3C_VEC_##TYPE {                                                                   \
        /**                                                                                        \
         * \brief Length of the buffer.                                                            \
         *                                                                                         \
         * \note Can be `0`.                                                                       \
         */                                                                                        \
        m3c_size_t len;                                                                            \
        /**                                                                                        \
         * \brief Capacity of the buffer.                                                          \
         *                                                                                         \
         * \note Can be `0`.                                                                       \
         */                                                                                        \
        m3c_size_t cap;                                                                            \
        /**                                                                                        \
         * \brief Pointer to the buffer.                                                           \
         *                                                                                         \
         * \warning Can be `NULL` iff `cap` is equal to `0`.                                       \
         */                                                                                        \
        TYPE *data;                                                                                \
    }

/**
 * \brief Macro for defining an array structure with a given type.
 *
 * \warning Only single-word types are supported.
 */
#define M3C_ARR(TYPE)                                                                              \
    struct __tagM3C_ARR_##TYPE {                                                                   \
        m3c_size_t len;                                                                            \
        TYPE *data;                                                                                \
    }

/**
 * \brief Comparator function.
 *
 * \details Accepts two pointers to different (or the same) objects and compares them.
 *
 * \param[in] x pointer to the first object
 * \param[in] y pointer to the second object
 *
 * \return
 * + `0`   - if objects are equal
 * + `> 0` - if the object pointed by `x` is greater than the object pointed by `y`
 * + `< 0` - if the object pointed by `x` is less than the object pointed by `y`
 */
typedef int(M3C_CMP_FN)(void const *, void const *);

/**
 * \brief Key extraction function.
 *
 * \note There are no requirements for an `arg`. The function can either mutate it or not need it
 * at all (so it can be `NULL` for some implementations).
 *
 * Types `TYPE_1` and `TYPE_2` can be the same.
 *
 * \param[in]     obj pointer to the object of type `TYPE_1`
 * \param[in,out] arg additional argument
 *
 * \return pointer to an object of type `TYPE_2`
 *
 * \sa Stub function #M3C_EchoFn simply returns the pointer passed in it.
 */
typedef void const *(M3C_KEY_FN)(void const *, void *);

/**
 * \brief Echoed `obj`.
 *
 * \param obj pointer to the object to be echoed
 * \param arg unused argument (but required by the interface)
 * \return the same pointer that was passed to by `obj` argument.
 *
 * \sa #M3C_ARR_BSEARCH_BY_KEY, #M3C_ARR_BSearch_impl
 */
void const *M3C_EchoFn(void const *obj, void const *arg);

/**
 * \brief Inits the vector.
 *
 * \note Doesn't allocate the buffer.
 *
 * \param[out] buf pointer to the pointer to the buffer
 * \param[out] len pointer to the buffer length
 * \param[out] cap pointer to the buffer capacity
 */
void M3C_VEC_Init_impl(void **buf, m3c_size_t *len, m3c_size_t *cap);

/**
 * \brief Inits the vector and allocates the underlying buffer so that it can store exactly
 * `initCap` elements.
 *
 * \param[out] buf      pointer to the pointer to the buffer
 * \param[out] len      pointer to the buffer length
 * \param[out] cap      pointer to the buffer capacity
 * \param      elemSize size of the vector element in bytes
 * \param      initCap  new capacity of the buffer, in number of elements
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to alloc or `elemSize * initCap` will overflow
 */
M3C_ERROR M3C_VEC_NewWithCapacity_impl(
    void **buf, m3c_size_t *len, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t initCap
);

/**
 * \brief Inits the vector and allocates the underlying buffer.
 *
 * \param[out] BUF       pointer to the pointer to the buffer
 * \param[out] LEN       pointer to the buffer length
 * \param[out] CAP       pointer to the buffer capacity
 * \param      ELEM_SIZE size of the vector element in bytes
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to alloc
 */
#define M3C_VEC_New_impl(BUF, LEN, CAP, ELEM_SIZE)                                                 \
    M3C_VEC_NewWithCapacity_impl((BUF), (LEN), (CAP), (ELEM_SIZE), M3C_VEC_DEFAULT_INITIAL_CAPACITY)

/**
 * \brief Deinits the array by freeing its underlying buffer, assuming the buffer has been allocated
 * in the heap.
 *
 * \param BUF pointer to the buffer
 *
 * \warning Doesn't reset the length and the capacity.
 */
#define M3C_ARR_DeinitBoxed_impl(BUF) m3c_free((BUF))

/**
 * \brief Clears the vector.
 *
 * \details Deallocates underlying buffer and resets the capacity and the length to zero.
 *
 * \param[in,out] buf pointer to the pointer to the buffer
 * \param[out]    len pointer to the buffer length
 * \param[out]    cap pointer to the buffer capacity
 */
void M3C_VEC_Clear_impl(void **buf, m3c_size_t *len, m3c_size_t *cap);

/**
 * \brief Inserts `elems` to the vector at index `index`.
 *
 * \param[in,out] buf      pointer to the pointer to the buffer
 * \param[in,out] len      pointer to the buffer length
 * \param[in,out] cap      pointer to the buffer capacity
 * \param         elemSize size of element in bytes
 * \param         index    index before which the elements will be inserted
 * \param[in]     elems    pointer to the elements array to be inserted
 * \param         n        number of elements to insert
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB - if `index` is Out Of Bounds
 * + #M3C_ERROR_OOM - if failed to realloc or the new capacity in bytes will be greater then
 * `M3C_SIZE_MAX`
 */
M3C_ERROR M3C_VEC_Insert_impl(
    void **buf, m3c_size_t *len, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t index,
    void const *elems, m3c_size_t n
);

/**
 * \brief Pushes `ELEMS` to the vector.
 *
 * \details If it is necessary to reallocate the buffer, sets the new capacity to twice the previous
 * capacity using addition. Uses #m3c_realloc as an reallocator.
 *
 * \param[in,out] BUF       pointer to the pointer to the buffer
 * \param[in,out] LEN       pointer to the buffer length
 * \param[in,out] CAP       pointer to the buffer capacity
 * \param[in]     ELEMS     pointer to the elements to be pushed
 * \param         N         number of elements to be pushed
 * \param         ELEM_SIZE size of element in bytes
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to realloc or the new capacity in bytes will be greater then
 * `M3C_SIZE_MAX`
 */
#define M3C_VEC_PushN_impl(BUF, LEN, CAP, ELEMS, N, ELEM_SIZE)                                     \
    M3C_VEC_Insert_impl((BUF), (LEN), (CAP), (ELEM_SIZE), *(LEN), (ELEMS), (N))

/**
 * \brief Pushes `ELEM` to the vector.
 *
 * \details If it is necessary to reallocate the buffer, sets the new capacity to twice the previous
 * capacity using addition. Uses #m3c_realloc as an reallocator.
 *
 * \param[in,out] BUF       pointer to the pointer to the buffer
 * \param[in,out] LEN       pointer to the buffer length
 * \param[in,out] CAP       pointer to the buffer capacity
 * \param[in]     ELEM      pointer to the element to be pushed
 * \param         ELEM_SIZE size of element in bytes
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to realloc or the new capacity in bytes will be greater then
 * `M3C_SIZE_MAX`
 */
#define M3C_VEC_Push_impl(BUF, LEN, CAP, ELEM, ELEM_SIZE)                                          \
    M3C_VEC_PushN_impl((BUF), (LEN), (CAP), (ELEM), 1, (ELEM_SIZE))

/**
 * \brief Increase the capacity of the vector to a value that's equal to `newCap`.
 *
 * \param[in,out] buf      pointer to the pointer to the buffer
 * \param[in,out] cap      pointer to the buffer capacity
 * \param         elemSize size of the vector element in bytes
 * \param         newCap   new capacity of the buffer, in number of elements
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to realloc or `elemSize * newCap` will overflow
 */
M3C_ERROR
M3C_VEC_ReserveExact_impl(void **buf, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t newCap);

/**
 * \brief Increases the capacity of the vector so that it can hold at least `n` new elements.
 *
 * \param[in,out] buf      pointer to the pointer to the buffer
 * \param[in]     len      pointer to the buffer length
 * \param[in,out] cap      pointer to the buffer capacity
 * \param         elemSize size of the vector element in bytes
 * \param         n        number of new elements
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to realloc or the new capacity in bytes will be greater then
 * `M3C_SIZE_MAX`
 */
M3C_ERROR M3C_VEC_ReserveUnused_impl(
    void **buf, m3c_size_t const *len, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t n
);

/**
 * \brief Copies elements from `SRC` array to non-overlapping `DST` array.
 *
 * \warning To use this macro safely, ensure that:
 * 1. `DST_I < DST_LEN` and `DST_I + N <= DST_LEN`
 * 2. `SRC_I < SRC_LEN` and `SRC_I + N <= SRC_LEN`
 * where `DST_LEN` and `SRC_LEN` are the lengths of buffers `DST` and `SRC`, respectively.
 *
 * \param[out] DST       pointer to the first element of the `DST` array
 * \param      DST_I     index of element in `DST` array in place of which the copying is performed
 * \param[in]  SRC       pointer to the first element of the `SRC` array
 * \param      SRC_I     index of the first element in `SRC` array to be copied
 * \param      ELEM_SIZE size of the array element in bytes
 * \param      N         number of elements to be copied
 *
 * \return result of the underlying call to `m3c_memcpy`
 */
#define M3C_ARR_CopyUnsafe_impl(                                                                   \
    DST, DST_I, /* dst */                                                                          \
    SRC, SRC_I, /* src */                                                                          \
    ELEM_SIZE, N                                                                                   \
)                                                                                                  \
    m3c_memcpy(                                                                                    \
        (void *)((char *)(DST) + (DST_I) * (ELEM_SIZE)),       /* dst */                           \
        (const void *)((char *)(SRC) + (SRC_I) * (ELEM_SIZE)), /* src */                           \
        (N) * (ELEM_SIZE)                                                                          \
    )

/**
 * \brief Copies elements from `src` array to non-overlapping `dst` array.
 *
 * \param[out] dstBuf   pointer to the first element of the `dst` array
 * \param      dstLen   length of the `dst` array
 * \param      dstI     index of element in `dst` array in place of which the copying is performed
 * \param[in]  srcBuf   pointer to the first element of the `src` array
 * \param      srcLen   length of the `src` array
 * \param      srcI     index of the first element in `src` array to be copied
 * \param      elemSize size of the array element in bytes
 * \param      n        number of elements to be copied
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB
 */
M3C_ERROR M3C_ARR_Copy_impl(
    void *m3c_restrict dstBuf, m3c_size_t dstLen, m3c_size_t dstI,       /* dst */
    void const *m3c_restrict srcBuf, m3c_size_t srcLen, m3c_size_t srcI, /* src */
    m3c_size_t elemSize, m3c_size_t n
);

/**
 * \brief Copies elements within an array.
 *
 * \param[in,out] buf      pointer to the first element of the array
 * \param         len      length of the array
 * \param         elemSize size of the array element in bytes
 * \param         dstI     index of the element in place of which the copying is performed
 * \param         srcI     index of the first element to be copied
 * \param         n        number of elements to be copied
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB - if either `srcI + n` or `dstI + n` are out of bounds
 */
M3C_ERROR M3C_ARR_CopyWithin_impl(
    void *buf, m3c_size_t len, m3c_size_t elemSize, m3c_size_t dstI, m3c_size_t srcI, m3c_size_t n
);

/**
 * \brief Copies elements within an array.
 *
 * \warning To use this macro safely, ensure that:
 * 1. `DST_I < LEN` and `DST_I + N <= LEN`
 * 2. `SRC_I < LEN` and `SRC_I + N <= LEN`
 * where `LEN` is the length of the buffer `BUF`
 *
 * \param[in,out] BUF       pointer to the first element of the array
 * \param         ELEM_SIZE size of the array element in bytes
 * \param         DST_I     index of the element in place of which the copying is performed
 * \param         SRC_I     index of the first element to be copied
 * \param         N         number of elements to be copied
 *
 * \return result of the underlying call to `m3c_memmove`
 *
 * \sa #M3C_ARR_CopyWithin_impl, #M3C_ARR_COPY_WITHIN_UNSAFE
 */
#define M3C_ARR_CopyWithinUnsafe_impl(BUF, ELEM_SIZE, DST_I, SRC_I, N)                             \
    m3c_memmove(                                                                                   \
        (void *)((char *)(BUF) + (DST_I) * (ELEM_SIZE)),                                           \
        (const void *)((char *)(BUF) + (SRC_I) * (ELEM_SIZE)), (N) * (ELEM_SIZE)                   \
    )

/**
 * \brief Shifts elements of the array to the right.
 *
 * \warning To use this macro safely, ensure that:
 * 1. `START_I < LEN`
 * 2. `STEP < LEN - START_I`
 *
 * \param[in,out] BUF       pointer to the first element of the array
 * \param         LEN       length of the array
 * \param         ELEM_SIZE size of the array element in bytes
 * \param         START_I   index of the first element to be shifted
 * \param         STEP      step of shifting
 *
 * \return result of the underlying call to `m3c_memmove`
 */
#define M3C_ARR_RShiftUnsafe_impl(BUF, LEN, ELEM_SIZE, START_I, STEP)                              \
    M3C_ARR_CopyWithinUnsafe_impl(                                                                 \
        (BUF), (ELEM_SIZE), (START_I) + (STEP), (START_I), (LEN) - ((START_I) + (STEP))            \
    )

/**
 * \brief Shifts elements of the array to the right.
 *
 * \note If the operation is noop, then returns #M3C_ERROR_OK (e.g. if `step` is too big)
 *
 * \param[in,out] buf      pointer to the first element of the array
 * \param         len      length of the array
 * \param         elemSize size of the array element in bytes
 * \param         startI   index of the first element to be shifted
 * \param         step     step of shifting
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB - if `startI` is Out Of Bounds
 *
 * \sa #M3C_ARR_CopyWithin_impl
 */
M3C_ERROR M3C_ARR_RShift_impl(
    void *buf, m3c_size_t len, m3c_size_t elemSize, m3c_size_t startI, m3c_size_t step
);

/**
 * \brief Binary searches the array.
 *
 * \warning The array must be sorted with the order consistent with the sort order of `cmpFn`.
 *
 * \note If the array contains several elements equal to the given element (`ELEM`), the index of
 * any of them is returned.
 *
 * \param[in]  buf      pointer to the first element of the array
 * \param      len      length of the array
 * \param      elemSize size of the array element in bytes
 * \param[in]  elem     pointer to the element to be searched
 * \param[in]  cmpFn    pointer to \ref M3C_CMP_FN "comparator function"
 * \param[out] n        writes here index (of found element (#M3C_ERROR_OK) or of the outermost
 * element that is smaller than the given element (#M3C_ERROR_NOT_FOUND))
 * \param[in]  keyFn    pointer to \ref M3C_KEY_FN "key extraction function". If it's `NULL` the
 * \ref M3C_EchoFn "echo function" will be used
 * \param[in]  keyArg   argument to pass to every `keyFn` invocation as the second argument
 *
 * \return
 * + #M3C_ERROR_OK        - element is found (`n` contains the index)
 * + #M3C_ERROR_NOT_FOUND - element is not found (`n` holds the index at which the element can be
 * inserted so as not to break the sorting of the array)
 *
 * \sa #M3C_ARR_BSEARCH, #M3C_ARR_BSEARCH_BY_KEY
 */
M3C_ERROR M3C_ARR_BSearch_impl(
    void const *buf, m3c_size_t len, m3c_size_t elemSize, void const *elem, M3C_CMP_FN *cmpFn,
    m3c_size_t *n, M3C_KEY_FN *keyFn, void *keyArg
);

/**
 * \brief Deinits the array by freeing its underlying buffer, assuming the buffer has been allocated
 * in the heap.
 *
 * \param[in] ARR pointer to the array struct
 *
 * \warning Doesn't reset the length.
 */
#define M3C_ARR_DEINIT_BOXED(ARR) M3C_ARR_DeinitBoxed_impl((void *)(ARR)->data)

/**
 * \brief Copies elements from `SRC` array to non-overlapping `DST` array.
 *
 * \warning To use this macro safely, ensure that:
 * 1. `DST_I < DST_LEN` and `DST_I + N <= DST_LEN`
 * 2. `SRC_I < SRC_LEN` and `SRC_I + N <= SRC_LEN`
 * where `DST_LEN` and `SRC_LEN` are the lengths of arrays `DST` and `SRC`, respectively.
 *
 * \param         TYPE  type of array element
 * \param[in,out] DST   pointer to the array struct of `DST` array
 * \param         DST_I index of element in `DST` array in place of which the copying is performed
 * \param[in]     SRC   pointer to the array struct of `SRC` array
 * \param         SRC_I index of the first element in `SRC` array to be copied
 * \param         N     number of elements to be copied
 *
 * \return result of the underlying call to `m3c_memcpy`
 */
#define M3C_ARR_COPY_UNSAFE(TYPE, DST, DST_I, SRC, SRC_I, N)                                       \
    M3C_ARR_CopyUnsafe_impl((DST)->data, DST_I, (SRC)->data, SRC_I, sizeof(TYPE), N)

/**
 * \brief Copies elements from `SRC` array to non-overlapping `DST` array.
 *
 * \param         TYPE  type of array element
 * \param[in,out] DST   pointer to the array struct of `DST` array
 * \param         DST_I index of element in `DST` array in place of which the copying is performed
 * \param[in]     SRC   pointer to the array struct of `SRC` array
 * \param         SRC_I index of the first element in `SRC` array to be copied
 * \param         N     number of elements to be copied
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB
 */
#define M3C_ARR_COPY(TYPE, DST, DST_I, SRC, SRC_I, N)                                              \
    M3C_ARR_Copy_impl(                                                                             \
        (void *)(DST)->data, (DST)->len, (DST_I),       /* dst */                                  \
        (void const *)(SRC)->data, (SRC)->len, (SRC_I), /* src */                                  \
        sizeof(TYPE), (N)                                                                          \
    )

/**
 * \brief Copies elements within an array.
 *
 * \warning To use this macro safely, ensure that:
 * 1. `DST_I < LEN` and `DST_I + N <= LEN`
 * 2. `SRC_I < LEN` and `SRC_I + N <= LEN`
 * where `LEN` is the length of the array `ARR`
 *
 * \param         TYPE  type of array element
 * \param[in,out] ARR   pointer to the array struct
 * \param         DST_I index of the element in place of which the copying is performed
 * \param         SRC_I index of the first element to be copied
 * \param         N     number of elements to be copied
 *
 * \return result of the underlying call to `m3c_memmove`
 *
 * \sa #M3C_ARR_COPY_WITHIN, #M3C_ARR_CopyWithinUnsafe_impl
 */
#define M3C_ARR_COPY_WITHIN_UNSAFE(TYPE, ARR, DST_I, SRC_I, N)                                     \
    M3C_ARR_CopyWithinUnsafe_impl((ARR)->data, sizeof(TYPE), DST_I, SRC_I, N)

/**
 * \brief Shifts elements of the array to the right.
 *
 * \warning To use this macro safely, ensure that:
 * 1. `START_I < LEN`
 * 2. `STEP < LEN - START_I`
 * where `LEN` is the length of the array.
 *
 * \param         TYPE      type of array element
 * \param[in,out] ARR       pointer to the array struct
 * \param         START_I   index of the first element to be shifted
 * \param         STEP      step of shifting
 *
 * \return result of the underlying call to `m3c_memmove`
 */
#define M3C_ARR_RSHIFT_UNSAFE(TYPE, ARR, START_I, STEP)                                            \
    M3C_ARR_RShiftUnsafe_impl((ARR)->data, (ARR)->len, sizeof(TYPE), START_I, STEP)

/**
 * \brief Copies elements within an array.
 *
 * \param         TYPE  type of array element
 * \param[in,out] ARR   pointer to the array struct
 * \param         DST_I index of the element in place of which the copying is performed
 * \param         SRC_I index of the first element to be copied
 * \param         N     number of elements to be copied
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB - if any of `SRC_I + N` and `DST_I + N` are out of bounds
 */
#define M3C_ARR_COPY_WITHIN(TYPE, ARR, DST_I, SRC_I, N)                                            \
    M3C_ARR_CopyWithin_impl((void *)(ARR)->data, (ARR)->len, sizeof(TYPE), (DST_I), (SRC_I), (N))

/**
 * \brief Shifts elements of the array to the right.
 *
 * \note If the operation is noop, then returns #M3C_ERROR_OK (e.g. if `STEP` is too big)
 *
 * \param         TYPE    type of array element
 * \param[in,out] ARR     pointer to the array struct
 * \param         START_I index of the first element to be shifted
 * \param         STEP    step of shifting
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB - if `START_I` is Out Of Bounds
 *
 * \sa #M3C_ARR_COPY_WITHIN
 */
#define M3C_ARR_RSHIFT(TYPE, ARR, START_I, STEP)                                                   \
    M3C_ARR_RShift_impl((void *)(ARR)->data, (ARR)->len, sizeof(TYPE), (START_I), (STEP))

/**
 * \brief Inits the vector struct.
 *
 * \note Doesn't allocate the buffer.
 *
 * \param[in,out] VEC pointer to the vector struct
 */
#define M3C_VEC_INIT(VEC) M3C_VEC_Init_impl((void **)&(VEC)->data, &(VEC)->len, &(VEC)->cap)

/**
 * \brief Inits the vector and allocates the underlying buffer so that it can store exactly
 * `INIT_CAP` elements.
 *
 * \param         TYPE     type of vector element
 * \param[in,out] VEC      pointer to the vector struct
 * \param         INIT_CAP initial capacity
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to alloc
 */
#define M3C_VEC_NEW_WITH_CAP(TYPE, VEC, INIT_CAP)                                                  \
    M3C_VEC_NewWithCapacity_impl(                                                                  \
        (void **)&(VEC)->data, &(VEC)->len, &(VEC)->cap, sizeof(TYPE), (INIT_CAP)                  \
    )

/**
 * \brief Inits the vector and allocates the underlying buffer.
 *
 * \param         TYPE type of vector element
 * \param[in,out] VEC  pointer to the vector struct
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to alloc
 */
#define M3C_VEC_NEW(TYPE, VEC)                                                                     \
    M3C_VEC_New_impl((void **)&(VEC)->data, &(VEC)->len, &(VEC)->cap, sizeof(TYPE))

/**
 * \brief Deinits the vector, by freeing its underlying buffer.
 *
 * \param[in] VEC pointer to the vector struct
 *
 * \warning Doesn't reset the length and the capacity.
 */
#define M3C_VEC_DEINIT(VEC) M3C_ARR_DeinitBoxed_impl((void *)(VEC)->data)

/**
 * \brief Clears the vector.
 *
 * \details Deallocates underlying buffer and resets the capacity and the length to zero.
 *
 * \param[in,out] VEC pointer to the vector struct
 */
#define M3C_VEC_CLEAR(VEC) M3C_VEC_Clear_impl((void **)&(VEC)->data, &(VEC)->len, &(VEC)->cap)

/**
 * \brief Copies elements within a vector.
 *
 * \warning To use this macro safely, ensure that:
 * 1. `DST_I < LEN` and `DST_I + N <= LEN`
 * 2. `SRC_I < LEN` and `SRC_I + N <= LEN`
 * where `LEN` is the length of the vector `VEC`
 *
 * \param         TYPE  type of vector element
 * \param[in,out] VEC   pointer to the vector struct
 * \param         DST_I index of the element in place of which the copying is performed
 * \param         SRC_I index of the first element to be copied
 * \param         N     number of elements to be copied
 *
 * \return result of the underlying call to `m3c_memmove`
 *
 * \sa #M3C_VEC_COPY_WITHIN, #M3C_ARR_CopyWithinUnsafe_impl
 */
#define M3C_VEC_COPY_WITHIN_UNSAFE(TYPE, VEC, DST_I, SRC_I, N)                                     \
    M3C_ARR_COPY_WITHIN_UNSAFE(TYPE, VEC, DST_I, SRC_I, N)

/**
 * \brief Copies elements within a vector.
 *
 * \param         TYPE  type of vector element
 * \param[in,out] VEC   pointer to the vector struct
 * \param         DST_I index of the element in place of which the copying is performed
 * \param         SRC_I index of the first element to be copied
 * \param         N     number of elements to be copied
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB - if any of `SRC_I + N` and `DST_I + N` are out of bounds
 */
#define M3C_VEC_COPY_WITHIN(TYPE, VEC, DST_I, SRC_I, N)                                            \
    M3C_ARR_COPY_WITHIN(TYPE, VEC, DST_I, SRC_I, N)

/**
 * \brief Shifts elements of the vector to the right.
 *
 * \note If the operation is noop, then returns #M3C_ERROR_OK (e.g. if `STEP` is too big)
 *
 * \param         TYPE    type of vector element
 * \param[in,out] VEC     pointer to the vector struct
 * \param         START_I index of the first element to be shifted
 * \param         STEP    step of shifting
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB - if `START_I` is Out Of Bounds
 *
 * \sa #M3C_VEC_COPY_WITHIN
 */
#define M3C_VEC_RSHIFT(TYPE, VEC, START_I, STEP) M3C_ARR_RSHIFT(TYPE, VEC, START_I, STEP)

/**
 * \brief Inserts `ELEMS` to the vector at index `INDEX`.
 *
 * \param         TYPE  type of vector element
 * \param[in,out] VEC   pointer to the vector struct
 * \param         INDEX index before which the elements will be inserted
 * \param[in]     ELEMS pointer to the elements array to be inserted
 * \param         N     number of elements to insert
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOB - if `index` is Out Of Bounds
 * + #M3C_ERROR_OOM - if failed to realloc or the new capacity in bytes will be greater then
 * `M3C_SIZE_MAX`
 */
#define M3C_VEC_INSERT(TYPE, VEC, INDEX, ELEMS, N)                                                 \
    M3C_VEC_Insert_impl(                                                                           \
        (void **)&(VEC)->data, &(VEC)->len, &(VEC)->cap, sizeof(TYPE), (INDEX), (ELEMS), (N)       \
    )

/**
 * \brief Pushes `ELEMS` to the `VEC`.
 *
 * \details If it is necessary to reallocate the buffer, sets the new capacity to twice the previous
 * capacity using addition. Uses #m3c_realloc as an reallocator. It's a macro over function
 * #M3C_VEC_Push_impl.
 *
 * \param         TYPE  type of vector element
 * \param[in,out] VEC   pointer to the vector struct
 * \param[in]     ELEMS pointer to the elements to be pushed
 * \param         N     number of elements to be pushed
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to realloc or the new capacity in bytes will be greater then
 * `M3C_SIZE_MAX`
 */
#define M3C_VEC_PUSH_N(TYPE, VEC, ELEMS, N)                                                        \
    M3C_VEC_PushN_impl((void **)&(VEC)->data, &(VEC)->len, &(VEC)->cap, (ELEMS), (N), sizeof(TYPE))

/**
 * \brief Pushes `ELEM` to the `VEC`.
 *
 * \details If it is necessary to reallocate the buffer, sets the new capacity to twice the previous
 * capacity using addition. Uses #m3c_realloc as an reallocator. It's a macro over function
 * #M3C_VEC_Push_impl.
 *
 * \param         TYPE type of vector element
 * \param[in,out] VEC  pointer to the vector struct
 * \param[in]     ELEM pointer to the element to be pushed
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to realloc or the new capacity in bytes will be greater then
 * `M3C_SIZE_MAX`
 */
#define M3C_VEC_PUSH(TYPE, VEC, ELEM) M3C_VEC_PUSH_N(TYPE, VEC, ELEM, 1)

/**
 * \brief Increase the capacity of `VEC` to a value that's equal to `NEW_CAP`.
 *
 * \param         TYPE    type of vector element
 * \param[in,out] VEC     pointer to the vector struct
 * \param         NEW_CAP new capacity of the vector, in number of elements
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to realloc or `sizeof(TYPE) * NEW_CAP` will overflow
 */
#define M3C_VEC_RESERVE_EXACT(TYPE, VEC, NEW_CAP)                                                  \
    M3C_VEC_ReserveExact_impl((void **)&(VEC)->data, &(VEC)->cap, sizeof(TYPE), (NEW_CAP))

/**
 * \brief Increases the capacity of the vector so that it can hold at least `N` new elements.
 *
 * \param         TYPE    type of vector element
 * \param[in,out] VEC     pointer to the vector struct
 * \param         N       number of new elements
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to realloc or the new capacity in bytes will be greater then
 * `M3C_SIZE_MAX`
 */
#define M3C_VEC_RESERVE_UNUSED(TYPE, VEC, N)                                                       \
    M3C_VEC_ReserveUnused_impl((void **)&(VEC)->data, &(VEC)->len, &(VEC)->cap, sizeof(TYPE), (N))

/**
 * \brief Binary searches the array.
 *
 * \warning The array must be sorted with the order consistent with the sort order of `CMP_FN`.
 *
 * \note If the array contains several elements equal to the given element (`ELEM`), the index of
 * any of them is returned.
 *
 * \param      TYPE    type of array element
 * \param[in]  ARR     pointer to the array struct
 * \param[in]  ELEM    pointer to the element to be searched
 * \param[in]  CMP_FN  pointer to \ref M3C_CMP_FN "comparator function"
 * \param[out] N       writes here index (of found element (#M3C_ERROR_OK) or of the outermost
 * element that is smaller than the given element (#M3C_ERROR_NOT_FOUND))
 * \param[in]  KEY_FN  pointer to \ref M3C_KEY_FN "key extraction function". If it's `NULL` the \ref
 * #M3C_EchoFn "echo function" will be used
 * \param[in]  KEY_ARG argument to pass to every `KEY_FN` invocation as the second argument
 *
 * \return
 * + #M3C_ERROR_OK        - element is found (`N` contains the index)
 * + #M3C_ERROR_NOT_FOUND - element is not found (`N` holds the index at which the element can be
 * inserted so as not to break the sorting of the array)
 *
 * \sa M3C_ARR_BSEARCH
 */
#define M3C_ARR_BSEARCH_BY_KEY(TYPE, ARR, ELEM, CMP_FN, N, KEY_FN, KEY_ARG)                        \
    M3C_ARR_BSearch_impl(                                                                          \
        (void *)(ARR)->data, (ARR)->len, sizeof(TYPE), (ELEM), (M3C_CMP_FN *)(CMP_FN), (N),        \
        (M3C_KEY_FN *)(KEY_FN), (KEY_ARG)                                                          \
    )

/**
 * \brief Binary searches the array.
 *
 * \warning The array must be sorted with the order consistent with the sort order of `CMP_FN`.
 *
 * \note If the array contains several elements equal to the given element (`ELEM`), the index of
 * any of them is returned.
 *
 * \param      TYPE   type of array element
 * \param[in]  ARR    pointer to the array struct
 * \param[in]  ELEM   pointer to the element to be searched
 * \param[in]  CMP_FN pointer to \ref M3C_CMP_FN "comparator function"
 * \param[out] N      writes here index (of found element (#M3C_ERROR_OK) or of the outermost
 * element that is smaller than the given element (#M3C_ERROR_NOT_FOUND))
 *
 * \return
 * + #M3C_ERROR_OK        - element is found (`N` contains the index)
 * + #M3C_ERROR_NOT_FOUND - element is not found (`N` holds the index at which the element can be
 * inserted so as not to break the sorting of the array)
 *
 * \sa M3C_ARR_BSEARCH_BY_KEY
 */
#define M3C_ARR_BSEARCH(TYPE, ARR, ELEM, CMP_FN, N)                                                \
    M3C_ARR_BSEARCH_BY_KEY(TYPE, ARR, ELEM, CMP_FN, N, M3C_NULL, M3C_NULL)

#endif /* _M3C_INCGUARD_COLTYPES_H */

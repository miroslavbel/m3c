#ifndef _M3C_INCGUARD_COLTYPES_H
#define _M3C_INCGUARD_COLTYPES_H

#include <m3c/common/types.h>
#include <m3c/common/errors.h>

/**
 * \brief Macro for defining a vector structure with a given type.
 *
 * \warning Only single-word types are supported.
 */
#define M3C_VEC(TYPE)                                                                              \
    struct __tagM3C_VEC_##TYPE {                                                                   \
        m3c_size_t len;                                                                            \
        m3c_size_t cap;                                                                            \
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
 * \brief Pushes `elem` to the vector specified through `len` and `cap`.
 *
 * \details If it is necessary to reallocate the buffer, sets the new capacity to twice the previous
 * capacity using addition. Uses #m3c_realloc as an reallocator.
 *
 * \param[in,out] buf      pointer to the pointer to the buffer
 * \param[in,out] len      pointer to the buffer length
 * \param[in,out] cap      pointer to the buffer capacity
 * \param[in]     elem     pointer to the element to be pushed
 * \param         elemSize size of element in bytes
 * \return
 * + M3C_ERROR_OK
 * + M3C_ERROR_OOM - if failed to realloc
 */
M3C_ERROR
M3C_VEC_Push_impl(
    void **buf, m3c_size_t *len, m3c_size_t *cap, void const *elem, m3c_size_t elemSize
);

/**
 * \brief Increase the capacity of the vector to a value that's greater or equal to `newCap`.
 *
 * \param[in,out] buf      pointer to the pointer to the buffer
 * \param[in,out] cap      pointer to the buffer capacity
 * \param         elemSize size of the vector element in bytes
 * \param         newCap   new capacity of the buffer, in number of elements
 * \return
 * + M3C_ERROR_OK
 * + M3C_ERROR_OOM - if failed to realloc
 */
M3C_ERROR
M3C_VEC_Reserve_impl(void **buf, m3c_size_t *cap, m3c_size_t elemSize, m3c_size_t newCap);

/**
 * \brief Binary searches the array.
 *
 * \details The comparator function (`cmp`) accepts two pointers to elements `x` and `y` and should
 * return:
 * + `0`   - if elements are equal
 * + `> 0` - if `x` is greater than `y`
 * + `< 0` - if `x` is less than `y`
 *
 * \warning The array must be sorted with the order consistent with the sort order of `CMP`.
 *
 * \note If the array contains several elements equal to the given element (`ELEM`), the index of
 * any of them is returned.
 *
 * \param[in]  buf      pointer to the first element of the array
 * \param      len      length of the array
 * \param      elemSize size of the array element in bytes
 * \param[in]  elem     pointer to the element to be searched
 * \param[in]  cmp      comparator function
 * \param[out] n        writes here index (of found element (#M3C_ERROR_OK) or of the outermost
 * element that is smaller than the given element (#M3C_ERROR_NOT_FOUND))
 *
 * \return
 * + #M3C_ERROR_OK        - element is found (`n` contains the index)
 * + #M3C_ERROR_NOT_FOUND - element is not found (`n` holds the index at which the element can be
 * inserted so as not to break the sorting of the array)
 */
M3C_ERROR M3C_ARR_BSearch_impl(
    void const *buf, m3c_size_t len, m3c_size_t elemSize, void const *elem,
    int (*cmp)(void const *, void const *), m3c_size_t *n
);

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
 * + M3C_ERROR_OK
 * + M3C_ERROR_OOM - if failed to realloc
 */
#define M3C_VEC_PUSH(TYPE, VEC, ELEM)                                                              \
    M3C_VEC_Push_impl((void **)&(VEC)->data, &(VEC)->len, &(VEC)->cap, ELEM, sizeof(TYPE))

/**
 * \brief Increase the capacity of `VEC` to a value that's greater or equal to `NEW_CAP`.
 *
 * \param         TYPE    type of vector element
 * \param[in,out] VEC     pointer to the vector struct
 * \param         NEW_CAP new capacity of the vector, in number of elements
 * \return
 * + M3C_ERROR_OK
 * + M3C_ERROR_OOM - if failed to realloc
 */
#define M3C_VEC_RESERVE(TYPE, VEC, NEW_CAP)                                                        \
    M3C_VEC_Reserve_impl((void **)&(VEC)->data, &(VEC)->cap, sizeof(TYPE), (NEW_CAP))

/**
 * \brief Binary searches the array.
 *
 * \details The comparator function (`CMP`) accepts two pointers to elements of type `TYPE` `x` and
 * `y` and should return:
 * + `0`   - if elements are equal
 * + `> 0` - if `x` is greater than `y`
 * + `< 0` - if `x` is less than `y`
 *
 * \warning The array must be sorted with the order consistent with the sort order of `CMP`.
 *
 * \note If the array contains several elements equal to the given element (`ELEM`), the index of
 * any of them is returned.
 *
 * \param      TYPE type of array element
 * \param[in]  ARR  pointer to the array struct
 * \param[in]  ELEM pointer to the element to be searched
 * \param[in]  CMP  comparator function
 * \param[out] N    writes here index (of found element (#M3C_ERROR_OK) or of the outermost element
 * that is smaller than the given element (#M3C_ERROR_NOT_FOUND))
 *
 * \return
 * + #M3C_ERROR_OK        - element is found (`N` contains the index)
 * + #M3C_ERROR_NOT_FOUND - element is not found (`N` holds the index at which the element can be
 * inserted so as not to break the sorting of the array)
 */
#define M3C_ARR_BSEARCH(TYPE, ARR, ELEM, CMP, N)                                                   \
    M3C_ARR_BSearch_impl(                                                                          \
        (void *)(ARR)->data, (ARR)->len, sizeof(TYPE), (ELEM),                                     \
        (int (*)(const void *, const void *))(CMP), (N)                                            \
    )

#endif /* _M3C_INCGUARD_COLTYPES_H */

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

#endif /* _M3C_INCGUARD_COLTYPES_H */

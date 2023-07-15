#ifndef _M3C_INCGUARD_COLTYPES_H
#define _M3C_INCGUARD_COLTYPES_H

#include <m3c/common/types.h>

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

#endif /* _M3C_INCGUARD_COLTYPES_H */

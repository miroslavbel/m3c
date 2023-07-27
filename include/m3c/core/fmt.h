#ifndef _M3C_INCGUARD_CORE_FMT_H
#define _M3C_INCGUARD_CORE_FMT_H

#include <m3c/common/types.h>

/**
 * \brief ASCII buffer, where the first byte is an unsigned number indicating the remaining (usable)
 * length of the buffer.
 *
 * \note The usable length (value of the first byte) can be up to 255.
 */
#define M3C_LU8_ASCII m3c_u8 *

/**
 * \brief The kind of argument.
 */
typedef enum __tagM3C_FmtArgKind {
    /**
     * \brief Represents #M3C_LU8_ASCII.
     */
    M3C_FmtArgKind_LU8_ASCII
} M3C_FmtArgKind;

/**
 * \brief The union of all possible argument values.
 */
typedef union __tagM3C_FmtArgValue {
    /**
     * \brief Value of type #M3C_LU8_ASCII.
     */
    const M3C_LU8_ASCII LU8_ASCII;
} M3C_FmtArgValue;

/**
 * \brief Argument.
 *
 * \details Contains both the kind and the value, so that it can be processed in runtime.
 */
typedef struct __tagM3C_FmtArg {
    /**
     * \brief Kind of this argument.
     */
    M3C_FmtArgKind kind;
    /**
     * \brief Value of this argument.
     */
    M3C_FmtArgValue val;
} M3C_FmtArg;

/**
 * \brief A collection of arguments with length.
 *
 * \details Used by diagnostics to display a message.
 */
typedef struct __tagM3C_FmtArgs {
    /**
     * \brief Length of #data.
     */
    m3c_u8 len;
    /**
     * \brief Buffer of arguments.
     */
    M3C_FmtArg *data;
} M3C_FmtArgs;

#endif /* _M3C_INCGUARD_CORE_FMT_H */

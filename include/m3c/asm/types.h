#ifndef _M3C_INCGUARD_ASM_TYPES_H
#define _M3C_INCGUARD_ASM_TYPES_H

#include <m3c/common/types.h>

/**
 * \brief Position.
 */
typedef struct __tagM3C_ASM_Position {
    /**
     * \brief Zero-based index of the line.
     */
    m3c_u16 line;
    /**
     * \brief Zero-based index of the character in the #line.
     */
    m3c_u16 character;
} M3C_ASM_Position;

#endif /* _M3C_INCGUARD_ASM_TYPES_H */

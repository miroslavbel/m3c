#ifndef _M3C_INCGUARD_ASM_DIAGNOSTICS_H
#define _M3C_INCGUARD_ASM_DIAGNOSTICS_H

#include <m3c/common/types.h>

/**
 * \brief Position with byte index.
 */
typedef struct __tagM3C_ASM_BPosition {
    /**
     * \brief Zero-based index of the line.
     */
    m3c_u32 line;
    /**
     * \brief Zero-based index of the character in the #line.
     */
    m3c_u32 character;
    /**
     * \brief Zero-based index of the byte.
     */
    m3c_u32 byte;
} M3C_ASM_BPosition;

/**
 * \brief "Instance" data of \ref M3C_DiagnosticsDomain_ASM "ASM" diagnostics.
 */
typedef struct __tagM3C_ASM_DiagnosticsData {
    /**
     * \brief Start position.
     */
    M3C_ASM_BPosition start;
    /**
     * \brief Length in bytes.
     */
    m3c_u32 len;
    /**
     * \brief Handle of include information.
     */
    m3c_u32 hInclude;
} M3C_ASM_DiagnosticsData;

#endif /* _M3C_INCGUARD_ASM_DIAGNOSTICS_H */

#ifndef _M3C_INCGUARD_ASM_DIAGNOSTICS_H
#define _M3C_INCGUARD_ASM_DIAGNOSTICS_H

#include <m3c/common/types.h>
#include <m3c/asm/types.h>

/**
 * \brief Enumeration of all \ref M3C_DIAGNOSTIC_DOMAIN_ASM "ASM" domain diagnostic ids.
 */
typedef enum __tagM3C_ASM_DiagnosticId {
    /**
     * \brief Invalid byte sequence.
     *
     * \warning Diagnostics with this id point to source code containing invalid byte sequence.
     */
    M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING,
    /**
     * \brief Unrecognized token.
     *
     * \note If the source code pointed to by the diagnostic contains invalid byte sequence,
     * \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING" diagnostic(s) are generated.
     *
     * \warning Diagnostics with this id may point to source code containing non-ASCII characters
     * and invalid byte sequence.
     */
    M3C_ASM_DIAGNOSTIC_ID_UNRECOGNIZED_TOKEN
} M3C_ASM_DiagnosticId;

/**
 * \brief "Instance" data of \ref M3C_DIAGNOSTIC_DOMAIN_ASM "ASM" diagnostics.
 */
typedef struct __tagM3C_ASM_DiagnosticsData {
    /**
     * \brief Start position.
     */
    M3C_ASM_Position start;
    /**
     * \brief End position (exclusive).
     */
    M3C_ASM_Position end;
    /**
     * \brief Handle of include information.
     */
    M3C_hINCLUDE hInclude;
} M3C_ASM_DiagnosticsData;

#endif /* _M3C_INCGUARD_ASM_DIAGNOSTICS_H */

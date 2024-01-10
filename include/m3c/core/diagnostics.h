#ifndef _M3C_INCGUARD_CORE_DIAGNOSTICS_H
#define _M3C_INCGUARD_CORE_DIAGNOSTICS_H

#include <m3c/core/fmt.h>
#include <m3c/asm/diagnostics.h>

/**
 * \brief Diagnostics domain.
 */
typedef enum __tagM3C_DiagnosticsDomain {
    /**
     * \brief For diagnostics related to assembling assembly into native format.
     *
     * \see M3C_ASM_DiagnosticId.
     */
    M3C_DIAGNOSTIC_DOMAIN_ASM = 1
} M3C_DiagnosticsDomain;

/**
 * \brief Diagnostics severity.
 */
typedef enum __tagM3C_Severity {
    M3C_SEVERITY_NOTE = 0,
    M3C_SEVERITY_WARNING = 1,
    M3C_SEVERITY_ERROR = 2,
    M3C_SEVERITY_FATAL_ERROR = 3
} M3C_Severity;

/**
 * \brief Union of the diagnostic Ids of all domains.
 */
typedef union __tagM3C_DiagnosticsId {
    /**
     * \brief Ids of \ref M3C_DIAGNOSTIC_DOMAIN_ASM "ASM" diagnostics.
     */
    M3C_ASM_DiagnosticId ASM;
} M3C_DiagnosticsId;

/**
 * \brief Represents common immutable data for all diagnostics with the same \ref
 * M3C_DiagnosticsInfo::domain "domain" and \ref M3C_DiagnosticsInfo::id "id" (and contains them
 * both).
 */
typedef struct __tagM3C_DiagnosticsInfo {
    /**
     * \brief Domain.
     */
    M3C_DiagnosticsDomain domain;
    /**
     * \brief Id.
     */
    M3C_DiagnosticsId id;
    /**
     * \brief Minimal possible severity.
     */
    M3C_Severity minSeverity;
    /**
     * \brief Args for message formatting.
     */
    M3C_FmtArgs args;
} M3C_DiagnosticsInfo;

/**
 * \brief Union of "instance" data from different domains of diagnostics.
 *
 * \details Data that changes from one instance with the same \ref M3C_DiagnosticsDomain "domain"
 * and id to another. For example, position, severity, etc.
 */
typedef union __tagM3C_DiagnosticsData {
    /**
     * \brief Represents "instance" data of \ref M3C_DIAGNOSTIC_DOMAIN_ASM "ASM" diagnostics.
     */
    M3C_ASM_DiagnosticsData ASM;
} M3C_DiagnosticsData;

/**
 * \brief Diagnostic.
 */
typedef struct __tagM3C_Diagnostic {
    /**
     * \brief Diagnostic "instance" data.
     *
     * \details Data that changes from one instance with the same \ref M3C_DiagnosticsDomain
     * "domain" and id to another. For example, position, severity, etc.
     */
    M3C_DiagnosticsData data;
    /**
     * \brief General info about this diagnostic.
     *
     * \details Contains diagnostic \ref M3C_DiagnosticsDomain "domain" and id and some common (and
     * static) info for diagnostics of this domain and id.
     */
    M3C_DiagnosticsInfo const *info;
    /**
     * \brief Severity of this diagnostic.
     */
    M3C_Severity severity;
} M3C_Diagnostic;

typedef M3C_VEC(M3C_Diagnostic) M3C_DiagnosticVec;

/**
 * \brief Diagnostics vector with \ref M3C_Diagnostics::warnings "warning" and \ref
 * M3C_Diagnostics::errors "error" counters.
 */
typedef struct __tagM3C_Diagnostics {
    /**
     * \brief Diagnostics vector.
     */
    M3C_DiagnosticVec vec;
    /**
     * \brief Number of \ref M3C_SEVERITY_WARNING "warnings".
     */
    m3c_u32 warnings;
    /**
     * \brief Number of errors (both \ref M3C_SEVERITY_ERROR "error" and \ref
     * M3C_SEVERITY_FATAL_ERROR "fatal error").
     */
    m3c_u32 errors;
} M3C_Diagnostics;

/**
 * \brief Inits #M3C_Diagnostics.
 *
 * \param[in,out] diagnostics diagnostics
 */
void __M3C_Diagnostics_Init(M3C_Diagnostics *diagnostics);

/**
 * \brief Deinits #M3C_Diagnostics.
 *
 * \param[in] diagnostics diagnostics
 */
void __M3C_Diagnostics_Deinit(M3C_Diagnostics const *diagnostics);

#endif /* _M3C_INCGUARD_CORE_DIAGNOSTICS_H */

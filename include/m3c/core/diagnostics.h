#ifndef _M3C_INCGUARD_CORE_DIAGNOSTICS_H
#define _M3C_INCGUARD_CORE_DIAGNOSTICS_H

#include <m3c/asm/diagnostics.h>

/**
 * \brief Diagnostics domain.
 */
typedef enum __tagM3C_DiagnosticsDomain {
    /**
     * \brief For diagnostics related to assembling assembly into native format.
     */
    M3C_DiagnosticsDomain_ASM = 1
} M3C_DiagnosticsDomain;

/**
 * \brief Diagnostics severity.
 */
typedef enum __tagM3C_Severity {
    M3C_Severity_NOTE = 0,
    M3C_Severity_WARNING = 1,
    M3C_Severity_ERROR = 2,
    M3C_Severity_FATAL_ERROR = 3
} M3C_Severity;

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
    m3c_u32 id;
    /**
     * \brief Minimal possible severity.
     */
    M3C_Severity minSeverity;
} M3C_DiagnosticsInfo;

/**
 * \brief Union of "instance" data from different domains of diagnostics.
 *
 * \details Data that changes from one instance with the same \ref M3C_DiagnosticsDomain "domain"
 * and id to another. For example, position, severity, etc.
 */
typedef union __tagM3C_DiagnosticsData {
    /**
     * \brief Represents "instance" data of \ref M3C_DiagnosticsDomain_ASM "ASM" diagnostics.
     */
    M3C_ASM_DiagnosticsData ASM;
} M3C_DiagnosticsData;

/**
 * \brief Diagnostic.
 */
typedef struct __tagM3C_Diagnostic {
    /**
     * \brief General info about this diagnostic.
     *
     * \details Contains diagnostic \ref M3C_DiagnosticsDomain "domain" and id and some common (and
     * static) info for diagnostics of this domain and id.
     */
    M3C_DiagnosticsInfo *info;
    /**
     * \brief Severity of this diagnostic.
     */
    M3C_Severity severity;
    /**
     * \brief Diagnostic "instance" data.
     *
     * \details Data that changes from one instance with the same \ref M3C_DiagnosticsDomain
     * "domain" and id to another. For example, position, severity, etc.
     */
    M3C_DiagnosticsData data;
} M3C_Diagnostic;

#endif /* _M3C_INCGUARD_CORE_DIAGNOSTICS_H */

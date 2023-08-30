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
     * \details #M3C_ASM_lex emits this diagnostic when encounters an invalid byte sequence.
     *
     * \warning Diagnostics with this id point to source code containing invalid byte sequence.
     */
    M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING,
    /**
     * \brief Unrecognized token.
     *
     * \details #M3C_ASM_lex emits this diagnostic when encounters an \ref
     * M3C_ASM_TOKEN_KIND_UNRECOGNIZED "UNRECOGNIZED" token.
     *
     * \warning Diagnostics with this id may point to source code containing non-ASCII characters
     * and invalid byte sequence.
     *
     * \note If the source code pointed to by the diagnostic contains invalid byte sequence,
     * \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING" diagnostic(s) are generated.
     */
    M3C_ASM_DIAGNOSTIC_ID_UNRECOGNIZED_TOKEN,
    /**
     * \brief Leading zeros are not permitted.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_NUMBER "NUMBER" token that has leading zeros (like `09`). Use an `0o` base
     * prefix for octal numbers.
     *
     * \note Leading zeros after the base prefix are permitted.
     */
    M3C_ASM_DIAGNOSTIC_ID_LEADING_ZEROS_ARE_NOT_PERMITTED,
    /**
     * \brief Invalid base prefix.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_NUMBER "NUMBER" token and the base prefix is invalid. Valid base prefixes
     * are:
     * + `[bByY]` - for binary
     * + `[oOqQ]` - for octal
     * + `[dD]` - for decimal (optional, but see \ref
     * M3C_ASM_DIAGNOSTIC_ID_LEADING_ZEROS_ARE_NOT_PERMITTED "LEADING_ZEROS_ARE_NOT_PERMITTED")
     * + `[xXhH]` - for hexadecimal
     */
    M3C_ASM_DIAGNOSTIC_ID_INVALID_BASE_PREFIX,
    /**
     * \brief Digit separator cannot appear here.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_NUMBER "NUMBER" token and the base prefix is followed by a digit
     * separator.
     */
    M3C_ASM_DIAGNOSTIC_ID_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE,
    /**
     * \brief Number literal must contain at least one digit.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_NUMBER "NUMBER" token and it has no digits after the base prefix.
     *
     * See also \ref
     * M3C_ASM_DIAGNOSTIC_ID_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE "DIGIT_SEPARATOR_CANNOT_APPEAR_HERE"
     */
    M3C_ASM_DIAGNOSTIC_ID_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT,
    /**
     * \brief Invalid digit for this base prefix.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_NUMBER "NUMBER" token and a digit which is not allowed for the current
     * base prefix is encountered. The valid digits for each base are:
     * + `[01]` - for binary
     * + `[0-7]` - for octal
     * + `[0-9]` - for decimal
     * + `[0-9A-Fa-F]` - for hexadecimal
     */
    M3C_ASM_DIAGNOSTIC_ID_INVALID_DIGIT_FOR_THIS_BASE_PREFIX,
    /**
     * \brief Invalid character(s) in string literal.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_STRING "STRING" token and a sequence of invalid characters is encountered.
     * Only `[0-9A-Za-z]` are valid characters for string literal.
     *
     * \warning Diagnostics with this id may point to source code containing non-ASCII characters
     * and invalid byte sequence.
     *
     * \note If the source code pointed to by the diagnostic contains invalid byte sequence,
     * \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING" diagnostic(s) are generated.
     */
    M3C_ASM_DIAGNOSTIC_ID_INVALID_CHARACTERS_IN_STRING_LITERAL,
    /**
     * \brief Unterminated string literal.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_STRING "STRING" token and this token has no closing QUOTATION MARK (`"`).
     *
     * \warning Diagnostics with this id may point to source code containing non-ASCII characters
     * and invalid byte sequence.
     *
     * \note If the source code pointed to by the diagnostic contains invalid byte sequence,
     * \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING" diagnostic(s) are generated.
     */
    M3C_ASM_DIAGNOSTIC_ID_UNTERMINATED_STRING_LITERAL
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

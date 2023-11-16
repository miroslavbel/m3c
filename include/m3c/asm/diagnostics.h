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
    M3C_ASM_DIAGNOSTIC_ID_UNTERMINATED_STRING_LITERAL,
    /**
     * \brief Unknown escape sequence.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_STRING "STRING" token and an unknown escape sequence is encountered.
     *
     * Known escape sequences:
     * + `\xN`, where N is one or two hexadecimal digits - arbitrary number of hexadecimal digits
     * + `\'` - byte 0x27
     * + `\"` - byte 0x22
     * + `\?` - byte 0x3f
     * + `\\` - byte 0x5c
     * + `\a` - byte 0x07
     * + `\b` - byte 0x08
     * + `\f` - byte 0x0c
     * + `\n` - byte 0x0a
     * + `\r` - byte 0x0d
     * + `\t` - byte 0x09
     * + `\v` - byte 0x0b
     */
    M3C_ASM_DIAGNOSTIC_ID_UNKNOWN_ESCAPE_SEQUENCE,
    /**
     * \brief `\x` used with no following hex digits.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_STRING "STRING" token and `\x` is used without following hexadecimal
     * digits. See #M3C_ASM_DIAGNOSTIC_ID_UNKNOWN_ESCAPE_SEQUENCE for all escape sequences.
     */
    M3C_ASM_DIAGNOSTIC_ID_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS,
    /**
     * \brief Number constant is too large.
     *
     * \details #M3C_ASM_lex emits this diagnostic when trying to lex a \ref
     * M3C_ASM_TOKEN_KIND_NUMBER "NUMBER" token and the number this token represents is too large
     * for the compiler to store it.
     *
     * \note In the current implementation, the compiler uses `i32` to store a number value.
     * \note The compiler sets the token kind of the token causing this diagnostic to \ref
     * M3C_ASM_TOKEN_KIND_UNRECOGNIZED "UNRECOGNIZED"
     */
    M3C_ASM_DIAGNOSTIC_ID_NUMBER_CONSTANT_IS_TOO_LARGE
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

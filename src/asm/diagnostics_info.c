#include <m3c/asm/diagnostics_info.h>

m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_INVALID_ENCODING[22] =
    "\x15"
    "invalid byte sequence";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_UNRECOGNIZED_TOKEN[19] =
    "\x12"
    "unrecognized token";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_LEADING_ZEROS_ARE_NOT_PERMITTED[32] =
    "\x1F"
    "leading zeros are not permitted";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_INVALID_BASE_PREFIX[20] =
    "\x13"
    "invalid base prefix";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE[35] =
    "\x22"
    "digit separator cannot appear here";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT[47] =
    "\x2E"
    "number literal must contain at least one digit";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_INVALID_DIGIT_FOR_THIS_BASE_PREFIX[35] =
    "\x22"
    "invalid digit for this base prefix";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_INVALID_CHARACTERS_IN_STRING_LITERAL[39] =
    "\x26"
    "invalid character(s) in string literal";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_UNTERMINATED_STRING_LITERAL[28] =
    "\x1B"
    "unterminated string literal";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_UNKNOWN_ESCAPE_SEQUENCE[24] =
    "\x17"
    "unknown escape sequence";
m3c_u8 const __M3C_ASM_STR_DIAGNOSTIC_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS[37] =
    "\x24"
    "\\x used with no following hex digits";

#define __M3C_ASM_DIAGNOSTIC_INFO_INVALID_ENCODING_ARGC 1
M3C_FmtArg __M3C_ASM_FMT_ARGS_INVALID_ENCODING[__M3C_ASM_DIAGNOSTIC_INFO_INVALID_ENCODING_ARGC] = {
    {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_INVALID_ENCODING}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_INVALID_ENCODING = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_INVALID_ENCODING_ARGC, __M3C_ASM_FMT_ARGS_INVALID_ENCODING},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_UNRECOGNIZED_TOKEN_ARGC 1
M3C_FmtArg
    __M3C_ASM_FMT_ARGS_UNRECOGNIZED_TOKEN[__M3C_ASM_DIAGNOSTIC_INFO_UNRECOGNIZED_TOKEN_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_UNRECOGNIZED_TOKEN}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_UNRECOGNIZED_TOKEN = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_UNRECOGNIZED_TOKEN,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_UNRECOGNIZED_TOKEN_ARGC, __M3C_ASM_FMT_ARGS_UNRECOGNIZED_TOKEN},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_LEADING_ZEROS_ARE_NOT_PERMITTED_ARGC 1
M3C_FmtArg __M3C_ASM_FMT_ARGS_LEADING_ZEROS_ARE_NOT_PERMITTED
    [__M3C_ASM_DIAGNOSTIC_INFO_LEADING_ZEROS_ARE_NOT_PERMITTED_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_LEADING_ZEROS_ARE_NOT_PERMITTED}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_LEADING_ZEROS_ARE_NOT_PERMITTED = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_LEADING_ZEROS_ARE_NOT_PERMITTED,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_LEADING_ZEROS_ARE_NOT_PERMITTED_ARGC,
      __M3C_ASM_FMT_ARGS_LEADING_ZEROS_ARE_NOT_PERMITTED},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_INVALID_BASE_PREFIX_ARGC 1
M3C_FmtArg
    __M3C_ASM_FMT_ARGS_INVALID_BASE_PREFIX[__M3C_ASM_DIAGNOSTIC_INFO_INVALID_BASE_PREFIX_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_INVALID_BASE_PREFIX}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_INVALID_BASE_PREFIX = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_INVALID_BASE_PREFIX,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_INVALID_BASE_PREFIX_ARGC, __M3C_ASM_FMT_ARGS_INVALID_BASE_PREFIX},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE_ARGC 1
M3C_FmtArg __M3C_ASM_FMT_ARGS_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE
    [__M3C_ASM_DIAGNOSTIC_INFO_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE_ARGC,
      __M3C_ASM_FMT_ARGS_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT_ARGC 1
M3C_FmtArg __M3C_ASM_FMT_ARGS_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT
    [__M3C_ASM_DIAGNOSTIC_INFO_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII,
         __M3C_ASM_STR_DIAGNOSTIC_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT_ARGC,
      __M3C_ASM_FMT_ARGS_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_INVALID_DIGIT_FOR_THIS_BASE_PREFIX_ARGC 1
M3C_FmtArg __M3C_ASM_FMT_ARGS_INVALID_DIGIT_FOR_THIS_BASE_PREFIX
    [__M3C_ASM_DIAGNOSTIC_INFO_INVALID_DIGIT_FOR_THIS_BASE_PREFIX_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_INVALID_DIGIT_FOR_THIS_BASE_PREFIX}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_INVALID_DIGIT_FOR_THIS_BASE_PREFIX = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_INVALID_DIGIT_FOR_THIS_BASE_PREFIX,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_INVALID_DIGIT_FOR_THIS_BASE_PREFIX_ARGC,
      __M3C_ASM_FMT_ARGS_INVALID_DIGIT_FOR_THIS_BASE_PREFIX},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_INVALID_CHARACTERS_IN_STRING_LITERAL_ARGC 1
M3C_FmtArg __M3C_ASM_FMT_ARGS_INVALID_CHARACTERS_IN_STRING_LITERAL
    [__M3C_ASM_DIAGNOSTIC_INFO_INVALID_CHARACTERS_IN_STRING_LITERAL_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_INVALID_CHARACTERS_IN_STRING_LITERAL}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_INVALID_CHARACTERS_IN_STRING_LITERAL = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_INVALID_CHARACTERS_IN_STRING_LITERAL,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_INVALID_CHARACTERS_IN_STRING_LITERAL_ARGC,
      __M3C_ASM_FMT_ARGS_INVALID_CHARACTERS_IN_STRING_LITERAL},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_UNTERMINATED_STRING_LITERAL_ARGC 1
M3C_FmtArg __M3C_ASM_FMT_ARGS_UNTERMINATED_STRING_LITERAL
    [__M3C_ASM_DIAGNOSTIC_INFO_UNTERMINATED_STRING_LITERAL_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_UNTERMINATED_STRING_LITERAL}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_UNTERMINATED_STRING_LITERAL = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_UNTERMINATED_STRING_LITERAL,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_UNTERMINATED_STRING_LITERAL_ARGC,
      __M3C_ASM_FMT_ARGS_UNTERMINATED_STRING_LITERAL},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_UNKNOWN_ESCAPE_SEQUENCE_ARGC 1
M3C_FmtArg __M3C_ASM_FMT_ARGS_UNKNOWN_ESCAPE_SEQUENCE
    [__M3C_ASM_DIAGNOSTIC_INFO_UNKNOWN_ESCAPE_SEQUENCE_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_UNKNOWN_ESCAPE_SEQUENCE}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_UNKNOWN_ESCAPE_SEQUENCE = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_UNKNOWN_ESCAPE_SEQUENCE,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_UNKNOWN_ESCAPE_SEQUENCE_ARGC,
      __M3C_ASM_FMT_ARGS_UNKNOWN_ESCAPE_SEQUENCE},
};

#define __M3C_ASM_DIAGNOSTIC_INFO_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS_ARGC 1
M3C_FmtArg __M3C_ASM_FMT_ARGS_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS
    [__M3C_ASM_DIAGNOSTIC_INFO_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS_ARGC] = {
        {M3C_FmtArgKind_LU8_ASCII, __M3C_ASM_STR_DIAGNOSTIC_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS}
};
const M3C_DiagnosticsInfo M3C_ASM_DIAGNOSTIC_INFO_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS = {
    M3C_DIAGNOSTIC_DOMAIN_ASM,
    M3C_ASM_DIAGNOSTIC_ID_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS,
    M3C_SEVERITY_ERROR,
    {__M3C_ASM_DIAGNOSTIC_INFO_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS_ARGC,
      __M3C_ASM_FMT_ARGS_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS},
};

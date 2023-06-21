#ifndef _M3C_INCGUARD_ASM_ERRORS_H
#define _M3C_INCGUARD_ASM_ERRORS_H

#include <m3c/common/encodings.h>

typedef enum tagM3C_ASM_Error {
    /**
     * \brief No error.
     */
    M3C_ASM_ERROR_OK = 0,

    /**
     * \brief Codepoint start byte is invalid.
     *
     * \note Must be equal to #M3C_UTF8_ERROR_ILLEGAL_START_BYTE. See it for more details.
     */
    M3C_ASM_ERROR_UTF8_ILLEGAL_START_BYTE = M3C_UTF8_ERROR_ILLEGAL_START_BYTE, /* 1 */
    /**
     * \brief Buffer ends earlier than the given codepoint.
     *
     * \note Must be equal to #M3C_UTF8_ERROR_UNEXPECTED_EOF. See it for more details.
     */
    M3C_ASM_ERROR_UTF8_UNEXPECTED_EOF = M3C_UTF8_ERROR_UNEXPECTED_EOF, /* 2 */
    /**
     * \brief Codepoint contains at least one invalid continuation byte.
     *
     * \note Must be equal to #M3C_UTF8_ERROR_ILLEGAL_CONTINUATION_BYTE. See it for more details.
     */
    M3C_ASM_ERROR_UTF8_ILLEGAL_CONTINUATION_BYTE = M3C_UTF8_ERROR_ILLEGAL_CONTINUATION_BYTE, /* 3 */
    /**
     * \brief Overlong encoding detected.
     *
     * \note Must be equal to #M3C_UTF8_ERROR_OVERLONG_ENCODING. See it for more details.
     */
    M3C_ASM_ERROR_UTF8_OVERLONG_ENCODING = M3C_UTF8_ERROR_OVERLONG_ENCODING, /* 4 */
    /**
     * \brief Codepoint encodes a value larger than the Unicode standard allows.
     *
     * \note Must be equal to #M3C_UTF8_ERROR_CODEPOINT_TOO_BIG. See it for more details.
     */
    M3C_ASM_ERROR_UTF8_CODEPOINT_TOO_BIG = M3C_UTF8_ERROR_CODEPOINT_TOO_BIG, /* 5 */

    /**
     * \brief Returned when the token or diagnostic failed to push, or the corresponding callback
     * is not specified (set to `NULL`).
     */
    M3C_ASM_ERROR_OOM = 6
} M3C_ASM_Error;

#endif /* _M3C_INCGUARD_ASM_ERRORS_H */

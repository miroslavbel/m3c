#ifndef _M3C_INCGUARD_ENCODINGS_H
#define _M3C_INCGUARD_ENCODINGS_H

#include <m3c/common/types.h>

#define M3C_UTF8_ERROR_OK 0

/**
 * \brief Codepoint start byte is invalid.
 *
 * \note Do not confuse with #M3C_UTF8_ERROR_OVERLONG_ENCODING.
 */
#define M3C_UTF8_ERROR_ILLEGAL_START_BYTE 1
/**
 * \brief Buffer ends earlier than the given codepoint.
 */
#define M3C_UTF8_ERROR_UNEXPECTED_EOF 2
/**
 * \brief Codepoint contains at least one invalid continuation byte.
 */
#define M3C_UTF8_ERROR_ILLEGAL_CONTINUATION_BYTE 3
/**
 * \brief Overlong encoding detected.
 */
#define M3C_UTF8_ERROR_OVERLONG_ENCODING 4
/**
 * \brief Codepoint encodes a value larger than the Unicode standard allows.
 */
#define M3C_UTF8_ERROR_CODEPOINT_TOO_BIG 5

/**
 * \brief Сhecks the first character from the given buffer against the UTF-8 encoding.
 *
 * \details If the character is valid, then moves the pointer so that it points to the first byte
 * of the next character. If the character is not valid, then the pointer remains in place, and an
 * error will be returned.
 *
 * \warning The buffer pointed to by \a ptr must be at least one byte in length.
 * \warning The presence of a validated UTF-8 codepoint in the Unicode Character Database (UCD) is
 * not checked.
 *
 * \param[in,out] ptr  pointer to a pointer to the first byte of the character being tested
 * \param[in]     last pointer to the last byte of the buffer
 * \return
 * - #M3C_UTF8_ERROR_OK - no error
 * - #M3C_UTF8_ERROR_ILLEGAL_START_BYTE - codepoint start byte is invalid
 * - #M3C_UTF8_ERROR_UNEXPECTED_EOF - buffer ends earlier than the given codepoint
 * - #M3C_UTF8_ERROR_ILLEGAL_CONTINUATION_BYTE - codepoint contains at least one invalid
 * continuation byte
 * - #M3C_UTF8_ERROR_OVERLONG_ENCODING - overlong encoding detected
 * - #M3C_UTF8_ERROR_CODEPOINT_TOO_BIG - codepoint encodes a value larger than the Unicode standard
 * allows
 */
m3c_u32 M3C_UTF8BufferValidate(const m3c_u8 **ptr, const m3c_u8 *const last);

#endif /* _M3C_INCGUARD_ENCODINGS_H */

#ifndef _M3C_INCGUARD_UTF8_H
#define _M3C_INCGUARD_UTF8_H

#include <m3c/common/types.h>
#include <m3c/common/errors.h>
#include <m3c/common/unicode.h>

/**
 * \brief UTF-8 code unit.
 */
typedef m3c_u8 M3C_UTF8cu;

/**
 * \brief Pointer to the UTF-8 buffer.
 *
 * \warning Can be not null-terminated.
 */
#define M3C_UTF8 M3C_UTF8cu *

/**
 * \brief Pointer to the UTF-8 null-terminated string.
 */
#define M3C_UTF8Z M3C_UTF8cu *

/**
 * \brief Replacement character (`�`) as a string literal.
 */
#define M3C_UTF8_REPLACEMENT_CHARACTER_STR "\xEF\xBF\xBD"

/**
 * \brief Length of replacement character (`�`) in UTF-8 encoding in bytes.
 *
 * \see #M3C_UTF8_REPLACEMENT_CHARACTER_STR
 */
#define M3C_UTF8_REPLACEMENT_CHARACTER_BLEN 3

/**
 * \brief Maximum number of bytes a character can occupy in UTF-8.
 */
#define M3C_UTF8_CP_MAX_BLEN 4

/**
 * \see #M3C_UTF8ValidateCodepoint
 */
#define M3C_UTF8_OK 0
/**
 * \see #M3C_UTF8ValidateCodepoint
 */
#define M3C_UTF8_ERR 1

/**
 * \brief Checks the validity of the first code point in the buffer.
 *
 * \details Returns the result of the validity check and writes to `*ptr` a pointer to the first
 * code unit of the next code point (or to the code unit immediately after the end of the buffer,
 * if `*ptr` pointed to the last code point). This function can be used to replace an invalid code
 * point in a buffer.
 *
 * \warning No overlong encoding is recovered and no checks for code unit sequence that decodes to
 * an invalid code point are performed.
 *
 * \param[in,out] ptr  a pointer to pointer to first code unit in buffer
 * \param[in]     last a pointer to the last code unit in the buffer or, if the buffer is empty, to
 * something with an address lesser than `*ptr`. Can be `NULL`
 * \return #M3C_UTF8_OK or #M3C_UTF8_ERR
 */
int M3C_UTF8ValidateCodepoint(const m3c_u8 **ptr, const m3c_u8 *last);

/**
 * \brief Decodes the first code point in the buffer. If it is an ASCII code point, it writes it to
 * `cp`. If it is not an ASCII code point or there is an error, it writes the code point of `�` to
 * the `cp`.
 *
 * \param[in]  ptr  a pointer to first code unit in buffer
 * \param[in]  last a pointer to the last code unit in the buffer or, if the buffer is empty, to
 * something with an address lesser than `ptr`. Can be `NULL`
 * \param[out] cp   writes here the decoded ASCII code point or `�`
 * \param[out] len  writes here the byte length of the decoded code point
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_EOF - if `last` is less than `ptr`
 * + #M3C_ERROR_INVALID_ENCODING - in this case, the code point of `�` is written to `cp` and the
 * length of the maximal subpart of an ill-formed subsequence is written to `len`
 */
M3C_ERROR M3C_UTF8GetASCIICodepointWithLen(
    const m3c_u8 *ptr, const m3c_u8 *last, M3C_UCP *cp, m3c_size_t *len
);

/**
 * \brief Decodes the first code point in the buffer.
 *
 * \param[in]  ptr  a pointer to first code unit in buffer
 * \param[in]  last a pointer to the last code unit in the buffer or, if the buffer is empty, to
 * something with an address lesser than `ptr`. Can be `NULL`
 * \param[out] cp   writes here decoded code point
 * \param[out] len  writes here the length of decoded code point
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_EOF - if `last` is less than `ptr`
 * + #M3C_ERROR_INVALID_ENCODING - in this case, the code point of `�` is written to `cp` and the
 * length of the maximal subpart of an ill-formed subsequence is written to `len`
 */
M3C_ERROR
M3C_UTF8ReadCodepointWithLen(const m3c_u8 *ptr, const m3c_u8 *last, M3C_UCP *cp, m3c_size_t *len);

/**
 * \brief Reads the previous code point in the buffer. If the pointer does not point to the first
 * byte of the code point, it will read that code point.
 *
 * \param[in]  ptr   a pointer
 * \param[in]  first a pointer to first code unit in buffer
 * \param[in]  last  a pointer to the last code unit in the buffer or, if the buffer is empty, to
 * something with an address lesser than `ptr`. Can be `NULL`
 * \param[out] cp    writes here decoded code point
 * \param[out] delta writes here the delta between `ptr` and the pointer to the first code unit of
 * read code point
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_EOF - if there are not previous code points in the buffer
 * + #M3C_ERROR_INVALID_ENCODING - in this case, the code point of `�` is written to `cp` and the
 * length of the maximal subpart of an ill-formed subsequence is written to `len`
 */
M3C_ERROR
M3C_UTF8ReadBackCodepointWithLen(
    const m3c_u8 *ptr, const m3c_u8 *first, const m3c_u8 *last, M3C_UCP *cp, m3c_size_t *delta
);

/**
 * \brief Writes the specified code point to the buffer.
 *
 * \param[out] ptr  a pointer to the first code unit to be written
 * \param      last a pointer to the last code unit in the buffer. Can be `NULL` if the buffer is
 * empty
 * \param      cp   a code point
 * \param[out] len  writes here the code point length in code units
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_EOF - if the buffer size is too small to write a code point into it
 * + #M3C_ERROR_INVALID_ENCODING - if code point is greater than `U+10FFFF`
 */
M3C_ERROR
M3C_UTF8WriteCodepointWithLen(m3c_u8 *ptr, const m3c_u8 *last, M3C_UCP cp, m3c_size_t *len);

#endif /* _M3C_INCGUARD_UTF8_H */

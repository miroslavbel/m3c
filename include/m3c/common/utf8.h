#ifndef _M3C_INCGUARD_UTF8_H
#define _M3C_INCGUARD_UTF8_H

#include <m3c/common/types.h>

/**
 * \see #M3C_UTF8ValidateChar
 */
#define M3C_UTF8_OK 0
/**
 * \see #M3C_UTF8ValidateChar
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

#endif /* _M3C_INCGUARD_UTF8_H */

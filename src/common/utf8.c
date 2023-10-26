#include <m3c/common/utf8.h>

#include <m3c/common/macros.h>

typedef struct __tagM3C_ByteRange {
    m3c_u8 lo;
    m3c_u8 hi;
} M3C_ByteRange;

const M3C_ByteRange M3C_UTF8SecondByteRanges[5] = {
    {0x80, 0xBF},
    {0xA0, 0xBF},
    {0x80, 0x9F},
    {0x90, 0xBF},
    {0x80, 0x8F}
};

int M3C_UTF8ValidateCodepoint(const m3c_u8 **ptr, const m3c_u8 *last) {
    const M3C_ByteRange *secondByte;
    int lost; /* the number of bytes to check after the second byte */

    if (last < *ptr)
        return M3C_UTF8_OK;

    if (M3C_InRange(**ptr, 0x00, 0x7F)) {
        (*ptr)++;
        return M3C_UTF8_OK;
    }

    if (M3C_InRange(**ptr, 0xC2, 0xDF)) {
        lost = 0;
        secondByte = &M3C_UTF8SecondByteRanges[0];
    } else if (M3C_InRange(**ptr, 0xE0, 0xE0)) {
        lost = 1;
        secondByte = &M3C_UTF8SecondByteRanges[1];
    } else if (M3C_InRange(**ptr, 0xE1, 0xEC) || M3C_InRange(**ptr, 0xEE, 0xEF)) {
        lost = 1;
        secondByte = &M3C_UTF8SecondByteRanges[0];
    } else if (M3C_InRange(**ptr, 0xED, 0xED)) {
        lost = 1;
        secondByte = &M3C_UTF8SecondByteRanges[2];
    } else if (M3C_InRange(**ptr, 0xF0, 0xF0)) {
        lost = 2;
        secondByte = &M3C_UTF8SecondByteRanges[3];
    } else if (M3C_InRange(**ptr, 0xF1, 0xF3)) {
        lost = 2;
        secondByte = &M3C_UTF8SecondByteRanges[0];
    } else if (M3C_InRange(**ptr, 0xF4, 0xF4)) {
        lost = 2;
        secondByte = &M3C_UTF8SecondByteRanges[4];
    } else {
        (*ptr)++;
        return M3C_UTF8_ERR;
    }
    (*ptr)++; /* now *ptr is pointing to the second byte */

    if (last - *ptr >= 0) {

        /* checking the second byte */
        if (!M3C_InRange(*ptr[0], secondByte->lo, secondByte->hi))
            return M3C_UTF8_ERR;
        (*ptr)++;

        /* checking the rest of bytes */
        while (lost > 0) {
            if (*ptr > last) {
                return M3C_UTF8_ERR;
            }

            if (!M3C_InRange(**ptr, 0x80, 0xBF)) {
                return M3C_UTF8_ERR;
            }

            (*ptr)++;
            lost--;
        }

        return M3C_UTF8_OK;
    }

    return M3C_UTF8_ERR;
}

M3C_ERROR M3C_UTF8GetASCIICodepointWithLen(
    const m3c_u8 *ptr, const m3c_u8 *last, M3C_UCP *cp, m3c_size_t *len
) {
    const M3C_ByteRange *secondByte;
    int lost; /* the number of bytes to check after the second byte */

    *len = 0;

    if (last < ptr)
        return M3C_ERROR_EOF;

    if (M3C_InRange(*ptr, 0x00, 0x7F)) {
        *len = 1;
        *cp = *ptr;
        return M3C_ERROR_OK;
    }

    *cp = M3C_REPLACEMENT_CHARACTER_UCP; /* in any other case (error or ok) */

    if (M3C_InRange(*ptr, 0xC2, 0xDF)) {
        lost = 0;
        secondByte = &M3C_UTF8SecondByteRanges[0];
    } else if (M3C_InRange(*ptr, 0xE0, 0xE0)) {
        lost = 1;
        secondByte = &M3C_UTF8SecondByteRanges[1];
    } else if (M3C_InRange(*ptr, 0xE1, 0xEC) || M3C_InRange(*ptr, 0xEE, 0xEF)) {
        lost = 1;
        secondByte = &M3C_UTF8SecondByteRanges[0];
    } else if (M3C_InRange(*ptr, 0xED, 0xED)) {
        lost = 1;
        secondByte = &M3C_UTF8SecondByteRanges[2];
    } else if (M3C_InRange(*ptr, 0xF0, 0xF0)) {
        lost = 2;
        secondByte = &M3C_UTF8SecondByteRanges[3];
    } else if (M3C_InRange(*ptr, 0xF1, 0xF3)) {
        lost = 2;
        secondByte = &M3C_UTF8SecondByteRanges[0];
    } else if (M3C_InRange(*ptr, 0xF4, 0xF4)) {
        lost = 2;
        secondByte = &M3C_UTF8SecondByteRanges[4];
    } else {
        *len = 1;
        return M3C_ERROR_INVALID_ENCODING;
    }
    ++ptr; /* now *ptr is pointing to the second byte */
    ++(*len);

    if (last - *ptr >= 0) {

        /* checking the second byte */
        if (!M3C_InRange(*ptr, secondByte->lo, secondByte->hi))
            return M3C_ERROR_INVALID_ENCODING;
        ++ptr;
        ++(*len);

        /* checking the rest of bytes */
        while (lost > 0) {
            if (ptr > last) {
                return M3C_ERROR_INVALID_ENCODING;
            }

            if (!M3C_InRange(*ptr, 0x80, 0xBF)) {
                return M3C_ERROR_INVALID_ENCODING;
            }

            ++ptr;
            ++(*len);
            --lost;
        }

        return M3C_ERROR_OK;
    }

    return M3C_ERROR_INVALID_ENCODING;
}

M3C_ERROR
M3C_UTF8WriteCodepointWithLen(m3c_u8 *ptr, const m3c_u8 *last, M3C_UCP cp, m3c_size_t *len) {

    if (last < ptr)
        return M3C_ERROR_EOF;

    if (cp < 0x80) {
        /* NOTE: buffer has a length equal to at least 1, so the check would be redundant */
        *len = 1;

        ptr[0] = (m3c_u8)cp;
        return M3C_ERROR_OK;

    } else if (cp < 0x800) {
        if (last - ptr < 1)
            return M3C_ERROR_EOF;
        *len = 2;

        ptr[0] = (cp >> 6 & 0x1F) + 0xC0;
        ptr[1] = (cp >> 0 & 0x3F) + 0x80;
        return M3C_ERROR_OK;

    } else if (cp < 0x10000) {
        if (last - ptr < 2)
            return M3C_ERROR_EOF;
        *len = 3;

        ptr[0] = (cp >> 12 & 0xF) + 0xE0;
        ptr[1] = (cp >> 6 & 0x3F) + 0x80;
        ptr[2] = (cp >> 0 & 0x3F) + 0x80;
        return M3C_ERROR_OK;

    } else if (cp < 0x110000) {
        if (last - ptr < 3)
            return M3C_ERROR_EOF;
        *len = 4;

        ptr[0] = (cp >> 18 & 0xF) + 0xF0;
        ptr[1] = (cp >> 12 & 0x3F) + 0x80;
        ptr[2] = (cp >> 6 & 0x3F) + 0x80;
        ptr[3] = (cp >> 0 & 0x3F) + 0x80;
        return M3C_ERROR_OK;

    } else
        return M3C_ERROR_INVALID_ENCODING;
}

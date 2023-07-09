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

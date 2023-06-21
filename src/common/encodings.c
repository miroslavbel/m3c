#include <m3c/common/encodings.h>

#define IsContinuationByte(ch) (0x80 <= (ch) && (ch) <= 0xBF)

m3c_u32 M3C_UTF8BufferValidate(const m3c_u8 **ptr, const m3c_u8 *const last) {
    m3c_u8 ch1 = **ptr;
    m3c_u8 ch2;
    m3c_u8 ch3;
    m3c_u8 ch4;
    m3c_u32 val;
    if (ch1 < 0x80) {
        (*ptr)++;
        return M3C_UTF8_ERROR_OK;
    } else if (ch1 >= 0xC2) { /* ch1: [0xC2 - 0xFF] */
        if (ch1 < 0xE0) {     /* ch1: [0xC2 - 0xE0) */
            /* two */
            if (*ptr + 1 > last)
                return M3C_UTF8_ERROR_UNEXPECTED_EOF;

            ch2 = *(*ptr + 1);
            if (IsContinuationByte(ch2)) {
                *ptr += 2;
                return M3C_UTF8_ERROR_OK;
            } else
                return M3C_UTF8_ERROR_ILLEGAL_CONTINUATION_BYTE;

        } else if (ch1 < 0xF0) { /* ch1: [0xE0 - 0xF0) */
            /* three */
            if (*ptr + 2 > last)
                return M3C_UTF8_ERROR_UNEXPECTED_EOF;

            ch2 = *(*ptr + 1);
            ch3 = *(*ptr + 2);
            if (!(IsContinuationByte(ch2) && IsContinuationByte(ch3)))
                return M3C_UTF8_ERROR_ILLEGAL_CONTINUATION_BYTE;

            val = (ch2 & 0x20 /* 0b100000 */) >> 5;
            val |= (ch1 & 0xf /*   0b1111 */) << 1;
            if (val == 0)
                return M3C_UTF8_ERROR_OVERLONG_ENCODING;

            *ptr += 3;
            return M3C_UTF8_ERROR_OK;
        } else if (ch1 < 0xF5) { /* ch1: [0xF0 - 0xF5) */
            /* four */
            if (*ptr + 3 > last)
                return M3C_UTF8_ERROR_UNEXPECTED_EOF;

            ch2 = *(*ptr + 1);
            ch3 = *(*ptr + 2);
            ch4 = *(*ptr + 3);
            if (!(IsContinuationByte(ch2) & IsContinuationByte(ch3) & IsContinuationByte(ch4)))
                return M3C_UTF8_ERROR_ILLEGAL_CONTINUATION_BYTE;

            val = (ch2 & 0x30 /* 0b110000 */) >> 4;
            val |= (ch1 & 0x7 /* 0b111 */) << 2;
            if (val >= 1) {
                *ptr += 4;
                return M3C_UTF8_ERROR_OK;
            } else
                return M3C_UTF8_ERROR_OVERLONG_ENCODING;

        } else if (ch1 < 0xF8) { /* ch1: [0xF5 - 0xF8) */
            /* four */
            if (*ptr + 3 > last) {
                return M3C_UTF8_ERROR_UNEXPECTED_EOF;
            }
            ch2 = *(*ptr + 1);
            ch3 = *(*ptr + 2);
            ch4 = *(*ptr + 3);
            if (IsContinuationByte(ch2) & IsContinuationByte(ch3) & IsContinuationByte(ch4))
                return M3C_UTF8_ERROR_CODEPOINT_TOO_BIG;
            else
                return M3C_UTF8_ERROR_ILLEGAL_CONTINUATION_BYTE;

        } else
            return M3C_UTF8_ERROR_ILLEGAL_START_BYTE; /* ch1: [0xF8 - 0xFF] */

    } else { /* ch1: [0x80 - 0xC2) */
        if (ch1 >= 0xC0) {
            /* two bytes */
            return M3C_UTF8_ERROR_OVERLONG_ENCODING; /* ch1: [0xC0 - 0xC2) */
        } else
            return M3C_UTF8_ERROR_ILLEGAL_START_BYTE; /* ch1: [0x80 - 0xC0) */
    }
}

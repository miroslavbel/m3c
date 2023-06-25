#ifndef _M3C_INCGUARD_ASM_TOKENIZER_DATA_H
#define _M3C_INCGUARD_ASM_TOKENIZER_DATA_H

/**
 * \brief The size of #__M3C_ASM_END_OF_TOKEN.
 */
#define __M3C_ASM_END_OF_TOKEN_N 2
/**
 * \brief Character ranges that signals the end of a token (without `\n`).
 *
 * \details Contains:
 * + `\t`, `\n`, ` ` - whitespaces
 *
 * \note The size is defined in #__M3C_ASM_END_OF_TOKEN.
 */
const m3c_u8 __M3C_ASM_END_OF_TOKEN[__M3C_ASM_END_OF_TOKEN_N][2] = {
    {'\t', '\n'},
    { ' ',  ' '}
};

/**
 * \brief The possible length of #__M3C_ASM_NUMBER_BODY.
 */
#define __M3C_ASM_BIN_NUMBER_BODY_N 2
/**
 * \brief The possible length of #__M3C_ASM_NUMBER_BODY.
 */
#define __M3C_ASM_OCT_NUMBER_BODY_N 3
/**
 * \brief The possible length of #__M3C_ASM_NUMBER_BODY.
 */
#define __M3C_ASM_DEC_NUMBER_BODY_N 4
/**
 * \brief The possible length of #__M3C_ASM_NUMBER_BODY.
 */
#define __M3C_ASM_HEX_NUMBER_BODY_N 6
/**
 * \brief Number body.
 *
 * \details RegEx for one char: `[_0-9A-Fa-f]`.
 *
 * Use the following values as array length:
 * + #__M3C_ASM_BIN_NUMBER_BODY_N for [_0-1]
 * + #__M3C_ASM_OCT_NUMBER_BODY_N for [_0-7]
 * + #__M3C_ASM_DEC_NUMBER_BODY_N for [_0-9]
 * + #__M3C_ASM_HEX_NUMBER_BODY_N for [_0-9A-Fa-f]
 *
 * For the version without underscore see #__M3C_ASM_DIGIT.
 */
const m3c_u8 __M3C_ASM_NUMBER_BODY[__M3C_ASM_HEX_NUMBER_BODY_N][2] = {
    {'_', '_'},
    {'0', '1'},
    {'2', '7'},
    {'8', '9'},
    {'A', 'F'},
    {'a', 'f'}
};

/**
 * \brief The possible length of #__M3C_ASM_DIGIT.
 */
#define __M3C_ASM_BIN_DIGIT_N 1
/**
 * \brief The possible length of #__M3C_ASM_DIGIT.
 */
#define __M3C_ASM_OCT_DIGIT_N 2
/**
 * \brief The possible length of #__M3C_ASM_DIGIT.
 */
#define __M3C_ASM_DEC_DIGIT_N 3
/**
 * \brief The possible length of #__M3C_ASM_DIGIT.
 */
#define __M3C_ASM_HEX_DIGIT_N 5
/**
 * \brief Digit.
 *
 * \details RegEx for one char: `[0-9A-Fa-f]`.
 *
 * Use the following values as array length:
 * + #__M3C_ASM_BIN_DIGIT_N for [0-1]
 * + #__M3C_ASM_OCT_DIGIT_N for [0-7]
 * + #__M3C_ASM_DEC_DIGIT_N for [0-9]
 * + #__M3C_ASM_HEX_DIGIT_N for [0-9A-Fa-f]
 *
 * For the version with underscore see #__M3C_ASM_NUMBER_BODY.
 */
#define __M3C_ASM_DIGIT __M3C_ASM_NUMBER_BODY + 1

/**
 * \brief The size of #__M3C_ASM_STRING_BODY.
 */
#define __M3C_ASM_STRING_BODY_N 3
/**
 * \brief String body.
 *
 * \details RegEx for one char: `[0-9A-Za-z]`.
 *
 * \note The size is defined in #__M3C_ASM_STRING_BODY_N.
 */
const m3c_u8 __M3C_ASM_STRING_BODY[__M3C_ASM_STRING_BODY_N][2] = {
    {'0', '9'},
    {'A', 'Z'},
    {'a', 'z'}
};

#endif /* _M3C_INCGUARD_ASM_TOKENIZER_DATA_H */

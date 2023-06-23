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
 * + `\t`, ` ` - whitespaces
 *
 * \note The size is defined in #__M3C_ASM_END_OF_TOKEN.
 */
const m3c_u8 __M3C_ASM_END_OF_TOKEN[__M3C_ASM_END_OF_TOKEN_N][2] = {
    {'\t', '\t'},
    { ' ',  ' '}
};

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

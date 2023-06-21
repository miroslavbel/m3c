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

#endif /* _M3C_INCGUARD_ASM_TOKENIZER_DATA_H */

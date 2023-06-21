#include <m3c/asm/tokenizer.h>

#include <m3c/common/macros.h>
#include <m3c/common/encodings.h>

#include "tokenizer_data.h"

#define __M3C_ASM_IsEof (tokenizer->ptr > tokenizer->src.last)

/**
 * \brief Advances \a tokenizer by one ASCII character.
 *
 * \details Just increase \ref #M3C_ASM_Tokenizer::ptr "tokenizer->ptr" and \ref
 * #M3C_ASM_Position::character "tokenizer->pos.character" by one.
 */
#define __M3C_ASM_Advance                                                                          \
    tokenizer->ptr++;                                                                              \
    tokenizer->pos.character++

/**
 * \brief Advances \a tokenizer by `\n` character.
 *
 * \details Just increase \ref #M3C_ASM_Tokenizer::ptr "tokenizer->ptr" and \ref
 * #M3C_ASM_Position::line "tokenizer->pos.line" by one and reset \ref #M3C_ASM_Position::character
 * "tokenizer->pos.character" to `0`.
 */
#define __M3C_ASM_AdvanceNL                                                                        \
    tokenizer->ptr++;                                                                              \
    tokenizer->pos.character = 0;                                                                  \
    tokenizer->pos.line++

/**
 * \brief Checks \a ch for matching against \a ranges.
 *
 * \param[in] ranges array of ASCII characters ranges
 * \param     n      number of elements in \a ranges
 * \param     ch     character to check
 * \return           does \a ch match
 */
m3c_bool __M3C_ASM_does_match(const m3c_u8 ranges[][2], m3c_u32 n, m3c_u8 ch) {
    m3c_u32 i = 0;
    for (; i < n; i++) {
        if (ranges[i][0] <= ch && ch <= ranges[i][1])
            return 1;
    }

    return 0;
}

/**
 * \brief Calculates the length of the given \a token and pushes it to \ref
 * #M3C_ASM_Tokenizer.tokens "tokenizer::tokens".
 *
 * \details Calculates the length of the \a token (assumes that the \ref #M3C_ASM_Tokenizer.ptr
 * "tokenizer::ptr" points to the byte after the token) and tries to push it into \ref
 * #M3C_ASM_Tokenizer.tokens "tokenizer::tokens". If \ref #M3C_ASM_TokenizerOptions.tokens_realloc
 * "options::tokens_realloc" callback is not defined (is set to `NULL`) or if the callback returned
 * an error, it returns an #M3C_ASM_ERROR_OOM error. Returns #M3C_ASM_ERROR_OK on success.
 *
 * \note Does not invalidate the token.
 *
 * \param[in,out] tokenizer tokenizer
 * \param[in]     options   options
 * \param[in]     token     the token to push
 * \return                  #M3C_ASM_ERROR_OOM or #M3C_ASM_ERROR_OK
 */
M3C_ASM_Error __M3C_ASM_push_token(
    M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options, M3C_ASM_Token *token
) {
    m3c_u32 newCap;
    void *newPtr;
    token->len = (m3c_u32)(tokenizer->ptr - token->ptr);

    if (tokenizer->tokens->len == tokenizer->tokens->cap) {
        if (!options->tokens_realloc)
            return M3C_ASM_ERROR_OOM;

        /* There is no reason to ask for more than the number of characters in src + 1. */
        newCap = m3c_min(
            m3c_max(1, (m3c_u32)(tokenizer->src.last - tokenizer->src.first)),
            tokenizer->tokens->cap + tokenizer->tokens->cap
        );
        newPtr = options->tokens_realloc(tokenizer->tokens->data, sizeof(M3C_ASM_Token) * newCap);
        if (!newPtr)
            return M3C_ASM_ERROR_OOM;

        tokenizer->tokens->data = newPtr;
        tokenizer->tokens->cap = newCap;
    }

    tokenizer->tokens->data[tokenizer->tokens->len++] = *token;
    return M3C_ASM_ERROR_OK;
}

M3C_ASM_Error
__M3C_ASM_handle_EOF(M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options) {
    M3C_ASM_Token token;
    token.kind = M3C_ASM_EOF_TOKEN;
    token.ptr = tokenizer->ptr;
    token.pos = tokenizer->pos;
    token.file = tokenizer->name;

    return __M3C_ASM_push_token(tokenizer, options, &token);
}

/**
 * \brief Sets the type of the given \a token to \ref #M3C_ASM_Token_Kind.M3C_ASM_INVDALID_TOKEN
 * "invalid" and parses that token until the end of the token (or the end of \ref
 * #M3C_ASM_Tokenizer.src "token.src").
 *
 * \details Assumes \ref #M3C_ASM_Tokenizer.ptr "token.ptr" to be set to the first byte of
 * the given \a token. If successful, sets a pointer to the next byte after the parsed token (which
 * may be outside the \ref #M3C_ASM_Tokenizer.src "token.src" boundary).
 *
 * \param[in,out] tokenizer tokenizer
 * \param[in]     options   options
 * \param[in,out] token     token
 * \return
 * - #M3C_ASM_ERROR_OK - if okay
 * - #M3C_ASM_ERROR_OOM - if failed to push token
 * - UTF-8 specific errors (see #M3C_UTF8BufferValidate):
 *   + #M3C_ASM_ERROR_UTF8_UNEXPECTED_EOF
 *   + #M3C_ASM_ERROR_UTF8_ILLEGAL_START_BYTE
 *   + #M3C_ASM_ERROR_UTF8_ILLEGAL_CONTINUATION_BYTE
 *   + #M3C_ASM_ERROR_UTF8_OVERLONG_ENCODING
 *   + #M3C_ASM_ERROR_UTF8_CODEPOINT_TOO_BIG
 */
M3C_ASM_Error __M3C_ASM_parse_invalid_token(
    M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options, M3C_ASM_Token *token
) {
    M3C_ASM_Error res;
    m3c_u8 ch;
    token->kind = M3C_ASM_INVDALID_TOKEN;

    /* we already know that this (first) char is invalid (and not \n) */
    __M3C_ASM_Advance;

    M3C_LOOP {
        if (__M3C_ASM_IsEof)
            return __M3C_ASM_push_token(tokenizer, options, token);
        ch = *tokenizer->ptr;

        if (ch >= 0x80) {
            M3C_IfRet(res, M3C_UTF8BufferValidate(&tokenizer->ptr, tokenizer->src.last));
        } else {
            if (ch == '\n') {
                /* push invalid token before `\n` */
                token->len = (m3c_u32)(tokenizer->ptr - token->ptr);
                M3C_IfRet(res, __M3C_ASM_push_token(tokenizer, options, token));

                /* set `\n` token itself */
                token->kind = M3C_ASM_NL_TOKEN;
                token->ptr = tokenizer->ptr;
                token->pos = tokenizer->pos;
                token->file = tokenizer->name;

                /* push `\n` token */
                __M3C_ASM_AdvanceNL;
                M3C_IfRet(res, __M3C_ASM_push_token(tokenizer, options, token));

                return M3C_ASM_ERROR_OK;

            } else if (__M3C_ASM_does_match(__M3C_ASM_END_OF_TOKEN, __M3C_ASM_END_OF_TOKEN_N, ch))
                return __M3C_ASM_push_token(tokenizer, options, token);
        }

        __M3C_ASM_Advance;
    }
}

/**
 * \brief Parses and returns the next token (but not #M3C_ASM_EOF_TOKEN) if there is one.
 *
 * \param[in, out] tokenizer tokenizer
 * \param[in]      options   options
 * \return
 * - #M3C_ASM_ERROR_OK - if okay
 * - #M3C_ASM_ERROR_OOM - if failed to push token
 * - UTF-8 specific errors (see #M3C_UTF8BufferValidate):
 *   + #M3C_ASM_ERROR_UTF8_UNEXPECTED_EOF
 *   + #M3C_ASM_ERROR_UTF8_ILLEGAL_START_BYTE
 *   + #M3C_ASM_ERROR_UTF8_ILLEGAL_CONTINUATION_BYTE
 *   + #M3C_ASM_ERROR_UTF8_OVERLONG_ENCODING
 *   + #M3C_ASM_ERROR_UTF8_CODEPOINT_TOO_BIG
 */
M3C_ASM_Error
__M3C_ASM_parse_next_token(M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options) {
    M3C_ASM_Token token;
    M3C_ASM_Error res;
    m3c_u8 ch;

    /* skip whitespaces */
    M3C_LOOP {
        if (__M3C_ASM_IsEof)
            return M3C_ASM_ERROR_OK;

        ch = *tokenizer->ptr;
        if (ch != '\t' && ch != ' ')
            break;

        __M3C_ASM_Advance;
    }

    /* init token. kind is not known yet */
    token.ptr = tokenizer->ptr;
    token.pos = tokenizer->pos;
    token.len = 0;
    token.file = tokenizer->name;

    /* token cannot contain not ASCII chars */
    if (ch >= 0x80)
        return __M3C_ASM_parse_invalid_token(tokenizer, options, &token);

    switch (ch) {
        case '\n':
            token.kind = M3C_ASM_NL_TOKEN;

            M3C_IfRet(res, __M3C_ASM_push_token(tokenizer, options, &token));

            __M3C_ASM_AdvanceNL;
            return res;

        /* TODO: impl another tokens*/
        default:
            /* TODO: diagnostics "invalid token at " */
            return __M3C_ASM_parse_invalid_token(tokenizer, options, &token);
    }
}

M3C_ASM_Error
M3C_ASM_Tokenize(M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options) {
    M3C_ASM_Error res;

    M3C_LOOP {
        if (__M3C_ASM_IsEof)
            return __M3C_ASM_handle_EOF(tokenizer, options);

        M3C_IfRet(res, __M3C_ASM_parse_next_token(tokenizer, options));
    }
}

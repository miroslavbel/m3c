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
    m3c_u32 newCapPlusOne;
    void *newPtr;
    token->len = (m3c_u32)(tokenizer->ptr - token->ptr);

    if (tokenizer->tokens->len == tokenizer->tokens->cap) {
        if (!options->tokens_realloc)
            return M3C_ASM_ERROR_OOM;

        /* There is no reason to ask for more than the number of characters in src + 1. */
        newCapPlusOne = m3c_min(
            m3c_max(
                2 /* M3C_ASM_EOF_TOKEN and `P+N+1` rule */,
                (m3c_u32)(tokenizer->src.last - tokenizer->src.first) +
                    2 /* M3C_ASM_EOF_TOKEN and `P+N+1` rule */
            ),
            tokenizer->tokens->cap + tokenizer->tokens->cap + 1 /* `P+N+1` rule */
        );
        newPtr =
            options->tokens_realloc(tokenizer->tokens->data, sizeof(M3C_ASM_Token) * newCapPlusOne);
        if (!newPtr)
            return M3C_ASM_ERROR_OOM;

        tokenizer->tokens->data = newPtr;
        tokenizer->tokens->cap = newCapPlusOne - 1;
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
 * #M3C_ASM_Tokenizer.src "tokenizer.src").
 *
 * \details Assumes \ref #M3C_ASM_Tokenizer.ptr "tokenizer.ptr" to be set to the first invalid byte
 * of the given \a token. If successful, sets a pointer to the next byte after the parsed token
 * (which may be outside the \ref #M3C_ASM_Tokenizer.src "tokenizer.src" boundary).
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

    M3C_LOOP {
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
                return __M3C_ASM_push_token(tokenizer, options, token);

            } else if (__M3C_ASM_does_match(__M3C_ASM_END_OF_TOKEN, __M3C_ASM_END_OF_TOKEN_N, ch))
                return __M3C_ASM_push_token(tokenizer, options, token);
        }

        __M3C_ASM_Advance;
        if (__M3C_ASM_IsEof)
            return __M3C_ASM_push_token(tokenizer, options, token);
    }
}

/**
 * \brief Parses noninitial digits in the number body. The number of noninitial digits can be 0 or
 * more.
 *
 * \param[in,out] tokenizer tokenizer
 * \param[in]     options   options
 * \param[in,out] token     token
 * \param n                 the length of #__M3C_ASM_NUMBER_BODY
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
M3C_ASM_Error __M3C_ASM_parse_noninitial_digits(
    M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options, M3C_ASM_Token *token,
    m3c_u32 n
) {
    m3c_u8 ch;

    M3C_LOOP {
        if (__M3C_ASM_IsEof)
            return __M3C_ASM_push_token(tokenizer, options, token);
        ch = *tokenizer->ptr;

        if (__M3C_ASM_does_match(__M3C_ASM_NUMBER_BODY, n, ch)) {
            __M3C_ASM_Advance;
            continue;
        }

        if (__M3C_ASM_does_match(__M3C_ASM_END_OF_TOKEN, __M3C_ASM_END_OF_TOKEN_N, ch))
            return __M3C_ASM_push_token(tokenizer, options, token);
        else
            /* TODO: diagnostics "invalid digit for this type of number literal" */
            return __M3C_ASM_parse_invalid_token(tokenizer, options, token);
    }
}

/**
 * \brief Parse the body of a numeric literal (after the prefix).
 *
 * \details Assumes that \ref #M3C_ASM_Tokenizer.ptr "tokenizer.ptr" points to the next byte after
 * the prefix.
 *
 * \param[in,out] tokenizer tokenizer
 * \param[in]     options   options
 * \param[in,out] token     token
 * \param n                 the length of #__M3C_ASM_NUMBER_BODY
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
M3C_ASM_Error __M3C_ASM_parse_number_body(
    M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options, M3C_ASM_Token *token,
    m3c_u32 n
) {
    m3c_u8 ch;

    if (__M3C_ASM_IsEof) {
        /* TODO: diagnostics "number must contain at least one digit" */
        token->kind = M3C_ASM_INVDALID_TOKEN;
        return __M3C_ASM_push_token(tokenizer, options, token);
    }

    /* skip possible underscore after prefix */
    M3C_LOOP {
        ch = *tokenizer->ptr;
        if (ch != '_')
            break;

        __M3C_ASM_Advance;
        if (__M3C_ASM_IsEof) {
            token->kind = M3C_ASM_INVDALID_TOKEN;
            return __M3C_ASM_push_token(tokenizer, options, token);
        }
    }

    /* first digit */
    if (!__M3C_ASM_does_match(__M3C_ASM_DIGIT, n - 1, ch))
        /* TODO: diagnostics "number must contain at least one digit" */
        return __M3C_ASM_parse_invalid_token(tokenizer, options, token);
    __M3C_ASM_Advance;

    /* any noninitial digits */
    return __M3C_ASM_parse_noninitial_digits(tokenizer, options, token, n);
}

/**
 * \brief Parses a number literal after the first char of `0`.
 *
 * \details Assumes that \ref #M3C_ASM_Tokenizer.ptr "tokenizer.ptr" points to the next byte after
 * zero. Leading zero is prohibited. The prefix can be `0b`, `0o`, `0d`, `0x`.
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
M3C_ASM_Error __M3C_ASM_parse_number_literal(
    M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options, M3C_ASM_Token *token
) {
    m3c_u8 ch;

    if (__M3C_ASM_IsEof)
        return __M3C_ASM_push_token(tokenizer, options, token);
    ch = *tokenizer->ptr;

    if (ch == 'b') {
        __M3C_ASM_Advance;
        return __M3C_ASM_parse_number_body(tokenizer, options, token, __M3C_ASM_BIN_NUMBER_BODY_N);
    } else if (ch == 'o') {
        __M3C_ASM_Advance;
        return __M3C_ASM_parse_number_body(tokenizer, options, token, __M3C_ASM_OCT_NUMBER_BODY_N);
    } else if (ch == 'd') {
        __M3C_ASM_Advance;
        return __M3C_ASM_parse_number_body(tokenizer, options, token, __M3C_ASM_DEC_NUMBER_BODY_N);
    } else if (ch == 'x') {
        __M3C_ASM_Advance;
        return __M3C_ASM_parse_number_body(tokenizer, options, token, __M3C_ASM_HEX_NUMBER_BODY_N);
    } else if (__M3C_ASM_does_match(__M3C_ASM_NUMBER_BODY, __M3C_ASM_HEX_NUMBER_BODY_N, ch)) {
        /* TODO: diagnostics "leading zeros in number literals are not permitted" */
        return __M3C_ASM_parse_invalid_token(tokenizer, options, token);
    } else if (__M3C_ASM_does_match(__M3C_ASM_END_OF_TOKEN, __M3C_ASM_END_OF_TOKEN_N, ch)) {
        return __M3C_ASM_push_token(tokenizer, options, token);
    }

    /* TODO: diagnostics "invalid number prefix" */
    return __M3C_ASM_parse_invalid_token(tokenizer, options, token);
}

/**
 * \brief Parses a string literal.
 *
 * \details Assumes \ref #M3C_ASM_Tokenizer.ptr "tokenizer.ptr" to be set to the first byte after
 * `"`. If successful, sets a pointer to the next byte after the parsed token (which may be outside
 * the \ref #M3C_ASM_Tokenizer.src "tokenizer.src" boundary).
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
M3C_ASM_Error __M3C_ASM_parse_string_literal(
    M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options, M3C_ASM_Token *token
) {
    M3C_ASM_Error res;
    m3c_u8 ch;

    M3C_LOOP {
        if (__M3C_ASM_IsEof) {
            /* TODO: diagnostics "unterminated string" */
            return __M3C_ASM_push_token(tokenizer, options, token);
        }
        ch = *tokenizer->ptr;

        if (ch >= 0x80) {
            /* TODO: diagnostics "string can only contain ASCII digits and letters" */
            return __M3C_ASM_parse_invalid_token(tokenizer, options, token);
        }

        if (ch == '\n') {
            /* TODO: diagnostics "unterminated string" */

            /* push string token */
            token->len = (m3c_u32)(tokenizer->ptr - token->ptr);
            M3C_IfRet(res, __M3C_ASM_push_token(tokenizer, options, token));

            /* set `\n` token itself */
            token->kind = M3C_ASM_NL_TOKEN;
            token->ptr = tokenizer->ptr;
            token->pos = tokenizer->pos;
            token->file = tokenizer->name;

            /* push `\n` token */
            __M3C_ASM_AdvanceNL;
            return __M3C_ASM_push_token(tokenizer, options, token);
        } else if (ch == '"') {
            __M3C_ASM_Advance;
            return __M3C_ASM_push_token(tokenizer, options, token);
        } else if (__M3C_ASM_does_match(__M3C_ASM_STRING_BODY, __M3C_ASM_STRING_BODY_N, ch)) {
            __M3C_ASM_Advance;
            continue;
        } else {
            /* TODO: diagnostics "string can only contain ASCII digits and letters" */
            return __M3C_ASM_parse_invalid_token(tokenizer, options, token);
        }
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

    if (ch == '\n') {
        token.kind = M3C_ASM_NL_TOKEN;

        __M3C_ASM_AdvanceNL;
        return __M3C_ASM_push_token(tokenizer, options, &token);
    } else if (ch == '"') {
        token.kind = M3C_ASM_STRING_LITERAL_TOKEN;
        __M3C_ASM_Advance;
        return __M3C_ASM_parse_string_literal(tokenizer, options, &token);
    } else if (ch == '0') {
        token.kind = M3C_ASM_NUMBER_LITERAL_TOKEN;
        __M3C_ASM_Advance;
        return __M3C_ASM_parse_number_literal(tokenizer, options, &token);
    } else if ('0' <= ch && ch <= '9') {
        token.kind = M3C_ASM_NUMBER_LITERAL_TOKEN;
        __M3C_ASM_Advance;
        return __M3C_ASM_parse_noninitial_digits(
            tokenizer, options, &token, __M3C_ASM_DEC_NUMBER_BODY_N
        );
    } else { /* TODO: impl another tokens */
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

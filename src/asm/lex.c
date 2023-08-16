#include <m3c/asm/lex.h>

#include <m3c/common/macros.h>
#include <m3c/common/utf8.h>
#include <m3c/asm/diagnostics_info.h>
#include <m3c/asm/preproc.h>

#define __ADVANCE_ONLY_PTR lexer->ptr += cpLen

#define __ADVANCE_ONLY_POS ++lexer->pos.character
#define __ADVANCE_ONLY_POS_NL                                                                      \
    ++lexer->pos.line;                                                                             \
    lexer->pos.character = 0

#define VAR_DECL                                                                                   \
    M3C_ERROR status;                                                                              \
    M3C_UCP cp;                                                                                    \
    m3c_size_t cpLen

/**
 * \brief Reads the character pointed to by the lexer. If there are no characters left in the
 * current fragment, it will move to the next fragment and try to read again.
 *
 * \warning Requires variables to be declared with the #VAR_DECL macro.
 *
 * \see #__M3C_ASM_Lexer_peek
 */
#define PEEK status = __M3C_ASM_Lexer_peek(lexer, &cp, &cpLen)

/**
 * \brief Moves the lexer forward, assuming the lexer doesn't point to a newline character.
 *
 * \warning Requires macro #PEEK to be called before it.
 */
#define ADVANCE                                                                                    \
    __ADVANCE_ONLY_PTR;                                                                            \
    __ADVANCE_ONLY_POS
/**
 * \brief Moves the lexer forward, assuming the lexer points to a newline character.
 *
 * \warning Requires macro #PEEK to be called before it.
 */
#define ADVANCE_NL                                                                                 \
    __ADVANCE_ONLY_PTR;                                                                            \
    __ADVANCE_ONLY_POS_NL

/**
 * \brief Sets the ptr and start position of the token.
 *
 * \details The lexer must point to the first character of the token.
 */
#define TOK_START(token)                                                                           \
    (token)->ptr = lexer->ptr;                                                                     \
    (token)->start = lexer->pos
/**
 * \brief Sets the end position of the token.
 *
 * \details The lexer must point to the next character after the token.
 */
#define TOK_END(token) (token)->end = lexer->pos

/**
 * \brief Sets the diagnostic start position from the current lexer position.
 *
 * \details The lexer must point to the first character of the diagnostic.
 */
#define DIAG_START_FROM_LEXER(diag)                                                                \
    (diag)->data.ASM.start = lexer->pos;                                                           \
    (diag)->data.ASM.hInclude = lexer->hInclude
/**
 * \brief Sets the diagnostic start position from the token start position.
 */
#define DIAG_START_FROM_TOKEN(diag, token)                                                         \
    (diag)->data.ASM.start = (token)->start;                                                       \
    (diag)->data.ASM.hInclude = lexer->hInclude
/**
 * \brief Sets the end position of the diagnostic.
 *
 * \details The lexer must point to the next character after the diagnostic.
 */
#define DIAG_END(diag) (diag)->data.ASM.end = lexer->pos

typedef struct __tagM3C_ASM_Lexer {
    m3c_u8 const *ptr;
    M3C_ASM_Position pos;
    m3c_u8 const *bLast;
    M3C_ASM_Fragment *fragment;
    M3C_ASM_Fragment *fragmentLast;
    M3C_ASM_Tokens *tokens;
    M3C_Diagnostics *diagnostics;
    M3C_hINCLUDE hInclude;
} M3C_ASM_Lexer;

/**
 * \brief Reads the character pointed to by the lexer. If there are no characters left in the
 * current fragment, it will move to the next fragment and try to read again.
 *
 * \param[in,out] lexer lexer
 * \param[out]    cp    writes here the decoded ASCII code point or `�`
 * \param[out]    cpLen writes here the byte length of the decoded code point
 * \return
 * + #M3C_ERROR_OK - if successfully read
 *   + ASCII code point - writes read code point to `cp`, writes len to `cpLen`
 *   + non-ASCII code point - writes `�` to `cp`, writes len to `cpLen`
 * + #M3C_ERROR_INVALID_ENCODING - in case of invalid encoding (writes `�` to `cp`, writes len to
 * `cpLen`)
 * + #M3C_ERROR_EOF - if there is no bytes left
 */
M3C_ERROR __M3C_ASM_Lexer_peek(M3C_ASM_Lexer *lexer, M3C_UCP *cp, m3c_size_t *cpLen) {
    if (lexer->ptr <= lexer->fragment->bLast)
        /* if it's not the end of the fragment just read the next char */
        return M3C_UTF8GetASCIICodepointWithLen(lexer->ptr, lexer->fragment->bLast, cp, cpLen);

    /* looking for non-empty fragment */
    M3C_LOOP {
        ++lexer->fragment;
        if (lexer->fragment > lexer->fragmentLast)
            return M3C_ERROR_EOF;

        if (lexer->fragment->bLast != M3C_NULL)
            break;
    }

    lexer->ptr = lexer->fragment->bFirst;
    lexer->pos = lexer->fragment->pos;
    return M3C_UTF8GetASCIICodepointWithLen(lexer->ptr, lexer->fragment->bLast, cp, cpLen);
}

/**
 * \brief Lexes \ref M3C_ASM_TOKEN_KIND_UNRECOGNIZED "UNRECOGNIZED" token.
 *
 * \details Diagnostics:
 * + (required) \ref M3C_ASM_DIAGNOSTIC_ID_UNRECOGNIZED_TOKEN "UNRECOGNIZED_TOKEN"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING"
 *
 * \param[in,out] lexer lexer
 * \param[in,out] token token
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_EOF - if EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexUnrecognisedToken(M3C_ASM_Lexer *lexer, M3C_ASM_Token *token) {
    VAR_DECL;
    M3C_Diagnostic diagInvalidToken;
    M3C_Diagnostic diagInvalidEncoding;

    /* we need to remember unrecognized token diagnostic index as we may need to add
     * diagInvalidEncoding diagnostics */
    m3c_size_t unrecognizedTokenDiagIndex;

    diagInvalidToken.severity = M3C_SEVERITY_ERROR;
    diagInvalidToken.info = &M3C_ASM_DIAGNOSTIC_INFO_UNRECOGNIZED_TOKEN;
    diagInvalidEncoding.severity = M3C_SEVERITY_ERROR;
    diagInvalidEncoding.info = &M3C_ASM_DIAGNOSTIC_INFO_INVALID_ENCODING;

    DIAG_START_FROM_TOKEN(&diagInvalidToken, token);

    /* push diagnostic and save its index */
    unrecognizedTokenDiagIndex = lexer->diagnostics->vec.len;
    if (M3C_VEC_PUSH(M3C_Diagnostic, &lexer->diagnostics->vec, &diagInvalidToken) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;
    ++lexer->diagnostics->errors;

    /* looking for EOF or EOT */
    M3C_LOOP {
        PEEK;

        if (status == M3C_ERROR_EOF)
            break;
        else if (status == M3C_ERROR_OK) {
            if (cp == '\t' || cp == ' ' || cp == '\n') /* EOT */
                break;
            else {
                ADVANCE;
                continue;
            }
        }
        /* well, let's handle invalid encoding (status == M3C_ERROR_INVALID_ENCODING) */

        DIAG_START_FROM_LEXER(&diagInvalidEncoding);
        ADVANCE;

        /* looking for EOF or valid code point */
        M3C_LOOP {
            PEEK;
            if (status == M3C_ERROR_OK || status == M3C_ERROR_EOF)
                break;

            ADVANCE;
        }
        DIAG_END(&diagInvalidEncoding);

        if (M3C_VEC_PUSH(M3C_Diagnostic, &lexer->diagnostics->vec, &diagInvalidEncoding) !=
            M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;
    }

    token->kind = M3C_ASM_TOKEN_KIND_UNRECOGNIZED;
    TOK_END(token);

    if (M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, token) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;

    /* NOTE: vector can be reallocated, so access only by index */
    DIAG_END(&lexer->diagnostics->vec.data[unrecognizedTokenDiagIndex]);

    return status;
}

/**
 * \brief Lexes \ref M3C_ASM_TOKEN_KIND_COMMENT "COMMENT" token.
 *
 * \details Diagnostics:
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING"
 *
 * \param[in,out] lexer lexer
 * \param[out]    token token
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_EOF - if EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexCommentToken(M3C_ASM_Lexer *lexer, M3C_ASM_Token *token) {
    VAR_DECL;
    M3C_Diagnostic diagInvalidEncoding;

    diagInvalidEncoding.severity = M3C_SEVERITY_ERROR;
    diagInvalidEncoding.info = &M3C_ASM_DIAGNOSTIC_INFO_INVALID_ENCODING;

    /* looking for EOL or EOF */
    M3C_LOOP {
        PEEK;

        if (status == M3C_ERROR_EOF)
            break;
        else if (status == M3C_ERROR_OK) {
            if (cp == '\n')
                break;
            else {
                ADVANCE;
                continue;
            }
        }
        /* well, let's handle invalid encoding (status == M3C_ERROR_INVALID_ENCODING) */

        DIAG_START_FROM_LEXER(&diagInvalidEncoding);
        ADVANCE;

        /* looking for EOF or valid code point */
        M3C_LOOP {
            PEEK;
            if (status == M3C_ERROR_OK || status == M3C_ERROR_EOF)
                break;

            ADVANCE;
        }
        DIAG_END(&diagInvalidEncoding);

        if (M3C_VEC_PUSH(M3C_Diagnostic, &lexer->diagnostics->vec, &diagInvalidEncoding) !=
            M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;
    }

    token->kind = M3C_ASM_TOKEN_KIND_COMMENT;
    TOK_END(token);

    if (M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, token) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;

    return status;
}

/**
 * \brief Lexes the next token (if there is one).
 *
 * \note If EOF is reached but no token is found, #M3C_ERROR_OK is returned.
 *
 * \param[in,out] lexer
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_EOF - if EOF is reached
 * + #M3C_ERROR_OOM - failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexNextToken(M3C_ASM_Lexer *lexer) {
    VAR_DECL;
    M3C_ASM_Token token;

    /* skip whitespaces (only space and '\t') */
    M3C_LOOP {
        PEEK;
        if (status == M3C_ERROR_EOF)
            return status;

        if (cp != ' ' && cp != '\t')
            break;
        ADVANCE;
    }
    TOK_START(&token);

    if (cp == '\n') {
        token.kind = M3C_ASM_TOKEN_KIND_NEW_LINE;
        ADVANCE_NL;
        TOK_END(&token);
        return M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, &token);
    } else if (cp == ';')
        return __M3C_ASM_lexCommentToken(lexer, &token);
    else
        return __M3C_ASM_lexUnrecognisedToken(lexer, &token);
}

M3C_ERROR M3C_ASM_lex(M3C_ASM_Document *document) {
    M3C_ASM_Lexer lexer;
    M3C_ERROR res;

    if (document->fragments.len == 0)
        return M3C_ERROR_OK;
    lexer.fragment = document->fragments.data;
    lexer.fragmentLast = &document->fragments.data[document->fragments.len - 1];

    lexer.pos = lexer.fragment->pos;
    lexer.ptr = document->bFirst;
    lexer.bLast = document->bLast;

    lexer.diagnostics = &document->diagnostics;
    lexer.tokens = &document->tokens;
    lexer.hInclude = document->hInclude;

    M3C_LOOP {
        res = __M3C_ASM_lexNextToken(&lexer);
        if (res == M3C_ERROR_EOF)
            return M3C_ERROR_OK;
        else if (res != M3C_ERROR_OK)
            return res;
        else
            continue;
    }
}

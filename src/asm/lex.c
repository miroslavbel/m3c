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

#define __VAR_DECL_WITHOUT_N                                                                       \
    M3C_ERROR status;                                                                              \
    M3C_UCP cp;                                                                                    \
    m3c_size_t cpLen

#define VAR_DECL                                                                                   \
    __VAR_DECL_WITHOUT_N;                                                                          \
    m3c_size_t n

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

typedef struct __tagM3C_ASM_ASCIIRange {
    m3c_u8 lo;
    m3c_u8 hi;
} M3C_ASM_ASCIIRange;

const M3C_ASM_ASCIIRange __underscore_digits_letters[8] = {
  /* the order is important */
    {'_', '_'},
    {'0', '1'},
    {'2', '7'},
    {'8', '9'},
    {'A', 'F'},
    {'a', 'f'},
    {'G', 'Z'},
    {'g', 'z'}
};

/**
 * \brief Digits (binary, octal, decimal, hexadecimal).
 *
 * \details To use see #DIGITS_LEN_BIN, #DIGITS_LEN_OCT, #DIGITS_LEN_DEC, #DIGITS_LEN_HEX.
 */
#define DIGITS &__underscore_digits_letters[1]
/**
 * \brief `[01]`.
 *
 * \details See #DIGITS.
 */
#define DIGITS_LEN_BIN 1
/**
 * \brief `[0-7]`.
 *
 * \details See #DIGITS.
 */
#define DIGITS_LEN_OCT 2
/**
 * \brief `[0-9]`.
 *
 * \details See #DIGITS.
 */
#define DIGITS_LEN_DEC 3
/**
 * \brief `[0-9A-Fa-f]`.
 *
 * \details See #DIGITS.
 */
#define DIGITS_LEN_HEX 5

/**
 * \brief Underscore (`_`) and digits (binary, octal, decimal, hexadecimal).
 *
 * \details To use see #UNDERSCORE_DIGITS_LEN_BIN, #UNDERSCORE_DIGITS_LEN_OCT,
 * #UNDERSCORE_DIGITS_LEN_DEC, #UNDERSCORE_DIGITS_LEN_HEX.
 */
#define UNDERSCORE_DIGITS &__underscore_digits_letters[0]
/**
 * \brief `[_0-1]`.
 *
 * \details See #UNDERSCORE_DIGITS.
 */
#define UNDERSCORE_DIGITS_LEN_BIN 2
/**
 * \brief `[_0-7]`.
 *
 * \details See #UNDERSCORE_DIGITS.
 */
#define UNDERSCORE_DIGITS_LEN_OCT 3
/**
 * \brief `[_0-9]`.
 *
 * \details See #UNDERSCORE_DIGITS.
 */
#define UNDERSCORE_DIGITS_LEN_DEC 4
/**
 * \brief `[_0-9A-Fa-f]`.
 *
 * \details See #UNDERSCORE_DIGITS.
 */
#define UNDERSCORE_DIGITS_LEN_HEX 6

/**
 * \brief Digits and letters (lower and upper case).
 *
 * \details To use see #DIGITS_LETTERS_LEN.
 */
#define DIGITS_LETTERS &__underscore_digits_letters[1]
/**
 * \brief `[0-9A-Za-z]`.
 *
 * \details See #DIGITS_LETTERS.
 */
#define DIGITS_LETTERS_LEN 7

/**
 * \brief Underscore (`_`), digits and letters (lower and upper case).
 *
 * \details To use see #UNDERSCORE_DIGITS_LETTERS_LEN.
 */
#define UNDERSCORE_DIGITS_LETTERS &__underscore_digits_letters[0]
/**
 * \brief `[_0-9A-Za-z]`.
 *
 * \details See #UNDERSCORE_DIGITS_LETTERS.
 */
#define UNDERSCORE_DIGITS_LETTERS_LEN 8

#define M3C_InRange_LETTER(cp) (M3C_InRange((cp), 'A', 'Z') || M3C_InRange((cp), 'a', 'z'))

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
 * \brief Checks if the given `cp` is in the given `ranges`.
 *
 * \param[in] ranges    pointer the the range buffer
 * \param     rangesLen number of ranges in the buffer
 * \param     cp        code point
 */
m3c_bool match(M3C_ASM_ASCIIRange const *ranges, m3c_u8 rangesLen, M3C_UCP cp) {
    M3C_ASM_ASCIIRange const *afterEnd = ranges + rangesLen;
    for (; ranges < afterEnd; ++ranges) {
        if (M3C_InRange(cp, ranges->lo, ranges->hi))
            return m3c_true;
    }
    return m3c_false;
}

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
 * \brief Reads the document while each code point is in the given `ranges`.
 *
 * \param[in,out] lexer     lexer
 * \param[in]     ranges    pointer to the first range to check against
 * \param[in]     rangesLen number of `ranges`
 * \param[out]    n         writes here the number of read code points
 * \param[in]     maxN      maximum number of code points
 * \return
 * + #M3C_ERROR_OK - if after the matching sequence (which can have len=0) there is at least one
 * valid code point
 * + #M3C_ERROR_INVALID_ENCODING - if after the matching sequence there is invalid bytes
 * + #M3C_ERROR_EOF - if EOF was reached
 */
M3C_ERROR
__M3C_ASM_lexWhile(
    M3C_ASM_Lexer *lexer, M3C_ASM_ASCIIRange const *ranges, m3c_u8 rangesLen, m3c_size_t *n,
    m3c_size_t maxN
) {
    __VAR_DECL_WITHOUT_N;

    *n = 0;
    while (*n < maxN) {
        PEEK;

        if (status == M3C_ERROR_OK) {
            if (match(ranges, rangesLen, cp)) {
                ++*n;
                ADVANCE;
                continue;
            } else
                return status;
        } else
            return status;
    }
    return M3C_ERROR_OK;
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
            if (cp == '\n' || cp == '\r' ||  /* EOL */
                cp == '\t' || cp == ' ' ||   /* whitespaces */
                cp == ';' ||                 /* comment */
                M3C_InRange(cp, '0', '9') || /* number */
                M3C_InRange_LETTER(cp) ||    /* symbol */
                cp == '_' ||                 /* symbol */
                cp == '"' ||                 /* string */
                cp == '(' || cp == ')' ||    /* parentheses */
                cp == ',' ||                 /* comma */
                cp == ':' ||                 /* colon */
                cp == '+' ||                 /* plus */
                cp == '-' ||                 /* minus */
                cp == '*' ||                 /* star */
                cp == '/' ||                 /* slash */
                cp == '%' ||                 /* percent */
                cp == '~' ||                 /* tilde */
                cp == '&' ||                 /* amp */
                cp == '|' ||                 /* pipe */
                cp == '^' ||                 /* caret */
                cp == '<' ||                 /* less, ltlt, lessequal */
                cp == '=' ||                 /* equalequal */
                cp == '>' ||                 /* greater, gtgt, greaterequal */
                cp == '!' ||                 /* exclaimequal */
                cp == '?'                    /* question */
            )
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
            if (cp == '\n' || cp == '\r')
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
 * \brief Lexes `[_0-9A-Za-z]` immediately after the number literal.
 *
 * \details Assumes that the number literal is already invalid, so there is no need to emit any
 * additional diagnostics. The goal is to only locate EOT and push the `token`.
 *
 * \param[in,out] lexer lexer
 * \param[in,out] token token
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexNumberUntilEnd(M3C_ASM_Lexer *lexer, M3C_ASM_Token *token) {
    VAR_DECL;

    token->kind = M3C_ASM_TOKEN_KIND_UNRECOGNIZED;

    /* NOTE: ignore status and n */
    __M3C_ASM_lexWhile(
        lexer, UNDERSCORE_DIGITS_LETTERS, UNDERSCORE_DIGITS_LETTERS_LEN, &n, M3C_ASM_Token_MAX_CLEN
    );

    TOK_END(token);

    if (M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, token) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;
    return M3C_ERROR_OK;
}

/**
 * \brief Lexes the number body.
 *
 * \details Lexes the `[_\d]` part of the number literal, where the actual `\d` is specified by
 * `rangesLen`. Also takes into account if there is some `[_0-9A-Za-z]` right after the `[_\d]`
 * part, which is illegal.
 *
 * Diagnostics:
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_DIGIT_FOR_THIS_BASE_PREFIX
 * "INVALID_DIGIT_FOR_THIS_BASE_PREFIX"
 *
 * \param[in,out] lexer     lexer
 * \param[in,out] token     token
 * \param         rangesLen one of the #UNDERSCORE_DIGITS lengths
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexNumberBody(M3C_ASM_Lexer *lexer, M3C_ASM_Token *token, m3c_u8 rangesLen) {
    VAR_DECL;

    M3C_Diagnostic diagInvalidDigitForThisBasePrefix;
    diagInvalidDigitForThisBasePrefix.severity = M3C_SEVERITY_ERROR;
    diagInvalidDigitForThisBasePrefix.info =
        &M3C_ASM_DIAGNOSTIC_INFO_INVALID_DIGIT_FOR_THIS_BASE_PREFIX;

    /**
     * NOTE: we intentionally ignore:
     *     1. status of lexWhile
     *     2. `n`
     */
    __M3C_ASM_lexWhile(lexer, UNDERSCORE_DIGITS, rangesLen, &n, M3C_ASM_Token_MAX_CLEN);

    DIAG_START_FROM_LEXER(&diagInvalidDigitForThisBasePrefix);

    /**
     * Maybe there are still some `[_0-9A-Za-z]` left which is not a valid digits. In this
     * case, the number literal is no longer valid and we must continue to the end of the
     * sequence.
     *
     * NOTE: we intentionally ignore status of lexWhile.
     */
    __M3C_ASM_lexWhile(
        lexer, UNDERSCORE_DIGITS_LETTERS, UNDERSCORE_DIGITS_LETTERS_LEN, &n, M3C_ASM_Token_MAX_CLEN
    );
    if (n != 0) {
        token->kind = M3C_ASM_TOKEN_KIND_UNRECOGNIZED;

        diagInvalidDigitForThisBasePrefix.data.ASM.end =
            diagInvalidDigitForThisBasePrefix.data.ASM.start;
        ++diagInvalidDigitForThisBasePrefix.data.ASM.end.character;

        if (M3C_VEC_PUSH(
                M3C_Diagnostic, &lexer->diagnostics->vec, &diagInvalidDigitForThisBasePrefix
            ) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;
    } else
        token->kind = M3C_ASM_TOKEN_KIND_NUMBER;

    TOK_END(token);

    if (M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, token) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;
    return M3C_ERROR_OK;
}

/**
 * \brief Lexer the part of number literal right after the base prefix.
 *
 * \details
 * Diagnostics:
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_BASE_PREFIX "INVALID_BASE_PREFIX"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE
 * "DIGIT_SEPARATOR_CANNOT_APPEAR_HERE"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_DIGIT_FOR_THIS_BASE_PREFIX
 * "INVALID_DIGIT_FOR_THIS_BASE_PREFIX"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT
 * "NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_LEADING_ZEROS_ARE_NOT_PERMITTED
 * "LEADING_ZEROS_ARE_NOT_PERMITTED"
 *
 * \param[in,out] lexer                      lexer
 * \param[in,out] token                      token
 * \param         digitRangeLen              one of the #DIGITS lengths
 * \param         underscoreAndDigitRangeLen one of the #UNDERSCORE_DIGITS lengths
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexNumberAfterPrefix(
    M3C_ASM_Lexer *lexer, M3C_ASM_Token *token, m3c_u8 digitRangeLen,
    m3c_u8 underscoreAndDigitRangeLen
) {
    VAR_DECL;
    M3C_Diagnostic diagUnknownBasePrefix;
    M3C_Diagnostic diagDigitSeparatorCannotAppearHere;
    M3C_Diagnostic diagInvalidDigitForThisBasePrefix;
    M3C_Diagnostic diagNumberLiteralMustContainAtLeastOneDigit;
    M3C_Diagnostic diagLeadingZerosAreNotPermitted;

    diagUnknownBasePrefix.severity = M3C_SEVERITY_ERROR;
    diagUnknownBasePrefix.info = &M3C_ASM_DIAGNOSTIC_INFO_INVALID_BASE_PREFIX;
    diagDigitSeparatorCannotAppearHere.severity = M3C_SEVERITY_ERROR;
    diagDigitSeparatorCannotAppearHere.info =
        &M3C_ASM_DIAGNOSTIC_INFO_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE;
    diagInvalidDigitForThisBasePrefix.severity = M3C_SEVERITY_ERROR;
    diagInvalidDigitForThisBasePrefix.info =
        &M3C_ASM_DIAGNOSTIC_INFO_INVALID_DIGIT_FOR_THIS_BASE_PREFIX;
    diagNumberLiteralMustContainAtLeastOneDigit.severity = M3C_SEVERITY_ERROR;
    diagNumberLiteralMustContainAtLeastOneDigit.info =
        &M3C_ASM_DIAGNOSTIC_INFO_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT;
    diagLeadingZerosAreNotPermitted.severity = M3C_SEVERITY_ERROR;
    diagLeadingZerosAreNotPermitted.info = &M3C_ASM_DIAGNOSTIC_INFO_LEADING_ZEROS_ARE_NOT_PERMITTED;

    PEEK;
    /**
     * We first handle negative cases for the first char after the base prefix:
     *   1. when there is '_' right after the base prefix
     *   2. `[0-9A-Za-z]` but not a valid digit
     *   3. not a `[0-9A-Za-z]` (hence not a valid digit) or (status == EOF or
     * INVALID_ENCODING)
     */
    if (status == M3C_ERROR_OK && cp == '_') {
        DIAG_START_FROM_LEXER(&diagDigitSeparatorCannotAppearHere);
        ADVANCE;
        DIAG_END(&diagDigitSeparatorCannotAppearHere);

        if (M3C_VEC_PUSH(
                M3C_Diagnostic, &lexer->diagnostics->vec, &diagDigitSeparatorCannotAppearHere
            ) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;

        return __M3C_ASM_lexNumberUntilEnd(lexer, token);

    } else if (status == M3C_ERROR_OK && match(DIGITS_LETTERS, DIGITS_LETTERS_LEN, cp) && !match(DIGITS, digitRangeLen, cp)) {
        DIAG_START_FROM_LEXER(&diagInvalidDigitForThisBasePrefix);
        ADVANCE;
        DIAG_END(&diagInvalidDigitForThisBasePrefix);

        if (M3C_VEC_PUSH(
                M3C_Diagnostic, &lexer->diagnostics->vec, &diagInvalidDigitForThisBasePrefix
            ) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;

        return __M3C_ASM_lexNumberUntilEnd(lexer, token);

    } else if ((status == M3C_ERROR_OK && !match(DIGITS, digitRangeLen, cp)) || status != M3C_ERROR_OK) {
        DIAG_START_FROM_TOKEN(&diagNumberLiteralMustContainAtLeastOneDigit, token);
        DIAG_END(&diagNumberLiteralMustContainAtLeastOneDigit);

        if (M3C_VEC_PUSH(
                M3C_Diagnostic, &lexer->diagnostics->vec,
                &diagNumberLiteralMustContainAtLeastOneDigit
            ) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;

        return __M3C_ASM_lexNumberUntilEnd(lexer, token);
    }

    /**
     * HACK: to be honest we need to parse two times:
     *     1. `[\d]{1}`
     *     2. `[_\d]*`
     * but we already handled case when '_' is located right after the base prefix so we can
     * just lexWhile `[_\d]*`.
     */
    return __M3C_ASM_lexNumberBody(lexer, token, underscoreAndDigitRangeLen);
}

/**
 * \brief Lexes the part of number literal right after the `0` (if `0` is the first code point of
 * this literal).
 *
 * \details Diagnostics:
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_BASE_PREFIX "INVALID_BASE_PREFIX"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE
 * "DIGIT_SEPARATOR_CANNOT_APPEAR_HERE"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_DIGIT_FOR_THIS_BASE_PREFIX
 * "INVALID_DIGIT_FOR_THIS_BASE_PREFIX"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT
 * "NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_LEADING_ZEROS_ARE_NOT_PERMITTED
 * "LEADING_ZEROS_ARE_NOT_PERMITTED"
 *
 * \warning It is not guaranteed to return #M3C_ERROR_EOF if EOF is reached after the token.
 *
 * \param[in,out] lexer lexer
 * \param[in,out] token token
 * \return
 * + #M3C_ERROR_OK - OK or EOF is reached
 * + #M3C_ERROR_EOF - if EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexZero(M3C_ASM_Lexer *lexer, M3C_ASM_Token *token) {
    VAR_DECL;
    m3c_u8 digitsLen;
    m3c_u8 underscoreDigitsLen;

    M3C_Diagnostic diagUnknownBasePrefix;
    M3C_Diagnostic diagDigitSeparatorCannotAppearHere;
    M3C_Diagnostic diagInvalidDigitForThisBasePrefix;
    M3C_Diagnostic diagNumberLiteralMustContainAtLeastOneDigit;
    M3C_Diagnostic diagLeadingZerosAreNotPermitted;

    diagUnknownBasePrefix.severity = M3C_SEVERITY_ERROR;
    diagUnknownBasePrefix.info = &M3C_ASM_DIAGNOSTIC_INFO_INVALID_BASE_PREFIX;

    diagDigitSeparatorCannotAppearHere.severity = M3C_SEVERITY_ERROR;
    diagDigitSeparatorCannotAppearHere.info =
        &M3C_ASM_DIAGNOSTIC_INFO_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE;

    diagInvalidDigitForThisBasePrefix.severity = M3C_SEVERITY_ERROR;
    diagInvalidDigitForThisBasePrefix.info =
        &M3C_ASM_DIAGNOSTIC_INFO_INVALID_DIGIT_FOR_THIS_BASE_PREFIX;

    diagNumberLiteralMustContainAtLeastOneDigit.severity = M3C_SEVERITY_ERROR;
    diagNumberLiteralMustContainAtLeastOneDigit.info =
        &M3C_ASM_DIAGNOSTIC_INFO_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT;

    diagLeadingZerosAreNotPermitted.severity = M3C_SEVERITY_ERROR;
    diagLeadingZerosAreNotPermitted.info = &M3C_ASM_DIAGNOSTIC_INFO_LEADING_ZEROS_ARE_NOT_PERMITTED;

    PEEK; /* re-peek '0' */
    ADVANCE;

    /*
     * Peek code point right after the '0'. Valid options:
     *   1. base prefix `[bByYoOqQdDxXhH]`
     *   2. SOT or whitespaces (hence leading zero are not permitted)
     */
    PEEK;
    if (status == M3C_ERROR_EOF) {
        token->kind = M3C_ASM_TOKEN_KIND_NUMBER;
        TOK_END(token);
        if (M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, token) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;

        return M3C_ERROR_EOF;
    }

    if (cp == 'b' || cp == 'B' || cp == 'y' || cp == 'Y') {
        /* binary */

        digitsLen = DIGITS_LEN_BIN;
        underscoreDigitsLen = UNDERSCORE_DIGITS_LEN_BIN;

        ADVANCE;
        return __M3C_ASM_lexNumberAfterPrefix(lexer, token, digitsLen, underscoreDigitsLen);
    } else if (cp == 'o' || cp == 'O' || cp == 'q' || cp == 'Q') {
        /* octal */

        digitsLen = DIGITS_LEN_OCT;
        underscoreDigitsLen = UNDERSCORE_DIGITS_LEN_OCT;

        ADVANCE;
        return __M3C_ASM_lexNumberAfterPrefix(lexer, token, digitsLen, underscoreDigitsLen);
    } else if (cp == 'd' || cp == 'D') {
        /* decimal */

        digitsLen = DIGITS_LEN_DEC;
        underscoreDigitsLen = UNDERSCORE_DIGITS_LEN_DEC;

        ADVANCE;
        return __M3C_ASM_lexNumberAfterPrefix(lexer, token, digitsLen, underscoreDigitsLen);
    } else if (cp == 'x' || cp == 'X' || cp == 'h' || cp == 'H') {
        /* hex */

        digitsLen = DIGITS_LEN_HEX;
        underscoreDigitsLen = UNDERSCORE_DIGITS_LEN_HEX;

        ADVANCE;
        return __M3C_ASM_lexNumberAfterPrefix(lexer, token, digitsLen, underscoreDigitsLen);
    } else if ((cp >= '0' && cp <= '9') || cp == '_') {
        DIAG_START_FROM_LEXER(&diagLeadingZerosAreNotPermitted);
        ADVANCE;
        DIAG_END(&diagLeadingZerosAreNotPermitted);

        if (M3C_VEC_PUSH(
                M3C_Diagnostic, &lexer->diagnostics->vec, &diagLeadingZerosAreNotPermitted
            ) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;

        return __M3C_ASM_lexNumberUntilEnd(lexer, token);

    } else if (M3C_InRange_LETTER(cp)) {

        DIAG_START_FROM_LEXER(&diagUnknownBasePrefix);
        ADVANCE;
        DIAG_END(&diagUnknownBasePrefix);

        if (M3C_VEC_PUSH(M3C_Diagnostic, &lexer->diagnostics->vec, &diagUnknownBasePrefix) !=
            M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;

        return __M3C_ASM_lexNumberUntilEnd(lexer, token);

    } else {
        /* it was decimal `0` and now the new token started or just whitespace
         * status == OK or INVALID_ENCODING */
        token->kind = M3C_ASM_TOKEN_KIND_NUMBER;
        TOK_END(token);

        if (M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, token) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        return M3C_ERROR_OK;
    }
}

/**
 * \brief Lexes the part of \ref M3C_ASM_TOKEN_KIND_STRING "string literal" where an invalid
 * character sequence is.
 *
 * \details Diagnostics:
 * + (required) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_CHARACTERS_IN_STRING_LITERAL
 * "INVALID_CHARACTERS_IN_STRING_LITERAL"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING"
 *
 * \param[in,out] lexer lexer
 * \param[in,out] token token
 * \return
 * + #M3C_ERROR_OK - OK or EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexStringInvalidCharacters(M3C_ASM_Lexer *lexer, M3C_ASM_Token *token) {
    VAR_DECL;

    M3C_Diagnostic diagInvalidCharacterInStringLiteral;
    M3C_Diagnostic diagInvalidEncoding;

    /* we need to remember diagnostic index as we may need to add diagInvalidEncoding diagnostics */
    m3c_size_t invalidCharacterInStringLiteralDiagIndex;

    diagInvalidCharacterInStringLiteral.severity = M3C_SEVERITY_ERROR;
    diagInvalidCharacterInStringLiteral.info =
        &M3C_ASM_DIAGNOSTIC_INFO_INVALID_CHARACTERS_IN_STRING_LITERAL;

    diagInvalidEncoding.severity = M3C_SEVERITY_ERROR;
    diagInvalidEncoding.info = &M3C_ASM_DIAGNOSTIC_INFO_INVALID_ENCODING;

    token->kind = M3C_ASM_TOKEN_KIND_UNRECOGNIZED;

    DIAG_START_FROM_LEXER(&diagInvalidCharacterInStringLiteral);

    /* push diagnostic and save its index */
    invalidCharacterInStringLiteralDiagIndex = lexer->diagnostics->vec.len;
    if (M3C_VEC_PUSH(
            M3C_Diagnostic, &lexer->diagnostics->vec, &diagInvalidCharacterInStringLiteral
        ) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;
    ++lexer->diagnostics->errors;

    M3C_LOOP {
        PEEK;

        if (status == M3C_ERROR_EOF ||
            (status == M3C_ERROR_OK &&
             (cp == '\n' || cp == '\r' ||                           /* EOL (and EOT with warning) */
              cp == '"' ||                                          /* EOT */
              (M3C_InRange(cp, '0', '9') || M3C_InRange_LETTER(cp)) /* valid code points */
             )))
            break;
        else if (status == M3C_ERROR_OK) { /* just an invalid character */
            ADVANCE;
            continue;
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

    DIAG_END(&lexer->diagnostics->vec.data[invalidCharacterInStringLiteralDiagIndex]);

    return M3C_ERROR_OK;
}

/**
 * \brief Lexes the \ref M3C_ASM_TOKEN_KIND_STRING "string literal".
 *
 * \details Diagnostics:
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_CHARACTERS_IN_STRING_LITERAL
 * "INVALID_CHARACTERS_IN_STRING_LITERAL"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_UNTERMINATED_STRING_LITERAL "UNTERMINATED_STRING_LITERAL"
 *
 * \warning It is not guaranteed to return #M3C_ERROR_EOF if EOF is reached after the token.
 *
 * \param[in,out] lexer lexer
 * \param[in,out] token token
 * \return
 * + #M3C_ERROR_OK - OK or EOF is reached
 * + #M3C_ERROR_EOF - if EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexString(M3C_ASM_Lexer *lexer, M3C_ASM_Token *token) {
    VAR_DECL;

    M3C_Diagnostic diagUnterminatedStringLiteral;
    diagUnterminatedStringLiteral.severity = M3C_SEVERITY_WARNING;
    diagUnterminatedStringLiteral.info = &M3C_ASM_DIAGNOSTIC_INFO_UNTERMINATED_STRING_LITERAL;

    token->kind = M3C_ASM_TOKEN_KIND_STRING;

    PEEK; /* re-peek '"' */
    ADVANCE;

    M3C_LOOP {
        PEEK;

        if (status == M3C_ERROR_OK && (M3C_InRange(cp, '0', '9') || M3C_InRange_LETTER(cp))) {
            ADVANCE;
            continue;

        } else if (status == M3C_ERROR_OK && cp == '"') {
            ADVANCE;
            TOK_END(token);

            if (M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, token) != M3C_ERROR_OK)
                return M3C_ERROR_OOM;
            return M3C_ERROR_OK;

        } else if (status == M3C_ERROR_EOF || (status == M3C_ERROR_OK && (cp == '\n' || cp == '\r'))) {
            DIAG_START_FROM_TOKEN(&diagUnterminatedStringLiteral, token);
            DIAG_END(&diagUnterminatedStringLiteral);

            if (M3C_VEC_PUSH(
                    M3C_Diagnostic, &lexer->diagnostics->vec, &diagUnterminatedStringLiteral
                ) != M3C_ERROR_OK)
                return M3C_ERROR_OOM;
            ++lexer->diagnostics->warnings;

            TOK_END(token);
            if (M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, token) != M3C_ERROR_OK)
                return M3C_ERROR_OOM;
            return status; /* can be OK and EOF */

        } else {
            /**
             * NOTE: (status == INVALID_ENCODING) OR (status == OK and cp !=
             *   1. [0-9A-Za-z]
             *   2. '"'
             *   3. EOL
             *   4. '\\'
             * )
             */

            status = __M3C_ASM_lexStringInvalidCharacters(lexer, token);
            if (status != M3C_ERROR_OK)
                return status;

            continue;
        }
    }
}

/**
 * \brief Lexes the \ref M3C_ASM_TOKEN_KIND_SYMBOL "symbol" token.
 *
 * \param[in,out] lexer lexer
 * \param[in,out] token token
 * \return
 * + #M3C_ERROR_OK - OK or EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexSymbol(M3C_ASM_Lexer *lexer, M3C_ASM_Token *token) {
    VAR_DECL;

    PEEK; /* re-peek first code point - [_A-Za-z] */
    ADVANCE;

    token->kind = M3C_ASM_TOKEN_KIND_SYMBOL;

    M3C_LOOP {
        PEEK;

        if (status == M3C_ERROR_OK &&
            (cp == '_' || M3C_InRange(cp, '0', '9') || M3C_InRange_LETTER(cp))) {
            ADVANCE;
            continue;
        }

        break;
    }
    TOK_END(token);

    return M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, token);
}

#define ONE_CHAR_TOKEN(ch, tokenKind)                                                              \
    (cp == (ch)) {                                                                                 \
        token.kind = (tokenKind);                                                                  \
        goto one_char_token;                                                                       \
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
        token.kind = M3C_ASM_TOKEN_KIND_EOL;
        ADVANCE_NL;
        TOK_END(&token);
        return M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, &token);
    } else if (cp == '\r') {
        token.kind = M3C_ASM_TOKEN_KIND_EOL;
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '\n') { /* \r ~ \n */
            ADVANCE_NL;
            TOK_END(&token);
            return M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, &token);
        } else {
            __ADVANCE_ONLY_POS_NL;
            TOK_END(&token);
            if (M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, &token) != M3C_ERROR_OK)
                return M3C_ERROR_OOM;
            return status;
        }
    } else if (cp == '!') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '=') {
            token.kind = M3C_ASM_TOKEN_KIND_EXCLAIMEQUAL;
            goto one_char_token;
        } else
            return __M3C_ASM_lexUnrecognisedToken(lexer, &token);
    } else if (cp == '"')
        return __M3C_ASM_lexString(lexer, &token);
    else if
        ONE_CHAR_TOKEN('%', M3C_ASM_TOKEN_KIND_PERCENT)
    else if
        ONE_CHAR_TOKEN('&', M3C_ASM_TOKEN_KIND_AMP)
    else if
        ONE_CHAR_TOKEN('(', M3C_ASM_TOKEN_KIND_L_PAREN)
    else if
        ONE_CHAR_TOKEN(')', M3C_ASM_TOKEN_KIND_R_PAREN)
    else if
        ONE_CHAR_TOKEN('*', M3C_ASM_TOKEN_KIND_STAR)
    else if
        ONE_CHAR_TOKEN('+', M3C_ASM_TOKEN_KIND_PLUS)
    else if
        ONE_CHAR_TOKEN(',', M3C_ASM_TOKEN_KIND_COMMA)
    else if
        ONE_CHAR_TOKEN('-', M3C_ASM_TOKEN_KIND_MINUS)
    else if
        ONE_CHAR_TOKEN('/', M3C_ASM_TOKEN_KIND_SLASH)
    else if (cp == '0')
        return __M3C_ASM_lexZero(lexer, &token);
    else if (cp >= '1' && cp <= '9') {
        ADVANCE;
        return __M3C_ASM_lexNumberBody(lexer, &token, UNDERSCORE_DIGITS_LEN_DEC);
    } else if
        ONE_CHAR_TOKEN (':', M3C_ASM_TOKEN_KIND_COLON)
    else if (cp == ';')
        return __M3C_ASM_lexCommentToken(lexer, &token);
    else if (cp == '<') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '<') {
            token.kind = M3C_ASM_TOKEN_KIND_LTLT;
            goto one_char_token;
        } else if (status == M3C_ERROR_OK && cp == '=') {
            token.kind = M3C_ASM_TOKEN_KIND_LESSEQUAL;
            goto one_char_token;
        } else {
            token.kind = M3C_ASM_TOKEN_KIND_LESS;
            TOK_END(&token);
            return M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, &token);
        }
    } else if (cp == '=') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '=') {
            token.kind = M3C_ASM_TOKEN_KIND_EQUALEQUAL;
            goto one_char_token;
        } else
            return __M3C_ASM_lexUnrecognisedToken(lexer, &token);
    } else if (cp == '>') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '>') {
            token.kind = M3C_ASM_TOKEN_KIND_GTGT;
            goto one_char_token;
        } else if (status == M3C_ERROR_OK && cp == '=') {
            token.kind = M3C_ASM_TOKEN_KIND_GREATEREQUAL;
            goto one_char_token;
        } else {
            token.kind = M3C_ASM_TOKEN_KIND_GREATER;
            TOK_END(&token);
            return M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, &token);
        }
    } else if ONE_CHAR_TOKEN ('?', M3C_ASM_TOKEN_KIND_QUESTION)
        else if (cp == '_' || M3C_InRange_LETTER(cp)) return __M3C_ASM_lexSymbol(lexer, &token);
    else if
        ONE_CHAR_TOKEN('^', M3C_ASM_TOKEN_KIND_CARET)
    else if
        ONE_CHAR_TOKEN('|', M3C_ASM_TOKEN_KIND_PIPE)
    else if
        ONE_CHAR_TOKEN('~', M3C_ASM_TOKEN_KIND_TILDE)
    else
        return __M3C_ASM_lexUnrecognisedToken(lexer, &token);

/* Not for EOL */
one_char_token:
    ADVANCE;
    TOK_END(&token);
    return M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, &token);
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

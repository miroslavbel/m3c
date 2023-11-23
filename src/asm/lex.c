#include <m3c/asm/lex.h>

#include <m3c/common/macros.h>
#include <m3c/common/utf8.h>
#include <m3c/rt/alloc.h>
#include <m3c/asm/diagnostics_info.h>
#include <m3c/asm/preproc.h>

#define __ADVANCE_ONLY_PTR lexer->ptr += cpLen
#define __ADVANCE_ONLY_PTR2 lexer->ptr2 += cpLen

#define __ADVANCE_ONLY_POS ++lexer->pos.character
#define __ADVANCE_ONLY_POS2 ++lexer->pos2.character
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
 * @brief As \ref PEEK "PEEK" but is used to reread document using `ptr2`, `pos2`, `fragment2`.
 */
#define PEEK2                                                                                      \
    status = __M3C_ASM_Lexer_peek2(                                                                \
        &lexer->ptr2, &lexer->pos2, &lexer->fragment2, lexer->fragmentLast, &cp, &cpLen            \
    )

/**
 * \brief Moves the lexer forward, assuming the lexer doesn't point to a newline character.
 *
 * \warning Requires macro #PEEK to be called before it.
 */
#define ADVANCE                                                                                    \
    __ADVANCE_ONLY_PTR;                                                                            \
    __ADVANCE_ONLY_POS
/**
 * \brief As \ref ADVANCE "ADVANCE" but is used to reread document using `ptr2`, `pos2`,
 * `fragment2`.
 *
 * \warning Requires macro #PEEK2 to be called before it.
 */
#define ADVANCE2                                                                                   \
    __ADVANCE_ONLY_PTR2;                                                                           \
    __ADVANCE_ONLY_POS2
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
#define TOK_START                                                                                  \
    lexer->token.ptr = lexer->ptr;                                                                 \
    lexer->token.start = lexer->pos;                                                               \
    lexer->token.lexeme.hStr = 0;                                                                  \
    /* save info for token rereading */                                                            \
    lexer->ptr2 = lexer->ptr;                                                                      \
    lexer->pos2 = lexer->pos;                                                                      \
    lexer->fragment2 = lexer->fragment
/**
 * \brief Sets the end position of the token.
 *
 * \details The lexer must point to the next character after the token.
 */
#define TOK_END lexer->token.end = lexer->pos
/**
 * \brief Sets the kind of the token.
 */
#define TOK_KIND(KIND) lexer->token.kind = KIND
/**
 * \brief Pushes the token.
 *
 * \return
 * + M3C_ERROR_OK
 * + M3C_ERROR_OOM - if failed to realloc
 */
#define TOK_PUSH M3C_VEC_PUSH(M3C_ASM_Token, lexer->tokens, &lexer->token)

/**
 * \brief Push string to the preproc's stringPool.
 *
 * \return
 * + M3C_ERROR_OK
 * + M3C_ERROR_OOM - if failed to realloc
 */
#define STR_PUSH(str) M3C_VEC_PUSH(M3C_ASM_CachedString, lexer->stringPool, (str))

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
#define DIAG_START_FROM_TOKEN(diag)                                                                \
    (diag)->data.ASM.start = lexer->token.start;                                                   \
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

#define M3C_InRange_DIGIT_HEX(cp)                                                                  \
    (M3C_InRange(cp, '0', '9') || M3C_InRange(cp, 'A', 'F') || M3C_InRange(cp, 'a', 'f'))

#define M3C_InRange_PRINTABLE(cp) (M3C_InRange(cp, ' ', '~'))

#define M3C_GetHexVal(cp) M3C_InRange(cp, '0', '9') ? cp - '0' : (cp & 0x1F) + 9

#define M3C_BIN_PREFIX(cp) ((cp) == 'b' || (cp) == 'B' || (cp) == 'y' || (cp) == 'Y')
#define M3C_OCT_PREFIX(cp) ((cp) == 'o' || (cp) == 'O' || (cp) == 'q' || (cp) == 'Q')
#define M3C_DEC_PREFIX(cp) ((cp) == 'd' || (cp) == 'D')
#define M3C_HEX_PREFIX(cp) ((cp) == 'x' || (cp) == 'X' || (cp) == 'h' || (cp) == 'H')

typedef struct __tagM3C_ASM_Lexer {
    m3c_u8 const *ptr;
    M3C_ASM_Position pos;
    m3c_u8 const *bLast;
    M3C_ASM_Fragment *fragment;
    M3C_ASM_Fragment *fragmentLast;
    /**
     * \brief Actual token.
     */
    M3C_ASM_Token token;
    M3C_ASM_Tokens *tokens;
    M3C_Diagnostics *diagnostics;
    /**
     * \brief Pointer to the first byte of the actual token.
     */
    m3c_u8 const *ptr2;
    /**
     * \brief Position of the first character of the actual token.
     */
    M3C_ASM_Position pos2;
    /**
     * \brief Fragment, where the actual token has started.
     */
    M3C_ASM_Fragment *fragment2;
    M3C_hINCLUDE hInclude;
    /**
     * \brief Preproc's stringPool.
     */
    M3C_ASM_StringPool *stringPool;
    /**
     * \brief Pointer to the terminating quote or `NULL` (if there is no any).
     */
    m3c_u8 const *terminatingQuotePtr;
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

    /* looking for the next non-empty fragment */
    M3C_LOOP {
        if (lexer->fragment == lexer->fragmentLast)
            return M3C_ERROR_EOF;
        ++lexer->fragment;

        if (lexer->fragment->bLast != M3C_NULL)
            break;
    }

    lexer->ptr = lexer->fragment->bFirst;
    lexer->pos = lexer->fragment->pos;
    return M3C_UTF8GetASCIICodepointWithLen(lexer->ptr, lexer->fragment->bLast, cp, cpLen);
}

M3C_ERROR __M3C_ASM_Lexer_peek2(
    const m3c_u8 **ptr, M3C_ASM_Position *pos, M3C_ASM_Fragment **fragment,
    const M3C_ASM_Fragment *lastFragment, M3C_UCP *cp, m3c_size_t *cpLen
) {
    if (*ptr <= (*fragment)->bLast)
        /* if it's not the end of the fragment just read the next char */
        return M3C_UTF8ReadCodepointWithLen(*ptr, (*fragment)->bLast, cp, cpLen);

    /* looking for the next non-empty fragment */
    M3C_LOOP {
        if (*fragment == lastFragment)
            return M3C_ERROR_EOF;
        ++(*fragment);

        if ((*fragment)->bLast != M3C_NULL)
            break;
    }

    *ptr = (*fragment)->bFirst;
    *pos = (*fragment)->pos;
    return M3C_UTF8ReadCodepointWithLen(*ptr, (*fragment)->bLast, cp, cpLen);
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
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_EOF - if EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexUnrecognisedToken(M3C_ASM_Lexer *lexer) {
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

    DIAG_START_FROM_TOKEN(&diagInvalidToken);

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
                cp == '&' ||                 /* amp, ampamp */
                cp == '|' ||                 /* pipe, pipepipe */
                cp == '^' ||                 /* caret */
                cp == '<' ||                 /* less, ltlt, lessequal */
                cp == '=' ||                 /* equalequal */
                cp == '>' ||                 /* greater, gtgt, greaterequal */
                cp == '!' ||                 /* exclaim, exclaimequal */
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

    TOK_KIND(M3C_ASM_TOKEN_KIND_UNRECOGNIZED);
    TOK_END;

    if (TOK_PUSH != M3C_ERROR_OK)
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
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_EOF - if EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexCommentToken(M3C_ASM_Lexer *lexer) {
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

    TOK_KIND(M3C_ASM_TOKEN_KIND_COMMENT);
    TOK_END;

    if (TOK_PUSH != M3C_ERROR_OK)
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
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexNumberUntilEnd(M3C_ASM_Lexer *lexer) {
    VAR_DECL;

    TOK_KIND(M3C_ASM_TOKEN_KIND_UNRECOGNIZED);

    /* NOTE: ignore status and n */
    __M3C_ASM_lexWhile(
        lexer, UNDERSCORE_DIGITS_LETTERS, UNDERSCORE_DIGITS_LETTERS_LEN, &n, M3C_ASM_Token_MAX_CLEN
    );

    TOK_END;

    if (TOK_PUSH != M3C_ERROR_OK)
        return M3C_ERROR_OOM;
    return M3C_ERROR_OK;
}

/**
 * \brief Fills the lexeme of \ref M3C_ASM_TOKEN_KIND_NUMBER "number" token.
 *
 * \details Diagnostics:
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_NUMBER_CONSTANT_IS_TOO_LARGE
 * "NUMBER_CONSTANT_IS_TOO_LARGE"
 *
 * \param[in,out] lexer lexer
 * \return
 * + M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push diagnostic
 */
M3C_ERROR __M3C_ASM_lexemizeNumber(M3C_ASM_Lexer *lexer) {
    VAR_DECL;

    M3C_Diagnostic diagNumberConstantIsTooLarge;

    int base;
    m3c_bool isBaseSet;
    m3c_i32 maxValueBeforeBaseMultOverflow;

    int chDigitVal;

    diagNumberConstantIsTooLarge.severity = M3C_SEVERITY_ERROR;
    diagNumberConstantIsTooLarge.info = &M3C_ASM_DIAGNOSTIC_INFO_NUMBER_CONSTANT_IS_TOO_LARGE;

    base = 10;
    maxValueBeforeBaseMultOverflow = M3C_I32_MAX / base;
    isBaseSet = m3c_false;

    lexer->token.lexeme.num = 0;

    while (lexer->ptr2 < lexer->ptr) {
        PEEK2;

        /* if we haven't set base yet, check for base first */
        if (!isBaseSet) {
            if (M3C_BIN_PREFIX(cp)) {
                base = 2;
                maxValueBeforeBaseMultOverflow = M3C_I32_MAX / base;
                isBaseSet = m3c_true;
                ADVANCE2;
                continue;
            } else if (M3C_OCT_PREFIX(cp)) {
                base = 8;
                maxValueBeforeBaseMultOverflow = M3C_I32_MAX / base;
                isBaseSet = m3c_true;
                ADVANCE2;
                continue;
            } else if (M3C_DEC_PREFIX(cp)) {
                base = 10;
                maxValueBeforeBaseMultOverflow = M3C_I32_MAX / base;
                isBaseSet = m3c_true;
                ADVANCE2;
                continue;
            } else if (M3C_HEX_PREFIX(cp)) {
                base = 16;
                maxValueBeforeBaseMultOverflow = M3C_I32_MAX / base;
                isBaseSet = m3c_true;
                ADVANCE2;
                continue;
            }
        }

        /* HACK: as we assume that the token is valid NUMBER, so we can only encounter:
         *   1. zero (right before the base prefix)
         *   2. digit with the `base` base
         *   3. char '_' (which we can just ignore)
         */
        if (cp != '_') {
            if (lexer->token.lexeme.num > maxValueBeforeBaseMultOverflow)
                goto push_diag_and_return;
            lexer->token.lexeme.num *= base;

            chDigitVal = cp > '9' ? (cp & 0x1F) + 9 : cp - '0';
            if (lexer->token.lexeme.num > M3C_I32_MAX - chDigitVal)
                goto push_diag_and_return;
            lexer->token.lexeme.num += chDigitVal;
        }

        ADVANCE2;
    }

    return M3C_ERROR_OK;

push_diag_and_return:
    TOK_KIND(M3C_ASM_TOKEN_KIND_UNRECOGNIZED);
    lexer->token.lexeme.num = 0;

    DIAG_START_FROM_TOKEN(&diagNumberConstantIsTooLarge);
    DIAG_END(&diagNumberConstantIsTooLarge);

    if (M3C_VEC_PUSH(M3C_Diagnostic, &lexer->diagnostics->vec, &diagNumberConstantIsTooLarge) !=
        M3C_ERROR_OK)
        return M3C_ERROR_OOM;
    ++lexer->diagnostics->errors;

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
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_NUMBER_CONSTANT_IS_TOO_LARGE
 * "NUMBER_CONSTANT_IS_TOO_LARGE"
 *
 * \param[in,out] lexer     lexer
 * \param         rangesLen one of the #UNDERSCORE_DIGITS lengths
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexNumberBody(M3C_ASM_Lexer *lexer, m3c_u8 rangesLen) {
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
        TOK_KIND(M3C_ASM_TOKEN_KIND_UNRECOGNIZED);

        diagInvalidDigitForThisBasePrefix.data.ASM.end =
            diagInvalidDigitForThisBasePrefix.data.ASM.start;
        ++diagInvalidDigitForThisBasePrefix.data.ASM.end.character;

        if (M3C_VEC_PUSH(
                M3C_Diagnostic, &lexer->diagnostics->vec, &diagInvalidDigitForThisBasePrefix
            ) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;
    } else
        TOK_KIND(M3C_ASM_TOKEN_KIND_NUMBER);

    TOK_END;

    __M3C_ASM_lexemizeNumber(lexer);

    if (TOK_PUSH != M3C_ERROR_OK)
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
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_NUMBER_CONSTANT_IS_TOO_LARGE
 * "NUMBER_CONSTANT_IS_TOO_LARGE"
 *
 * \param[in,out] lexer                      lexer
 * \param         digitRangeLen              one of the #DIGITS lengths
 * \param         underscoreAndDigitRangeLen one of the #UNDERSCORE_DIGITS lengths
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexNumberAfterPrefix(
    M3C_ASM_Lexer *lexer, m3c_u8 digitRangeLen, m3c_u8 underscoreAndDigitRangeLen
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

        return __M3C_ASM_lexNumberUntilEnd(lexer);

    } else if (status == M3C_ERROR_OK && match(DIGITS_LETTERS, DIGITS_LETTERS_LEN, cp) && !match(DIGITS, digitRangeLen, cp)) {
        DIAG_START_FROM_LEXER(&diagInvalidDigitForThisBasePrefix);
        ADVANCE;
        DIAG_END(&diagInvalidDigitForThisBasePrefix);

        if (M3C_VEC_PUSH(
                M3C_Diagnostic, &lexer->diagnostics->vec, &diagInvalidDigitForThisBasePrefix
            ) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;

        return __M3C_ASM_lexNumberUntilEnd(lexer);

    } else if ((status == M3C_ERROR_OK && !match(DIGITS, digitRangeLen, cp)) || status != M3C_ERROR_OK) {
        DIAG_START_FROM_TOKEN(&diagNumberLiteralMustContainAtLeastOneDigit);
        DIAG_END(&diagNumberLiteralMustContainAtLeastOneDigit);

        if (M3C_VEC_PUSH(
                M3C_Diagnostic, &lexer->diagnostics->vec,
                &diagNumberLiteralMustContainAtLeastOneDigit
            ) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;

        return __M3C_ASM_lexNumberUntilEnd(lexer);
    }

    /**
     * HACK: to be honest we need to parse two times:
     *     1. `[\d]{1}`
     *     2. `[_\d]*`
     * but we already handled case when '_' is located right after the base prefix so we can
     * just lexWhile `[_\d]*`.
     */
    return __M3C_ASM_lexNumberBody(lexer, underscoreAndDigitRangeLen);
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
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_NUMBER_CONSTANT_IS_TOO_LARGE
 * "NUMBER_CONSTANT_IS_TOO_LARGE"
 *
 * \warning It is not guaranteed to return #M3C_ERROR_EOF if EOF is reached after the token.
 *
 * \param[in,out] lexer lexer
 * \return
 * + #M3C_ERROR_OK - OK or EOF is reached
 * + #M3C_ERROR_EOF - if EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexZero(M3C_ASM_Lexer *lexer) {
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
        TOK_KIND(M3C_ASM_TOKEN_KIND_NUMBER);
        TOK_END;
        if (TOK_PUSH != M3C_ERROR_OK)
            return M3C_ERROR_OOM;

        return M3C_ERROR_EOF;
    }

    if (M3C_BIN_PREFIX(cp)) {
        /* binary */

        digitsLen = DIGITS_LEN_BIN;
        underscoreDigitsLen = UNDERSCORE_DIGITS_LEN_BIN;

        ADVANCE;
        return __M3C_ASM_lexNumberAfterPrefix(lexer, digitsLen, underscoreDigitsLen);
    } else if (M3C_OCT_PREFIX(cp)) {
        /* octal */

        digitsLen = DIGITS_LEN_OCT;
        underscoreDigitsLen = UNDERSCORE_DIGITS_LEN_OCT;

        ADVANCE;
        return __M3C_ASM_lexNumberAfterPrefix(lexer, digitsLen, underscoreDigitsLen);
    } else if (M3C_DEC_PREFIX(cp)) {
        /* decimal */

        digitsLen = DIGITS_LEN_DEC;
        underscoreDigitsLen = UNDERSCORE_DIGITS_LEN_DEC;

        ADVANCE;
        return __M3C_ASM_lexNumberAfterPrefix(lexer, digitsLen, underscoreDigitsLen);
    } else if (M3C_HEX_PREFIX(cp)) {
        /* hex */

        digitsLen = DIGITS_LEN_HEX;
        underscoreDigitsLen = UNDERSCORE_DIGITS_LEN_HEX;

        ADVANCE;
        return __M3C_ASM_lexNumberAfterPrefix(lexer, digitsLen, underscoreDigitsLen);
    } else if ((cp >= '0' && cp <= '9') || cp == '_') {
        DIAG_START_FROM_LEXER(&diagLeadingZerosAreNotPermitted);
        ADVANCE;
        DIAG_END(&diagLeadingZerosAreNotPermitted);

        if (M3C_VEC_PUSH(
                M3C_Diagnostic, &lexer->diagnostics->vec, &diagLeadingZerosAreNotPermitted
            ) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;

        return __M3C_ASM_lexNumberUntilEnd(lexer);

    } else if (M3C_InRange_LETTER(cp)) {

        DIAG_START_FROM_LEXER(&diagUnknownBasePrefix);
        ADVANCE;
        DIAG_END(&diagUnknownBasePrefix);

        if (M3C_VEC_PUSH(M3C_Diagnostic, &lexer->diagnostics->vec, &diagUnknownBasePrefix) !=
            M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->errors;

        return __M3C_ASM_lexNumberUntilEnd(lexer);

    } else {
        /* it was decimal `0` and now the new token started or just whitespace
         * status == OK or INVALID_ENCODING */
        TOK_KIND(M3C_ASM_TOKEN_KIND_NUMBER);
        TOK_END;

        if (TOK_PUSH != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        return M3C_ERROR_OK;
    }
}

/**
 * \brief Lexes the part of \ref M3C_ASM_TOKEN_KIND_STRING "string literal" where an invalid
 * character sequence is.
 *
 * \details Diagnostics:
 * + (required) \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING"
 *
 * \param[in,out] lexer lexer
 * \return
 * + #M3C_ERROR_OK - OK or EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexStringInvalidEncodings(M3C_ASM_Lexer *lexer) {
    /* NOTE(M3C-59): C compiler just pushed these bytes to a string without a word. To do so we need
     * to PEEK differently - right now PEEK reads code points (not bytes) and just returns `�` for
     * those bytes */

    VAR_DECL;

    M3C_Diagnostic diagInvalidEncoding;

    diagInvalidEncoding.severity = M3C_SEVERITY_ERROR;
    diagInvalidEncoding.info = &M3C_ASM_DIAGNOSTIC_INFO_INVALID_ENCODING;

    TOK_KIND(M3C_ASM_TOKEN_KIND_UNRECOGNIZED);
    DIAG_START_FROM_LEXER(&diagInvalidEncoding);

    PEEK;
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

    return M3C_ERROR_OK;
}

/**
 * \brief Lexes escape sequences in \ref M3C_ASM_TOKEN_KIND_STRING "string literal".
 *
 * \details Diagnostics:
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_UNKNOWN_ESCAPE_SEQUENCE
 * "UNKNOWN_ESCAPE_SEQUENCE"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS
 * "X_USED_WITH_NO_FOLLOWING_HEX_DIGITS"
 *
 * \param[in,out] lexer lexer
 * \return
 * + #M3C_ERROR_OK - OK or EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexEscapeSequence(M3C_ASM_Lexer *lexer) {
    VAR_DECL;

    M3C_Diagnostic diagUnknownEscapeSequence;
    M3C_Diagnostic diagXUsedWithNoFollowingHexDigits;

    diagUnknownEscapeSequence.severity = M3C_SEVERITY_WARNING;
    diagUnknownEscapeSequence.info = &M3C_ASM_DIAGNOSTIC_INFO_UNKNOWN_ESCAPE_SEQUENCE;

    diagXUsedWithNoFollowingHexDigits.severity = M3C_SEVERITY_ERROR;
    diagXUsedWithNoFollowingHexDigits.info =
        &M3C_ASM_DIAGNOSTIC_INFO_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS;

    DIAG_START_FROM_LEXER(&diagUnknownEscapeSequence);
    DIAG_START_FROM_LEXER(&diagXUsedWithNoFollowingHexDigits);

    PEEK; /* re-peek '\\' */
    ADVANCE;

    PEEK;
    if (status == M3C_ERROR_OK &&
        (cp == '\'' || cp == '"' || cp == '?' || cp == '\\' || cp == 'a' || cp == 'b' ||
         cp == 'f' || cp == 'n' || cp == 'r' || cp == 't' || cp == 'v')) {
        ADVANCE;
        return M3C_ERROR_OK;
    } else if (status == M3C_ERROR_OK && cp == 'x') {
        ADVANCE;
        PEEK; /* peek the first digit */

        if (status != M3C_ERROR_OK || (status == M3C_ERROR_OK && !M3C_InRange_DIGIT_HEX(cp))) {
            TOK_KIND(M3C_ASM_TOKEN_KIND_UNRECOGNIZED);
            DIAG_END(&diagXUsedWithNoFollowingHexDigits);

            if (M3C_VEC_PUSH(
                    M3C_Diagnostic, &lexer->diagnostics->vec, &diagXUsedWithNoFollowingHexDigits
                ) != M3C_ERROR_OK)
                return M3C_ERROR_OOM;
            ++lexer->diagnostics->errors;

            return M3C_ERROR_OK;
        }

        /* Well, the first digit was a valid hex-digit */

        ADVANCE;
        PEEK; /* peek the second digit (if any) */

        if (status == M3C_ERROR_OK && M3C_InRange_DIGIT_HEX(cp)) {
            /* we don't care if there are another hex digits ahead like gcc or clang do */

            ADVANCE;
            return M3C_ERROR_OK;
        } else {
            /* just return. We already have at least one hex digit after "\\x" */
            return M3C_ERROR_OK;
        }
    } else {
        /* unknown escape sequences (and can be also EOF or an invalid encoding) */

        DIAG_END(&diagUnknownEscapeSequence);
        if (M3C_VEC_PUSH(M3C_Diagnostic, &lexer->diagnostics->vec, &diagUnknownEscapeSequence) !=
            M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        ++lexer->diagnostics->warnings;

        return M3C_ERROR_OK;
    }
}

M3C_ERROR __M3C_ASM_lexemizeString(M3C_ASM_Lexer *lexer) {
    VAR_DECL;

    M3C_ASM_CachedString cachedString;
    M3C_VEC(m3c_u8) vec;
    int d1;
    int d2;

    vec.data = m3c_malloc(sizeof(m3c_u8) * M3C_LEX_STRING_START_CAP);
    if (!vec.data)
        return M3C_ERROR_OOM;
    vec.cap = M3C_LEX_STRING_START_CAP;
    vec.len = 0;

    /* get rid of the first `"` */
    PEEK2;
    ADVANCE2;

    /* NOTE: if string is terminated just not read the last quote. If isn't just read until end. */
    while (lexer->ptr2 <
           (lexer->terminatingQuotePtr == M3C_NULL ? lexer->ptr : lexer->terminatingQuotePtr)) {

        PEEK2;

        if (cp == '\\') {
            ADVANCE2;
            PEEK2;
            if (status == M3C_ERROR_EOF)
                break;

            if (cp == '\'') {
                cp = 0x27;
            } else if (cp == '"') {
                cp = 0x22;
            } else if (cp == '?') {
                cp = 0x3F;
            } else if (cp == '\\') {
                cp = 0x5C;
            } else if (cp == 'a') {
                cp = 0x07;
            } else if (cp == 'b') {
                cp = 0x08;
            } else if (cp == 'f') {
                cp = 0x0c;
            } else if (cp == 'n') {
                cp = 0x0A;
            } else if (cp == 'r') {
                cp = 0x0D;
            } else if (cp == 't') {
                cp = 0x09;
            } else if (cp == 'v') {
                cp = 0x0B;
            } else if (cp == 'x') {
                ADVANCE2;

                /* NOTE: EOF here is impossible here as in this case we have kind == UNRECOGNIZED
                 * token and do not perform lexemizing */
                PEEK2;
                if (M3C_InRange_DIGIT_HEX(cp)) {
                    d1 = M3C_GetHexVal(cp);
                    ADVANCE2;
                    PEEK2;
                    if (M3C_InRange_DIGIT_HEX(cp) & status == M3C_ERROR_OK) {
                        d2 = M3C_GetHexVal(cp);
                        cp = d1 * 16 + d2;
                        ADVANCE2;
                    } else
                        cp = d1;

                    if (M3C_VEC_RESERVE(m3c_u8, &vec, vec.len + 1) != M3C_ERROR_OK) {
                        m3c_free(vec.data);
                        return M3C_ERROR_OOM;
                    }
                    vec.data[vec.len] = (m3c_u8)cp;
                    vec.len++;

                    continue;
                }
            }
        }

        if (M3C_VEC_RESERVE(m3c_u8, &vec, vec.len + 4) != M3C_ERROR_OK) {
            m3c_free(vec.data);
            return M3C_ERROR_OOM;
        }
        M3C_UTF8WriteCodepointWithLen(vec.data + vec.len, vec.data + vec.cap, cp, &cpLen);
        vec.len += cpLen;

        ADVANCE2;
    }

    cachedString.ptr = vec.data;
    cachedString.len = (m3c_u32)vec.len;

    lexer->token.lexeme.hStr = (m3c_u32)lexer->stringPool->len;

    if (STR_PUSH(&cachedString) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;

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
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_UNKNOWN_ESCAPE_SEQUENCE
 * "UNKNOWN_ESCAPE_SEQUENCE"
 * + (possible) \ref M3C_ASM_DIAGNOSTIC_ID_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS
 * "X_USED_WITH_NO_FOLLOWING_HEX_DIGITS"
 *
 * \warning It is not guaranteed to return #M3C_ERROR_EOF if EOF is reached after the token.
 *
 * \param[in,out] lexer lexer
 * \return
 * + #M3C_ERROR_OK - OK or EOF is reached
 * + #M3C_ERROR_EOF - if EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR __M3C_ASM_lexString(M3C_ASM_Lexer *lexer) {
    VAR_DECL;

    M3C_Diagnostic diagUnterminatedStringLiteral;
    diagUnterminatedStringLiteral.severity = M3C_SEVERITY_WARNING;
    diagUnterminatedStringLiteral.info = &M3C_ASM_DIAGNOSTIC_INFO_UNTERMINATED_STRING_LITERAL;

    TOK_KIND(M3C_ASM_TOKEN_KIND_STRING);

    PEEK; /* re-peek '"' */
    ADVANCE;

    M3C_LOOP {
        PEEK;

        if (status == M3C_ERROR_OK && cp == '"') {
            /* ": EOT */
            lexer->terminatingQuotePtr = lexer->ptr;

            ADVANCE;
            TOK_END;

            goto lexemize;

        } else if (status == M3C_ERROR_OK && cp == '\\') {
            /* \: start of escape sequence */

            status = __M3C_ASM_lexEscapeSequence(lexer);
            if (status != M3C_ERROR_OK)
                return status;
            continue;

        } else if (status == M3C_ERROR_OK && (cp != '\n' && cp != '\r')) {
            /* any char except `\` or `"`: any regular string char */

            ADVANCE;
            continue;

        } else if (status == M3C_ERROR_EOF || (status == M3C_ERROR_OK && (cp == '\n' || cp == '\r'))) {
            /* EOF, \n, or \r: EOT (with diagnostic) */

            lexer->terminatingQuotePtr = M3C_NULL;
            DIAG_START_FROM_TOKEN(&diagUnterminatedStringLiteral);
            DIAG_END(&diagUnterminatedStringLiteral);

            if (M3C_VEC_PUSH(
                    M3C_Diagnostic, &lexer->diagnostics->vec, &diagUnterminatedStringLiteral
                ) != M3C_ERROR_OK)
                return M3C_ERROR_OOM;
            ++lexer->diagnostics->warnings;

            TOK_END;
            goto lexemize;

        } else {
            /* status == INVALID_ENCODING */

            status = __M3C_ASM_lexStringInvalidEncodings(lexer);
            if (status != M3C_ERROR_OK)
                return status;

            continue;
        }
    }

lexemize:
    if (lexer->token.kind == M3C_ASM_TOKEN_KIND_STRING) {

        if (__M3C_ASM_lexemizeString(lexer) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
    }

    if (TOK_PUSH != M3C_ERROR_OK)
        return M3C_ERROR_OOM;
    return status; /* can be OK and EOF */
}

/**
 * \brief Fills the lexeme of \ref M3C_ASM_TOKEN_KIND_SYMBOL "symbol" token.
 *
 * \param[in,out] lexer lexer
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push string to stringPool
 */
M3C_ERROR __M3C_ASM_lexemizeSymbol(M3C_ASM_Lexer *lexer) {
    VAR_DECL;

    m3c_u8 *str;
    m3c_u8 *strPtr;
    M3C_ASM_CachedString cachedString;

    /* NOTE: we can allocate more then we need here if token is splitted by line continuation
     * sequence(s) */
    str = m3c_malloc(sizeof(m3c_u8) * (lexer->ptr - lexer->ptr2));
    if (!str)
        return M3C_ERROR_OOM;
    strPtr = str;

    /* read string from document to str */
    while (lexer->ptr2 < lexer->ptr) {
        PEEK2;

        /* NOTE: only ASCII chars can be in this token,
         *   1. so we know that `cp` <= 127
         *   2. so we need only one byte to represent `cp` in UTF-8 in stringPool
         */
        *strPtr = (m3c_u8)cp;
        ++strPtr;

        ADVANCE2;
    }

    cachedString.ptr = str;
    cachedString.len = (m3c_u32)(strPtr - str);

    lexer->token.lexeme.hStr = (m3c_u32)lexer->stringPool->len;

    if (STR_PUSH(&cachedString) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;

    return M3C_ERROR_OK;
}

/**
 * \brief Lexes the \ref M3C_ASM_TOKEN_KIND_SYMBOL "symbol" token.
 *
 * \param[in,out] lexer lexer
 * \return
 * + #M3C_ERROR_OK - OK or EOF is reached
 * + #M3C_ERROR_OOM - if failed to push token, diagnostic, or lexeme
 */
M3C_ERROR __M3C_ASM_lexSymbol(M3C_ASM_Lexer *lexer) {
    VAR_DECL;

    PEEK; /* re-peek first code point - [_A-Za-z] */
    ADVANCE;

    TOK_KIND(M3C_ASM_TOKEN_KIND_SYMBOL);

    M3C_LOOP {
        PEEK;

        if (status == M3C_ERROR_OK &&
            (cp == '_' || M3C_InRange(cp, '0', '9') || M3C_InRange_LETTER(cp))) {
            ADVANCE;
            continue;
        }

        break;
    }
    TOK_END;

    status = __M3C_ASM_lexemizeSymbol(lexer);
    if (status != M3C_ERROR_OK)
        return status;

    return TOK_PUSH;
}

#define ONE_CHAR_TOKEN(ch, tokenKind)                                                              \
    (cp == (ch)) {                                                                                 \
        TOK_KIND(tokenKind);                                                                       \
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
 * + #M3C_ERROR_OOM - if failed to push token, diagnostic, or lexeme
 */
M3C_ERROR __M3C_ASM_lexNextToken(M3C_ASM_Lexer *lexer) {
    VAR_DECL;

    /* skip whitespaces (only space and '\t') */
    M3C_LOOP {
        PEEK;
        if (status == M3C_ERROR_EOF)
            return M3C_ERROR_EOF;

        if (cp != ' ' && cp != '\t')
            break;
        ADVANCE;
    }
    TOK_START;

    if (cp == '\n') {
        TOK_KIND(M3C_ASM_TOKEN_KIND_EOL);
        ADVANCE_NL;
        TOK_END;
        return TOK_PUSH;
    } else if (cp == '\r') {
        TOK_KIND(M3C_ASM_TOKEN_KIND_EOL);
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '\n') { /* \r ~ \n */
            ADVANCE_NL;
            TOK_END;
            return TOK_PUSH;
        } else {
            __ADVANCE_ONLY_POS_NL;
            TOK_END;
            return TOK_PUSH;
        }
    } else if (cp == '!') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '=') {
            TOK_KIND(M3C_ASM_TOKEN_KIND_EXCLAIMEQUAL);
            goto one_char_token;
        } else {
            TOK_KIND(M3C_ASM_TOKEN_KIND_EXCLAIM);
            TOK_END;
            return TOK_PUSH;
        }
    } else if (cp == '"')
        return __M3C_ASM_lexString(lexer);
    else if
        ONE_CHAR_TOKEN('%', M3C_ASM_TOKEN_KIND_PERCENT)
    else if (cp == '&') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '&') {
            TOK_KIND(M3C_ASM_TOKEN_KIND_AMPAMP);
            goto one_char_token;
        } else {
            TOK_KIND(M3C_ASM_TOKEN_KIND_AMP);
            TOK_END;
            return TOK_PUSH;
        }
    } else if
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
        return __M3C_ASM_lexZero(lexer);
    else if (cp >= '1' && cp <= '9') {
        ADVANCE;
        return __M3C_ASM_lexNumberBody(lexer, UNDERSCORE_DIGITS_LEN_DEC);
    } else if
        ONE_CHAR_TOKEN (':', M3C_ASM_TOKEN_KIND_COLON)
    else if (cp == ';')
        return __M3C_ASM_lexCommentToken(lexer);
    else if (cp == '<') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '<') {
            TOK_KIND(M3C_ASM_TOKEN_KIND_LESSLESS);
            goto one_char_token;
        } else if (status == M3C_ERROR_OK && cp == '=') {
            TOK_KIND(M3C_ASM_TOKEN_KIND_LESSEQUAL);
            goto one_char_token;
        } else {
            TOK_KIND(M3C_ASM_TOKEN_KIND_LESS);
            TOK_END;
            return TOK_PUSH;
        }
    } else if (cp == '=') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '=') {
            TOK_KIND(M3C_ASM_TOKEN_KIND_EQUALEQUAL);
            goto one_char_token;
        } else
            return __M3C_ASM_lexUnrecognisedToken(lexer);
    } else if (cp == '>') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '>') {
            TOK_KIND(M3C_ASM_TOKEN_KIND_GREATERGREATER);
            goto one_char_token;
        } else if (status == M3C_ERROR_OK && cp == '=') {
            TOK_KIND(M3C_ASM_TOKEN_KIND_GREATEREQUAL);
            goto one_char_token;
        } else {
            TOK_KIND(M3C_ASM_TOKEN_KIND_GREATER);
            TOK_END;
            return TOK_PUSH;
        }
    } else if
        ONE_CHAR_TOKEN('?', M3C_ASM_TOKEN_KIND_QUESTION)
    else if (cp == '_' || M3C_InRange_LETTER(cp))
        return __M3C_ASM_lexSymbol(lexer);
    else if
        ONE_CHAR_TOKEN('^', M3C_ASM_TOKEN_KIND_CARET)
    else if (cp == '|') {
        ADVANCE;
        PEEK;
        if (status == M3C_ERROR_OK && cp == '|') {
            TOK_KIND(M3C_ASM_TOKEN_KIND_PIPEPIPE);
            goto one_char_token;
        } else {
            TOK_KIND(M3C_ASM_TOKEN_KIND_PIPE);
            TOK_END;
            return TOK_PUSH;
        }
    } else if
        ONE_CHAR_TOKEN('~', M3C_ASM_TOKEN_KIND_TILDE)
    else
        return __M3C_ASM_lexUnrecognisedToken(lexer);

/* Not for EOL */
one_char_token:
    ADVANCE;
    TOK_END;
    return TOK_PUSH;
}

M3C_ERROR M3C_ASM_lex(M3C_ASM_PreProc *preproc, m3c_u32 hDocument) {
    M3C_ASM_Document *document;
    M3C_ASM_Lexer lexer;
    M3C_ERROR res;

    if (hDocument >= preproc->documents.len)
    return M3C_ERROR_BAD_HANDLE;

    lexer.stringPool = &preproc->stringPool;
    document = &preproc->documents.data[hDocument];

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

#ifndef _M3C_INCGUARD_ASM_LEX_H
#define _M3C_INCGUARD_ASM_LEX_H

#include <m3c/common/errors.h>
#include <m3c/asm/types.h>

#ifndef M3C_LEX_STRING_START_CAP
/**
 * \brief The initial capacity for \ref M3C_ASM_TOKEN_KIND_STRING "string literal" lexeme.
 *
 * \details We assume that the majority of strings will serve as variable or label identifiers, and
 * therefore, their length will be limited to three characters.
 */
#    define M3C_LEX_STRING_START_CAP 4
#endif

/**
 * \brief Token kind.
 */
typedef enum __tagM3C_ASM_TokenKind {
    /**
     * \brief Unrecognized token.
     *
     * \warning Can contain non-ASCII characters and invalid bytes.
     */
    M3C_ASM_TOKEN_KIND_UNRECOGNIZED = 0,
    /**
     * \brief Comment.
     *
     * \details Always include `;` as the first character. Don't include EOL sequence.
     *
     * \warning Can contain non-ASCII characters and invalid bytes.
     */
    M3C_ASM_TOKEN_KIND_COMMENT = 1,
    /**
     * \brief Number literal.
     *
     * \details Integer number literal.
     */
    M3C_ASM_TOKEN_KIND_NUMBER = 2,
    /**
     * \brief String literal.
     *
     * \details String literal starts with \c '\\\"' and ends with \c '\\\"', EOL, or EOF. If string
     * literals doesn't ends with \c '\\\"' \ref M3C_ASM_DIAGNOSTIC_ID_UNTERMINATED_STRING_LITERAL
     * "UNTERMINATED_STRING_LITERAL" diagnostic will be emitted.
     *
     * String literal can contains at least 0 elements. Each element of string can be:
     * + a printable character (`[ -~]`) except \c '\\\"' and \c '\\\\' characters
     * + an escape sequences
     *
     * Escape sequences:
     * + `\xN`, where N is one or two hexadecimal digits - arbitrary number of hexadecimal digits
     * + \c \\' - byte 0x27
     * + `\"` - byte 0x22
     * + `\?` - byte 0x3f
     * + `\\` - byte 0x5c
     * + `\a` - byte 0x07
     * + `\b` - byte 0x08
     * + `\f` - byte 0x0c
     * + `\n` - byte 0x0a
     * + `\r` - byte 0x0d
     * + `\t` - byte 0x09
     * + `\v` - byte 0x0b
     *
     * \warning Can be unterminated.
     */
    M3C_ASM_TOKEN_KIND_STRING = 3,
    /**
     * \brief Symbol.
     *
     * \details RegEx: `[_A-Za-z][_0-9A-Za-z]*`.
     */
    M3C_ASM_TOKEN_KIND_SYMBOL = 4,
    /**
     * \brief Left parenthesis (`(`).
     */
    M3C_ASM_TOKEN_KIND_L_PAREN = 5,
    /**
     * \brief Right parenthesis (`)`).
     */
    M3C_ASM_TOKEN_KIND_R_PAREN = 6,
    /**
     * \brief Comma (`,`).
     */
    M3C_ASM_TOKEN_KIND_COMMA = 7,
    /**
     * \brief Colon (`:`).
     */
    M3C_ASM_TOKEN_KIND_COLON = 8,
    /**
     * \brief Plus sign (`+`).
     */
    M3C_ASM_TOKEN_KIND_PLUS = 9,
    /**
     * \brief Hyphen-minus (`-`).
     */
    M3C_ASM_TOKEN_KIND_MINUS = 10,
    /**
     * \brief Asterisk (`*`).
     */
    M3C_ASM_TOKEN_KIND_STAR = 11,
    /**
     * \brief Solidus (`/`).
     */
    M3C_ASM_TOKEN_KIND_SLASH = 12,
    /**
     * \brief Percent sign (`%`).
     */
    M3C_ASM_TOKEN_KIND_PERCENT = 13,
    /**
     * \brief Tilde (`~`).
     */
    M3C_ASM_TOKEN_KIND_TILDE = 14,
    /**
     * \brief Ampersand (`&`).
     */
    M3C_ASM_TOKEN_KIND_AMP = 15,
    /**
     * \brief Vertical line (`|`).
     */
    M3C_ASM_TOKEN_KIND_PIPE = 16,
    /**
     * \brief Circumflex accent (`^`).
     */
    M3C_ASM_TOKEN_KIND_CARET = 17,
    /**
     * \brief Left shift (`<<`).
     */
    M3C_ASM_TOKEN_KIND_LESSLESS = 18,
    /**
     * \brief Right shift (`>>`).
     */
    M3C_ASM_TOKEN_KIND_GREATERGREATER = 19,
    /**
     * \brief `==`.
     */
    M3C_ASM_TOKEN_KIND_EQUALEQUAL = 20,
    /**
     * \brief `!=`.
     */
    M3C_ASM_TOKEN_KIND_EXCLAIMEQUAL = 21,
    /**
     * \brief `<`.
     */
    M3C_ASM_TOKEN_KIND_LESS = 22,
    /**
     * \brief `<=`.
     */
    M3C_ASM_TOKEN_KIND_LESSEQUAL = 23,
    /**
     * \brief `>`.
     */
    M3C_ASM_TOKEN_KIND_GREATER = 24,
    /**
     * \brief `>=`.
     */
    M3C_ASM_TOKEN_KIND_GREATEREQUAL = 25,
    /**
     * \brief Question mark (`?`).
     */
    M3C_ASM_TOKEN_KIND_QUESTION = 26,
    /**
     * \brief Exclamation mark (`!`).
     */
    M3C_ASM_TOKEN_KIND_EXCLAIM = 27,
    /**
     * \brief `||`.
     */
    M3C_ASM_TOKEN_KIND_PIPEPIPE = 28,
    /**
     * \brief `&&`.
     */
    M3C_ASM_TOKEN_KIND_AMPAMP = 29,
    /**
     * \brief End of line.
     *
     * \details Recognised EOL sequences:
     * + `\n`
     * + `\r\n`
     * + `\r`
     *
     * \note The \ref M3C_ASM_Token::end "end" of tokens of this kind is always set to the start
     * of a new line.
     */
    M3C_ASM_TOKEN_KIND_EOL
} M3C_ASM_TokenKind;

/**
 * \brief Maximum token length in code points (due to \ref M3C_ASM_Position "Position" constraints).
 */
#define M3C_ASM_Token_MAX_CLEN ((m3c_u32)M3C_U16_MAX * (m3c_u32)M3C_U16_MAX)

/**
 * \brief Lexeme - the "value" of the token.
 */
typedef union __tagM3C_ASM_Lexeme {
    /**
     * \brief Integer value.
     *
     * \details Used by \ref M3C_ASM_TOKEN_KIND_NUMBER "NUMBER" token type.
     */
    m3c_i32 num;
    /**
     * \brief Handle of the string lexeme.
     *
     * \details Used by the following token types:
     * + \ref M3C_ASM_TOKEN_KIND_SYMBOL "SYMBOL"
     * + \ref M3C_ASM_TOKEN_KIND_STRING "STRING"
     *
     * \note This handle in the current implementation corresponds to the index in \ref
     * __tagM3C_ASM_PreProc::stringPool "preproc's string pool".
     */
    m3c_u32 hStr;
} M3C_ASM_Lexeme;

/**
 * \brief Token.
 *
 * \note Only tokens of kind #M3C_ASM_TOKEN_KIND_UNRECOGNIZED and #M3C_ASM_TOKEN_KIND_COMMENT can
 * contain non-ASCII characters and invalid bytes.
 *
 * \warning Token may overlap physical line when using preprocessor (`\\` + EOL is used as logical
 * line continuation sequence).
 */
struct __tagM3C_ASM_Token {
    /**
     * \brief Parsed lexeme of the token.
     *
     * \warning Only tokens of the following types have their own lexemes:
     * + \ref M3C_ASM_TOKEN_KIND_NUMBER "NUMBER" (using #M3C_ASM_Lexeme::num)
     * + \ref M3C_ASM_TOKEN_KIND_SYMBOL "SYMBOL" (using #M3C_ASM_Lexeme::hStr)
     * + \ref M3C_ASM_TOKEN_KIND_STRING "STRING" (using #M3C_ASM_Lexeme::hStr)
     */
    M3C_ASM_Lexeme lexeme;
    /**
     * \brief Token kind.
     */
    M3C_ASM_TokenKind kind;
    /**
     * \brief Position of the first character of this token.
     */
    M3C_ASM_Position start;
    /**
     * \brief Position of the end of token (exclusive).
     *
     * \note If the token kind is \ref M3C_ASM_TOKEN_KIND_EOL "EOL", the field is set to the start
     * of a new line.
     */
    M3C_ASM_Position end;
};

/**
 * \brief Lexes the given document.
 *
 * \details Fills \ref M3C_ASM_Document::tokens "tokens" and \ref M3C_ASM_Document::diagnostics
 * "diagnostics" of the document.
 *
 * \param[in,out] preproc   preprocessor
 * \param         hDocument document handle (the document index in `preproc::documents`)
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push token, diagnostic, or lexeme
 */
M3C_ERROR M3C_ASM_lex(M3C_ASM_PreProc *preproc, m3c_u32 hDocument);

#endif /* _M3C_INCGUARD_ASM_LEX_H */

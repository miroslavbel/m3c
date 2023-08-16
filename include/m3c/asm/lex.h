#ifndef _M3C_INCGUARD_ASM_LEX_H
#define _M3C_INCGUARD_ASM_LEX_H

#include <m3c/common/errors.h>
#include <m3c/asm/types.h>

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
     * \brief New line.
     */
    M3C_ASM_TOKEN_KIND_NEW_LINE
} M3C_ASM_TokenKind;

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
     * \brief Pointer to the first byte of the token.
     *
     * \note Pointer to the file buffer is used, as the following stages will require access to the
     * token bytes.
     */
    m3c_u8 const *ptr;
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
     */
    M3C_ASM_Position end;
};

/**
 * \brief Lexes the given document.
 *
 * \details Fills \ref M3C_ASM_Document::tokens "tokens" and \ref M3C_ASM_Document::diagnostics
 * "diagnostics" of the document.
 *
 * \param[in,out] document
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if failed to push token or diagnostic
 */
M3C_ERROR M3C_ASM_lex(M3C_ASM_Document *document);

#endif /* _M3C_INCGUARD_ASM_LEX_H */

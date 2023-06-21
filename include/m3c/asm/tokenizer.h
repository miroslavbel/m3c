#ifndef _M3C_INCGUARD_ASM_TOKENIZER_H
#define _M3C_INCGUARD_ASM_TOKENIZER_H

#include <m3c/common/types.h>
#include <m3c/common/callbacks.h>

#include <m3c/asm/errors.h>

/**
 * \brief A buffer describing the beginning and end of the source code.
 */
typedef struct tagM3C_ASM_SourceBuffer {
    /**
     * \brief A pointer to the first byte of the buffer.
     *
     * \warning If the buffer length is `0`, then it can point anywhere. The buffer length is
     * considered to be zero if the #last field is less than this field.
     * \warning The buffer length must be less than `2 ** 32 - 1`.
     */
    const m3c_u8 *first;
    /**
     * \brief A pointer to the last byte in the buffer.
     *
     * \warning If the buffer length is `0`, then it can point anywhere. The buffer length is
     * considered to be zero if this field is less than the #first field.
     * \warning The buffer length must be less than `2 ** 32 - 1`.
     */
    const m3c_u8 *last;
} M3C_ASM_SourceBuffer;

typedef struct tagM3C_ASM_FileName {
    m3c_u8 *nick;
    m3c_u8 *full;
} M3C_ASM_FileName;

typedef struct tagM3C_ASM_Position {
    /**
     * Zero-based index of line.
     */
    m3c_u32 line;
    /**
     * Zero-based index of character in the line.
     */
    m3c_u32 character;
} M3C_ASM_Position;

typedef enum tagM3C_ASM_Token_Kind {
    /**
     * \brief Represents an invalid token.
     *
     * \warning May contain non-ASCII characters.
     */
    M3C_ASM_INVDALID_TOKEN = 0,

    /**
     * \brief Represents a new line.
     *
     * \note Tokens of this kind have \ref M3C_ASM_Token::len "len" equal to `1`.
     */
    M3C_ASM_NL_TOKEN = 14,

    /**
     * \brief Represents the end of the buffer. Always the last token in the buffer.
     *
     * \note Tokens of this kind have \ref M3C_ASM_Token::len "len" equal to `0`.
     */
    M3C_ASM_EOF_TOKEN = 15
} M3C_ASM_Token_Kind;

typedef struct tagM3C_ASM_Token {
    /**
     * \brief Token type.
     */
    M3C_ASM_Token_Kind kind;
    /**
     * \brief A pointer to the first byte of the token.
     *
     * \warning If the token kind is #M3C_ASM_EOF_TOKEN, then it points to the byte immediately
     * after the end of the source buffer.
     */
    const m3c_u8 *ptr;
    M3C_ASM_Position pos;
    /**
     * \brief The length of the token in bytes.
     *
     * \note Tokens of kind #M3C_ASM_EOF_TOKEN have a length equal to 0.
     */
    m3c_u32 len;
    M3C_ASM_FileName *file;
} M3C_ASM_Token;

typedef struct tagM3C_ASM_Tokens {
    /**
     * \brief A pointer to tokens.
     */
    M3C_ASM_Token *data;
    /**
     * \brief The number of tokens.
     */
    m3c_u32 len;
    /**
     * \brief Capacity.
     *
     * \note There is no reason to set capacity for more than the number of characters in src + `1`
     * (but just in case, it should be equal to or greater than two).
     *
     * \warning Should always be equal to or greater than two.
     */
    m3c_u32 cap;
} M3C_ASM_Tokens;

typedef struct tagM3C_ASM_Tokenizer {
    /**
     * \brief A pointer to a structure describing the path to the source code.
     *
     * \note The tokenizer does not use this structure. Only copies a pointer to it into each token.
     * \warning Can be `NULL`.
     */
    M3C_ASM_FileName *name;
    M3C_ASM_SourceBuffer src;
    /**
     * \brief The zero-based position (line number, column number) that #ptr points to.
     */
    M3C_ASM_Position pos;
    /**
     * \brief A pointer to the first byte in #src from which to start parsing.
     */
    const m3c_u8 *ptr;
    /**
     * \brief A vector of \ref M3C_ASM_Token "tokens".
     */
    M3C_ASM_Tokens *tokens;
    /* TODO: diagnostics */
} M3C_ASM_Tokenizer;

typedef struct tagM3C_ASM_TokenizerOptions {
    /**
     * \brief A callback that is called to relocate the \ref M3C_ASM_Tokenizer::tokens
     * "vector of tokens" if it runs out of space.
     *
     * \details If it returns a pointer to zero, parsing will fail with an #M3C_ASM_ERROR_OOM error.
     * If set as a `NULL`, no call is made and #M3C_ASM_ERROR_OOM will return immediately.
     *
     * \note Requests twice as much memory as before. However, the maximum requested memory size is
     * for the number of tokens equal to the length of the source code (one character - one token)
     * `+ 1`.
     *
     * \warning Can be `NULL`.
     */
    M3C_ReallocCB tokens_realloc;
    M3C_ReallocCB diagnostics_realloc;
    m3c_bool emit_diagnostics;
} M3C_ASM_TokenizerOptions;

/**
 * \brief Breaks the source code into tokens and pushes them into \ref M3C_ASM_Tokenizer::tokens
 * "tokens".
 *
 * \param[in,out] tokenizer tokenizer
 * \param[in]     options   options
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
M3C_ASM_Tokenize(M3C_ASM_Tokenizer *tokenizer, const M3C_ASM_TokenizerOptions *options);

#endif /* _M3C_INCGUARD_ASM_TOKENIZER_H */

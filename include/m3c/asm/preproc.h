#ifndef _M3C_INCGUARD_ASM_PREPROC_H
#define _M3C_INCGUARD_ASM_PREPROC_H

#include <m3c/common/types.h>
#include <m3c/common/coltypes.h>
#include <m3c/common/errors.h>
#include <m3c/asm/types.h>
#include <m3c/core/diagnostics.h>

/** \file
 * Preprocessor routines.
 *
 * \section term Terminology:
 *
 * \subsection term_eol End of line sequence
 * <b>E</b>nd <b>o</b>f <b>l</b>ine (or <b>e</b>nd-<b>o</b>f-<b>l</b>ine, EOL) sequence - is a
 * sequence of characters that indicates the end of a line.
 *
 *
 * Only the following sequences are considered as end-of-line sequences:
 * + `"\n"`
 * + `"\r"`
 * + `"\r\n"`
 *
 * \sa \ref term_phase_ls
 *
 * \subsection term_lcc Line continuation character
 * The backslash character (<tt>'\\\\'</tt>), when placed immediately before the \ref term_eol
 * "end of line sequence" sequence, forms a \ref term_lcs "line continuation sequence".
 *
 * \subsection term_lcs Line continuation sequence
 * A sequence of characters that is recognized by the preprocessor as a continuation of a \ref
 * term_logical_line "logical line". If the preprocessor is used these sequences is simply removed
 * from the source code during \ref term_phase_ls "Line Splitting Phase", lengthening \ref
 * term_logical_line "logical lines".
 *
 *
 * Line continuation sequences (<tt>'\\\\'</tt> is used as \ref term_lcc
 * "line continuation character"):
 * + `"\\\n"`
 * + `"\\\r"`
 * + `"\\\r\n"`
 *
 * \sa \ref term_eol
 *
 * \subsection term_phase_ls Line Splitting Phase
 * If a preprocessor is used, the document is split into \ref term_logical_line "logical lines"
 * during this phase by removing \ref term_lcs "line continuation sequences" from the source code.
 * If a preprocessor is not used, \ref term_physical_line "physical lines" correspond strictly to
 * \ref term_logical_line "logical lines".
 *
 * For example, in this phase, a document containing a sequence of characters `"0\\\r\n1\r\n2\\"`
 * will be split into the following \ref term_logical_line "logical lines":
 * + (without preprocessor) `"0\\\r\n"`, `"1\r\n"`, `"2\\"`
 * + (with preprocessor) `"01\r\n"`, `"2\\"`
 *
 * \subsubsection term_phase_ls_id Implementation details:
 * Actually, this phase does not split the document into \ref term_logical_line "logical lines", but
 * into \ref M3C_ASM_Fragment "fragments". For example, a document containing a sequence of
 * characters `"0\\\r\n1\r\n2\\"` will be split into the following \ref M3C_ASM_Fragment
 * "fragments":
 * + (without preprocessor) `"0\\\r\n"`, `"1\r\n"`, `"2\\"`
 * + (with preprocessor) `"0"`, `"1\r\n"`, `"2\\"`
 *
 * \note In other words, each fragment corresponds to a \ref term_physical_line "physical line",
 * except when the preprocessor is used and the \ref term_physical_line "physical line" ends with
 * the \ref term_lcs "line continuation sequence", in which case the end of the fragment containing
 * this \ref term_lcs "line continuation sequence" is truncated.
 *
 * \sa \ref term_phase
 *
 * \subsection term_logical_line Logical line
 * A sequence of characters that ends with either an \ref term_eol "end of line sequence" or the end
 * of file (EOF). If a preprocessor is used, it may be formed from several \ref term_physical_line
 * "physical lines" during \ref term_phase_ls "Line Splitting Phase". However, if no preprocessor is
 * used, it corresponds exactly to one \ref term_physical_line "physical line".
 *
 * \sa \ref term_logical_pos
 *
 * \subsection term_logical_pos Logical position
 * Contains zero-based \ref term_logical_line "logical line" index and zero-based character index.
 *
 * \sa \ref term_phase_ls, \ref term_physical_pos
 *
 * \subsection term_physical_line Physical line
 * A sequence of characters in a document that ends with either an \ref term_eol
 * "end of line sequence" or the end of file (EOF).
 *
 * \sa \ref term_physical_pos, \ref term_logical_line
 *
 * \subsection term_physical_pos Physical position
 * The physical (real) position of a character in a (raw) document. Contains zero-based
 * \ref term_physical_line "physical line" index and zero-based character index.
 *
 * \sa \ref term_phase_ls, \ref term_logical_pos
 *
 * \subsection term_phase Phase
 *
 * Known phases:
 * + \ref term_phase_ls
 */

/**
 * \brief Fragment.
 */
typedef struct __tagM3C_ASM_Fragment {
    /**
     * \brief Pointer to the first byte of this fragment.
     */
    m3c_u8 const *bFirst;
    /**
     * \brief Pointer to the last byte of this fragment (or `NULL` if this fragment has a length
     * equal to `0`).
     */
    m3c_u8 const *bLast;
    /**
     * \brief \ref term_physical_pos "Physical position" of the first character in this
     * fragment.
     *
     * \note \ref M3C_ASM_Position::character "pos::character" will be always equal to zero.
     */
    M3C_ASM_Position pos;
} M3C_ASM_Fragment;

/**
 * \brief See \ref M3C_ASM_Document::fragments "document::fragments".
 */
typedef struct __tagM3C_ASM_FragmentsCache {
    /**
     * \brief Number of fragments.
     */
    m3c_size_t len;
    /**
     * \brief Pointer to an fragments array of length #len.
     */
    M3C_ASM_Fragment *data;
} M3C_ASM_FragmentsCache;

struct __tagM3C_ASM_Document {
    /**
     * \brief Pointer to the first byte of this document.
     *
     * \warning Can't be `NULL` even if the document is empty.
     */
    m3c_u8 const *bFirst;
    /**
     * \brief Pointer to the last byte of this document (or `NULL` if this document has a length
     * equal to `0`).
     */
    m3c_u8 const *bLast;
    /**
     * \brief Document fragments.
     *
     * \details This field will be filled in during \ref term_phase_ls "Line Splitting Phase".
     *
     * \note To access a specific physical line fragment, use the index of the \ref
     * term_physical_line "physical line". However, if a preprocessor is used, the \ref term_lcs
     * "line continuation sequence" will be missing in the fragment.
     * \note The fragments will be the same for the same document. There is no need to calculate
     * them each time the same document is included.
     */
    M3C_ASM_FragmentsCache fragments;
    /**
     * \brief Raw tokens.
     *
     * \note The tokens will be the same for the same document. There is no need to calculate
     * them each time the same document is included.
     */
    M3C_ASM_Tokens tokens;
    /**
     * \brief Lexer diagnostics.
     *
     * \details \ref M3C_ASM_DiagnosticsData::hToken "DiagnosticsData::hToken" of all diagnostics
     * must correspond to the index of token in \ref M3C_ASM_Document::tokens "Document::tokens".
     *
     * Possible diagnostics:
     * + \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_ENCODING "INVALID_ENCODING"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_UNRECOGNIZED_TOKEN "UNRECOGNIZED_TOKEN"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_LEADING_ZEROS_ARE_NOT_PERMITTED
     * "LEADING_ZEROS_ARE_NOT_PERMITTED"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_BASE_PREFIX "INVALID_BASE_PREFIX"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_DIGIT_SEPARATOR_CANNOT_APPEAR_HERE
     * "DIGIT_SEPARATOR_CANNOT_APPEAR_HERE"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT
     * "NUMBER_LITERAL_MUST_CONTAIN_AT_LEAST_ONE_DIGIT"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_INVALID_DIGIT_FOR_THIS_BASE_PREFIX
     * "INVALID_DIGIT_FOR_THIS_BASE_PREFIX"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_UNTERMINATED_STRING_LITERAL "UNTERMINATED_STRING_LITERAL"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_UNKNOWN_ESCAPE_SEQUENCE "UNKNOWN_ESCAPE_SEQUENCE"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_X_USED_WITH_NO_FOLLOWING_HEX_DIGITS
     * "X_USED_WITH_NO_FOLLOWING_HEX_DIGITS"
     * + \ref M3C_ASM_DIAGNOSTIC_ID_NUMBER_CONSTANT_IS_TOO_LARGE "NUMBER_CONSTANT_IS_TOO_LARGE"
     *
     * \note The diagnostics will be the same for the same document. There is no need to calculate
     * them each time the same document is included.
     */
    M3C_Diagnostics diagnostics;
};

/**
 * \brief Describes a string in the string pool.
 */
typedef struct __tagM3C_ASM_CachedString {
    /**
     * \brief Pointer to the string (in UTF-8).
     *
     * \warning The string is not null-terminated.
     */
    m3c_u8 *ptr;
    /**
     * \brief The byte length of the string.
     *
     * \warning The string is not null-terminated. The UTF-8 is used.
     */
    m3c_u32 len;
} M3C_ASM_CachedString;

typedef M3C_VEC(M3C_ASM_CachedString) M3C_ASM_StringPool;

/**
 * \brief Preprocessor.
 */
struct __tagM3C_ASM_PreProc {
    /**
     * \brief List of source code documents.
     */
    M3C_VEC(M3C_ASM_Document) documents;
    /**
     * \brief String pool.
     *
     * Used to store token lexemes.
     */
    M3C_ASM_StringPool stringPool;
};

/**
 * \brief Inits the \ref M3C_ASM_Document "document" struct.
 *
 * \param[in,out] document document struct to init
 * \param         buf      document buffer (source text). Can't be `NULL` even if the document is
 * empty.
 * \param         bufLen   length of the document buffer. Can be `0`
 */
void M3C_ASM_Document_Init(M3C_ASM_Document *document, m3c_u8 const *buf, m3c_size_t bufLen);

/**
 * \brief Deinits the \ref M3C_ASM_Document "document" struct.
 *
 * \param[in] document document struct to deinit
 */
void M3C_ASM_Document_Deinit(M3C_ASM_Document const *document);

/**
 * \brief Performs \ref term_phase_ls "Line Splitting Phase".
 *
 * \details Splits the document into lines (using \ref term_eol "EOL" sequences), filling the
 * document \ref M3C_ASM_Document::fragments "fragments". If a preprocessor (param `usePreproc`) is
 * used, \ref term_lcs "line continuation sequences" are cut from the fragments.
 *
 * \note If the fragments are already filled, returns \ref M3C_ERROR_OK "OK" and does nothing.
 *
 * \param[in,out] document   document
 * \param         usePreproc
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - if the function lacks memory
 */
M3C_ERROR __M3C_ASM_Document_SplitLines(M3C_ASM_Document *document, m3c_bool usePreproc);

#endif /* _M3C_INCGUARD_ASM_PREPROC_H */

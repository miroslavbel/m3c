#ifndef _M3C_INCGUARD_ASM_PREPROC_H
#define _M3C_INCGUARD_ASM_PREPROC_H

#include <m3c/common/types.h>
#include <m3c/common/coltypes.h>
#include <m3c/common/utf8.h>

typedef struct __tagM3C_ASM_Fragment M3C_ASM_Fragment;

/**
 * \brief Singly linked list of text fragments.
 */
typedef struct __tagM3C_ASM_FragmentList {
    /**
     * \brief Pointer to the first text fragment.
     */
    M3C_ASM_Fragment *first;
    /**
     * \brief Length.
     */
    m3c_u32 len;
} M3C_ASM_FragmentList;

typedef struct __tagM3C_ASM_Document {
    /**
     * \brief Full path to this document in UTF-8 (serves also as a unique document identifier).
     */
    const M3C_UTF8Z fullPath;
    /**
     * \brief Pointer to the first byte of this document.
     *
     * \warning Can contain invalid UTF-8 sequnece.
     */
    const m3c_u8 *bFirst;
    /**
     * \brief Pointer to the last byte of this document.
     *
     * \warning If the length of this document is `0`, then it must be less than #bFirst. In
     * such cases, it's preferable to always set this filed to `NULL`.
     */
    const m3c_u8 *bLast;
    /**
     * \brief Text fragments.
     *
     * \details This field is filled after the stage of replacing invalid UTF-8 sequences and the
     * stage of continuation line collapsing (if the preprocessor is not used, then the stage of
     * continuation line collapsing is skipped).
     *
     * \note Serves for caching text fragments, since the same document produces the same text
     * fragments at this stage.
     */
    M3C_ASM_FragmentList fragments;
} M3C_ASM_Document, *M3C_ASM_Document_Ptr;

/**
 * \brief Represents the character position in a document.
 */
typedef struct __tagM3C_ASM_Position {
    /**
     * \brief Zero-based line index.
     */
    m3c_u32 line;
    /**
     * \brief Zero-based character index.
     */
    m3c_u32 character;
} M3C_ASM_Position;

/**
 * \brief A structure representing a text fragment.
 */
struct __tagM3C_ASM_Fragment {
    /**
     * \brief Pointer to the first byte of this text fragment.
     */
    const M3C_UTF8 bFirst;
    /**
     * \brief Pointer to the last byte of this text fragment.
     *
     * \warning If the length of this text fragment is `0`, then it must be less than #bFirst. In
     * such cases, it's preferable to always set this field to `NULL`.
     */
    const M3C_UTF8cu *bLast;
    /**
     * \brief Position of the first character in this text node.
     */
    M3C_ASM_Position pos;
    /**
     * \brief Pointer to the next #M3C_ASM_Fragment.
     *
     * \warning Can be `NULL` in the last node.
     */
    M3C_ASM_Fragment *next;
};

typedef M3C_VEC(M3C_UTF8Z) M3C_ASM_StringPool;
typedef M3C_VEC(M3C_ASM_Document_Ptr) M3C_ASM_DocVec;

/**
 * \brief The preprocessor.
 */
typedef struct __tagM3C_ASM_PreProc {
    /**
     * \brief Documents vector.
     *
     * \details The preprocessor does not own these documents and cannot modify their content (raw
     * buffer) in any way. Documents passed to the preprocessor must not change and must outlive any
     * entities produced by the preprocessor (fragments, tokens, text relocations, etc.). The
     * documents are sorted by the \ref M3C_ASM_Document::fullPath "fullPath" field in byte
     * order and must be unique.
     *
     * \note A vector of pointers is used here, since the documents are provided and managed by the
     * user and the preprocessor needs to have valid references to the documents.
     */
    M3C_ASM_DocVec documents;
    /**
     * \brief The strings allocated by the preprocessor are stored here (for encoding recovery,
     * macro expansion, and so on).
     *
     * \warning Should not be freed if any preprocessor entity is still in use.
     */
    M3C_ASM_StringPool stringPool;
} M3C_ASM_PreProc;

/**
 * \brief Number of code points in #__M3C_ASM_REPLACEMENT_STR.
 */
#define __M3C_ASM_REPLACEMENT_STR_CPLEN 4
/**
 * \brief #__M3C_ASM_REPLACEMENT_STR length in bytes.
 */
#define __M3C_ASM_REPLACEMENT_STR_BLEN                                                             \
    (__M3C_ASM_REPLACEMENT_STR_CPLEN * M3C_UTF8_REPLACEMENT_CHARACTER_BLEN)
/**
 * \brief A predefined buffer that can be used for encoding recovery (rather than allocating a new
 * string each time).
 */
static const M3C_UTF8cu __M3C_ASM_REPLACEMENT_STR[__M3C_ASM_REPLACEMENT_STR_BLEN + 1] =
    M3C_UTF8_REPLACEMENT_CHARACTER_STR M3C_UTF8_REPLACEMENT_CHARACTER_STR
        M3C_UTF8_REPLACEMENT_CHARACTER_STR M3C_UTF8_REPLACEMENT_CHARACTER_STR;

#endif /* _M3C_INCGUARD_ASM_PREPROC_H */

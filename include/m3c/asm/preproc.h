#ifndef _M3C_INCGUARD_ASM_PREPROC_H
#define _M3C_INCGUARD_ASM_PREPROC_H

#include <m3c/common/types.h>
#include <m3c/common/errors.h>

/**
 * \brief Position.
 */
typedef struct __tagM3C_ASM_Position {
    /**
     * \brief Zero-based index of the line.
     */
    m3c_u32 line;
    /**
     * \brief Zero-based index of the character in the #line.
     */
    m3c_u32 character;
} M3C_ASM_Position;

/**
 * \brief Fragment.
 */
typedef struct __tagM3C_ASM_Fragment {
    /**
     * \brief Pointer to the first byte of this fragment.
     */
    m3c_u8 const *bFirst;
    /**
     * \brief Pointer to the last byte of this fragment (or `NULL` if this document has a length
     * equal to `0`).
     */
    m3c_u8 const *bLast;
    /**
     * \brief Position of the first character in this fragment.
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

typedef struct __tagM3C_ASM_Document {
    /**
     * \brief Pointer to the first byte of this document.
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
     * \details If a preprocessor is used, these are the fragments after Continuation Line
     * Collapsing phase. If no preprocessor is used, then only one fragment is stored here, which
     * contains the whole document.
     *
     * \note The fragments will be the same for the same document. There is no need to calculate
     * them each time the same document is included.
     */
    M3C_ASM_FragmentsCache fragments;
} M3C_ASM_Document;

/**
 * \brief Performs the continuation line collapsing phase of the preprocessor, filling the document
 * \ref M3C_ASM_Document::fragments "fragments".
 *
 * \warning Overwrites fragments without regard to what is there, which can lead to memory leak.
 *
 * \param document document
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM - when the function lacks memory
 */
M3C_ERROR __M3C_ASM_Document_ContinuationLineCollapsingPhase(M3C_ASM_Document *document);

#endif /* _M3C_INCGUARD_ASM_PREPROC_H */

#include <m3c/asm/preproc.h>

#include <m3c/common/macros.h>
#include <m3c/common/errors.h>
#include <m3c/rt/alloc.h>
#include <m3c/rt/mem.h>

/**
 * \brief Pushes the given `string` to the `stringPool`.
 *
 * \param[in,out] stringPool
 * \param[in]     string     string to be pushed
 * \return
 * + M3C_ERROR_OK
 * + M3C_ERROR_OOM - if failed to realloc `stringPool`
 */
M3C_ERROR __M3C_ASM_StringPool_Push(M3C_ASM_StringPool *stringPool, M3C_UTF8Z string) {
    M3C_UTF8Z *newPtr;
    m3c_size_t newCap;

    if (stringPool->len == stringPool->cap) {
        newCap = stringPool->cap + stringPool->cap;

        newPtr = (M3C_UTF8Z *)m3c_realloc(stringPool->data, newCap);
        if (!newPtr)
            return M3C_ERROR_OOM;

        stringPool->cap = newCap;
        stringPool->data = newPtr;
    }

    stringPool->data[stringPool->len++] = string;
    return M3C_ERROR_OK;
}

/**
 * \brief Fills the given `fragment` until it encounters an invalid UTF-8 sequence for \ref
 * #__M3C_ASM_PreProc_EncodingRecoveryStage "encoding recovery stage".
 *
 * \details Fills in the given `fragment` with valid codepoints. If the fragment points to an
 * invalid codepoint, it will set `fragment->bLast` to `NULL`. Sets `pos` to the position of code
 * point right after the last processed code point.
 *
 * ### Params:
 *
 * #### In:
 * + `fragment->pos`    - position of the first code point to process
 * + `fragment->bFirst` - pointer to the first code unit to process
 * + `fragment->bLast`  - pointer to the last code unit in the buffer
 *
 * #### Out:
 * + `fragment->pos`    - (*already set*) position of the first code point to process
 * + `fragment->bFirst` - (*already set*) pointer to the first code unit to process
 * + `fragment->bLast`  - pointer to the last valid code unit or `NULL` if the length of valid
 * UTF-8 sequence is `0`
 * + `*pos`             - position of code point right after the last valid code point
 *
 * \param[in,out] fragment fragment
 * \param[out]    pos      writes here the position of code point right after the last valid code
 * point
 */
void __M3C_ASM_PreProc_FillValidEncodingFragment(
    M3C_ASM_Fragment *fragment, M3C_ASM_Position *pos
) {
    const m3c_u8 *ptr;
    const m3c_u8 *newPtr;

    ptr = fragment->bFirst;
    newPtr = ptr;
    *pos = fragment->pos;

    while (newPtr <= fragment->bLast) {
        if (M3C_UTF8ValidateCodepoint(&newPtr, fragment->bLast) == M3C_UTF8_OK) {
            if (*ptr == '\n') {
                ++pos->line;
                pos->character = 0;
            } else
                ++pos->character;

            ptr = newPtr;
            continue;
        }

        fragment->bLast = ptr > fragment->bFirst ? ptr - 1 : M3C_NULL;
        return;
    }
}

/**
 * \brief Replaces an invalid UTF-8 sequence with replacement characters (`�`) for \ref
 * #__M3C_ASM_PreProc_EncodingRecoveryStage "encoding recovery stage".
 *
 * \details Tries to use the predefined string #__M3C_ASM_REPLACEMENT_STR, but if the sequence is
 * too long, it will allocate a new string and push it to the `stringPool`.
 *
 * ### Params:
 *
 * #### In:
 * + `fragment->pos`    - position of the first code point to process
 * + `fragment->bFirst` - pointer to the first code unit to process
 * + `fragment->bLast`  - pointer to the last code unit in the buffer
 *
 * #### Out:
 * + `fragment->pos`    - (*already set*) position of the first code point to process
 * + `fragment->bFirst` - pointer to the first replaced code unit or to the first code unit to
 * process if the length of invalid UTF-8 sequence is `0`
 * + `fragment->bLast`  - pointer to the last replaced code unit or `NULL` if the length of invalid
 * UTF-8 sequence is `0`
 * + `*ptr`             - pointer to the code unit right after the invalid UTF-8 sequence (even if
 * the length of invalid UTF-8 sequence is `0`)
 * + `*pos`             - position of code point right after the invalid UTF-8 sequence (even if the
 * length of invalid UTF-8 sequence is `0`)
 *
 * \param[in,out] fragment   fragment
 * \param[out]    pos        writes here position of the first code point right after the invalid
 * sequence (first valid code point or EOF)
 * \param[out]    ptr        writes here pointer to the first code unit right after the invalid
 * sequence (first valid code unit or EOF)
 * \param[in,out] stringPool string pool
 *
 * \return
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM failed to allocate or push a new string filled with replacement character to the
 * `stringPool`
 */
M3C_ERROR __M3C_ASM_PreProc_FillInvalidEncodingFragment(
    M3C_ASM_Fragment *fragment, M3C_ASM_Position *pos, const m3c_u8 **ptr,
    M3C_ASM_StringPool *stringPool
) {
    const m3c_u8 *newPtr;
    m3c_u32 oldCharacter; /* start character index */
    m3c_u32 chDelta;      /* we need to know how many characters `�` to fill in the sequence */
    M3C_UTF8Z badString; /* for the new allocated string if the sequence is too long to represent it
                           through another string */

    *pos = fragment->pos;
    oldCharacter = fragment->pos.character;

    *ptr = fragment->bFirst;
    newPtr = *ptr;

    while (newPtr <= fragment->bLast) {
        if (M3C_UTF8ValidateCodepoint(&newPtr, fragment->bLast) == M3C_UTF8_ERR) {
            ++pos->character; /* allways non-`\n` */
            *ptr = newPtr;
            continue;
        }
        break;
    }

    chDelta = pos->character - oldCharacter;
    if (chDelta > __M3C_ASM_REPLACEMENT_STR_CPLEN) {
        /* the invalid UTF-8 sequence is very long, we have to allocate a new string and put it in
         * the pool */

        badString = m3c_malloc(M3C_UTF8_REPLACEMENT_CHARACTER_BLEN * chDelta + 1);
        if (!badString)
            return M3C_ERROR_OOM;

        if (__M3C_ASM_StringPool_Push(stringPool, badString) != M3C_ERROR_OK) {
            m3c_free(badString);
            return M3C_ERROR_OOM;
        }

        /* fill the string with replacement char and do not forget about null at the end */
        m3c_memfill(badString, __M3C_ASM_REPLACEMENT_STR, 3, chDelta);
        badString[M3C_UTF8_REPLACEMENT_CHARACTER_BLEN * chDelta] = 0;

        fragment->bFirst = badString;
        fragment->bLast = &badString[M3C_UTF8_REPLACEMENT_CHARACTER_BLEN * chDelta - 1];

    } else if (chDelta > 0) {
        /* the invalid UTF-8 sequence is not very long, we can just point to
         * __M3C_ASM_REPLACEMENT_STR */

        fragment->bFirst = __M3C_ASM_REPLACEMENT_STR;
        fragment->bLast =
            __M3C_ASM_REPLACEMENT_STR + chDelta * M3C_UTF8_REPLACEMENT_CHARACTER_BLEN - 1;

    } else /* empty invalid sequense */
        fragment->bLast = M3C_NULL;

    return M3C_ERROR_OK;
}

/**
 * \brief Checks the contents of the file for compliance with UTF-8 encoding and replaces invalid
 * sequences with replacement character (`�`).
 *
 * \details This is the first stage of the preprocessor. It is expected to run even if the compiler
 * is run in non-preprocessor mode. Assumes that document fragments are empty.
 *
 * \param[in,out] preproc       preprocessor
 * \param         documentIndex document index in \ref M3C_ASM_PreProc::documents
 * "preproc.documents"
 *
 * \return (in case of an error, the content of \ref M3C_ASM_Document::fragments fragments is
 * undefined)
 * + #M3C_ERROR_OK
 * + #M3C_ERROR_OOM failed to allocate or push a new string filled with replacement character to the
 * \ref M3C_ASM_PreProc::stringPool preproc.stringPool
 */
M3C_ERROR __M3C_ASM_PreProc_EncodingRecoveryStage(M3C_ASM_PreProc *preproc, m3c_u32 documentIndex) {
    M3C_ASM_Fragment *goodFragment;
    M3C_ASM_Fragment *badFragment;

    M3C_ASM_Document *document = preproc->documents.data[documentIndex];
    M3C_ASM_Position pos = {0, 0};
    const m3c_u8 *ptr = document->bFirst;

    goodFragment = (M3C_ASM_Fragment *)m3c_malloc(sizeof(M3C_ASM_Fragment));
    if (!goodFragment)
        return M3C_ERROR_OOM;
    document->fragments.first = goodFragment;
    document->fragments.len = 1;

    M3C_LOOP {
        /* init fragment */
        goodFragment->bFirst = ptr;
        goodFragment->bLast = document->bLast;
        goodFragment->pos = pos;
        goodFragment->next = M3C_NULL;

        __M3C_ASM_PreProc_FillValidEncodingFragment(goodFragment, &pos);

        if (goodFragment->bLast >= document->bLast)
            return M3C_ERROR_OK; /* EOF */

        if (goodFragment->bLast == M3C_NULL) {
            /* valid sequence length is 0.
             * There is no need to allocate a new fragment. Just use goodFragment for invalid
             * sequence */

            goodFragment->bLast = document->bLast; /* restore from `NULL` */
            badFragment = goodFragment;

        } else {
            /* non-empty valid sequence.
             * we need to allocate a new fragment, since we know that there are still invalid
             * characters left */

            badFragment = (M3C_ASM_Fragment *)m3c_malloc(sizeof(M3C_ASM_Fragment));
            if (!badFragment)
                return M3C_ERROR_OOM;
            goodFragment->next = badFragment;
            ++document->fragments.len;

            badFragment->bFirst = goodFragment->bLast + 1;
            badFragment->bLast = document->bLast;
            badFragment->pos = pos;
            badFragment->next = M3C_NULL;
        }

        if (__M3C_ASM_PreProc_FillInvalidEncodingFragment(
                badFragment, &pos, &ptr, &preproc->stringPool
            ) == M3C_ERROR_OOM) {
            /* NOTE: but bad fragment is still attched. It can be the first fragment
             * ever or not */
            return M3C_ERROR_OOM;
        }

        if (ptr > document->bLast) {
            return M3C_ERROR_OK; /* EOF */
        }

        goodFragment = (M3C_ASM_Fragment *)m3c_malloc(sizeof(M3C_ASM_Fragment));
        if (!goodFragment)
            return M3C_ERROR_OOM;
        badFragment->next = goodFragment;
        ++document->fragments.len;
    }
}

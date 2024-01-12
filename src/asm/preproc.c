#include <m3c/asm/preproc.h>

#include <m3c/common/coltypes.h>
#include <m3c/common/utf8.h>
#include <m3c/common/macros.h>

#include <m3c/rt/alloc.h>

#include <m3c/asm/lex.h>

void M3C_ASM_Document_Init(M3C_ASM_Document *document, m3c_u8 const *buf, m3c_size_t bufLen) {
    M3C_VEC_INIT(&document->tokens);
    __M3C_Diagnostics_Init(&document->diagnostics);

    document->fragments.data = M3C_NULL;
    document->fragments.len = 0;

    document->bFirst = buf;
    document->bLast = bufLen > 0 ? buf + bufLen - 1 : M3C_NULL;
}

void M3C_ASM_Document_Deinit(M3C_ASM_Document const *document) {
    M3C_VEC_DEINIT(&document->tokens);
    __M3C_Diagnostics_Deinit(&document->diagnostics);

    M3C_ARR_DEINIT_BOXED(&document->fragments);

    /* NOTE: no free for document buf (`::bFirst`). We don't own it. */
}

M3C_ERROR M3C_ASM_PreProc_New(M3C_ASM_PreProc *preProc) {

    /* NOTE: we need at least one document as the entry document */
    if (M3C_VEC_NEW_WITH_CAP(M3C_ASM_Document, &preProc->documents, 2) != M3C_ERROR_OK)
        return M3C_ERROR_OOM;

    M3C_VEC_INIT(&preProc->stringPool);

    __M3C_ASM_PPSeq_Init(&preProc->seq);

    return M3C_ERROR_OK;
}

void M3C_ASM_PreProc_Deinit(M3C_ASM_PreProc const *preProc) {
    m3c_size_t i;
    M3C_ASM_Document const *document;

    M3C_VEC_FOREACH(&preProc->documents, &i, &document) { M3C_ASM_Document_Deinit(document); }
    M3C_VEC_DEINIT(&preProc->documents);

    M3C_VEC_DEINIT(&preProc->stringPool);

    __M3C_ASM_PPSeq_Deinit(&preProc->seq);
}

M3C_ERROR __M3C_ASM_Document_SplitLines(M3C_ASM_Document *document, m3c_bool usePreproc) {
    typedef M3C_VEC(M3C_ASM_Fragment) M3C_ASM_Fragments;

    M3C_UCP cp;
    M3C_ASM_Fragments vec;
    M3C_ASM_Fragment *fragment;
    M3C_ASM_Fragment newFragment;

    m3c_u8 const *ptr0;
    m3c_u8 const *nextFragmentPtr;

    m3c_size_t cpLen0;
    m3c_size_t cpLen1;
    m3c_size_t cpLenB;

    M3C_ASM_Position pos = {0, 0};

    if (document->fragments.len)
        return M3C_ERROR_OK;

    vec.data = (M3C_ASM_Fragment *)m3c_malloc(sizeof(M3C_ASM_Fragment) * 2);
    if (!vec.data)
        return M3C_ERROR_OOM;
    vec.cap = 2;
    vec.len = 0;

    /* init first fragment */
    vec.len = 1;
    fragment = &vec.data[0];
    fragment->bFirst = document->bFirst;
    fragment->bLast = document->bLast;
    fragment->pos = pos;

    ptr0 = document->bFirst;

    M3C_LOOP {

        if (M3C_UTF8ReadCodepointWithLen(ptr0, fragment->bLast, &cp, &cpLen0) == M3C_ERROR_EOF)
            break;

        if (cp == '\r') {

            if (M3C_UTF8ReadCodepointWithLen(ptr0 + cpLen0, fragment->bLast, &cp, &cpLen1) ==
                    M3C_ERROR_OK &&
                cp == '\n')
                nextFragmentPtr = ptr0 + cpLen0 + cpLen1;
            else
                nextFragmentPtr = ptr0 + cpLen0;

            if (usePreproc && (M3C_UTF8ReadBackCodepointWithLen(
                                   ptr0, fragment->bFirst, fragment->bLast, &cp, &cpLenB
                               ) == M3C_ERROR_OK &&
                               cp == '\\')) {
                fragment->bLast = ptr0 - cpLenB == fragment->bFirst ? M3C_NULL : ptr0 - cpLenB - 1;
            } else
                fragment->bLast = nextFragmentPtr - 1;

            goto cut;

        } else if (cp == '\n') {

            nextFragmentPtr = ptr0 + cpLen0;

            if (usePreproc &&
                M3C_UTF8ReadBackCodepointWithLen(
                    ptr0, fragment->bFirst, fragment->bLast, &cp, &cpLenB
                ) == M3C_ERROR_OK &&
                cp == '\\') {
                fragment->bLast = ptr0 - cpLenB == fragment->bFirst ? M3C_NULL : ptr0 - cpLenB - 1;
            } else {
                fragment->bLast = nextFragmentPtr - 1;
            }

            goto cut;
        }

        ptr0 += cpLen0;
        ++pos.character;
        continue;

    cut:
        /* NOTE: params - nextFragmentPtr, fragment->bLast */

        if (nextFragmentPtr > document->bLast)
            break;

        if (M3C_VEC_PUSH(M3C_ASM_Fragment, &vec, &newFragment) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
        fragment = &vec.data[vec.len - 1];

        ++pos.line;
        pos.character = 0;

        fragment->bFirst = nextFragmentPtr;
        fragment->bLast = document->bLast;
        fragment->pos = pos;

        ptr0 = nextFragmentPtr;
        continue;
    }

    /* fill the fragment cache */
    document->fragments.data = vec.data;
    document->fragments.len = vec.len;

    return M3C_ERROR_OK;
}

#include <m3c/asm/preproc.h>

#include <m3c/common/coltypes.h>
#include <m3c/common/utf8.h>
#include <m3c/rt/alloc.h>

M3C_ERROR __M3C_ASM_Document_ContinuationLineCollapsingPhase(M3C_ASM_Document *document) {
    typedef M3C_VEC(M3C_ASM_Fragment) M3C_ASM_Fragments;
    M3C_ASM_Fragments vec;
    M3C_ASM_Fragment *fragment;
    M3C_ASM_Fragment newFragment;
    M3C_ASM_Position pos = {0, 0};
    m3c_u8 const *ptr;
    m3c_size_t x;

    /* init vec */
    /* LATER: do we assume that line continuation is a very rarely used feature? Why use 2 as
     * capacity? */
    vec.data = (M3C_ASM_Fragment *)m3c_malloc(sizeof(M3C_ASM_Fragment) * 2);
    if (!vec.data)
        return M3C_ERROR_OOM;
    vec.cap = 2;
    vec.len = 1;

    /* init first fragment */
    vec.data->bFirst = document->bFirst;
    vec.data->bLast = document->bLast;
    vec.data->pos = pos;

    ptr = document->bFirst;

    /* NOTE: if the document is empty, `document->bLast` will be `NULL` */
    while (ptr <= document->bLast) {
        if (*ptr == '\n') {
            fragment = &vec.data[vec.len - 1];

            if (ptr - fragment->bFirst >= 1 && ptr[-1] == '\\') {
                /* '\\\n' */

                x = 2;
                goto lc;

            } else if (ptr - fragment->bFirst >= 2 && ptr[-1] == '\r' && ptr[-2] == '\\') {
                /* '\\\r\n' */

                x = 3;
                goto lc;

            } else {
                /* just '\n` */
                ++ptr;
                ++pos.line;
                pos.character = 0;
            }
        } else {
            /* not `\n` but can by multibyte */
            M3C_UTF8ValidateCodepoint(&ptr, document->bLast);
            ++pos.character;
        }
        continue;

    lc:
        fragment->bLast = ptr - fragment->bFirst >= x ? ptr - x : M3C_NULL;

        if (ptr == document->bLast)
            break; /* EOF */

        /* seek to the next char */
        ++ptr;
        ++pos.line;
        pos.character = 0;

        /* init newFragment */
        newFragment.bFirst = ptr;
        newFragment.bLast = document->bLast;
        newFragment.pos = pos;

        /* push to the vector */
        if (M3C_VEC_PUSH(M3C_ASM_Fragment, &vec, &newFragment) != M3C_ERROR_OK)
            return M3C_ERROR_OOM;
    }

    /* LATER: shrink the vec? */

    /* fill the fragment cache */
    document->fragments.data = vec.data;
    document->fragments.len = vec.len;

    return M3C_ERROR_OK;
}

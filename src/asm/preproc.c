#include <m3c/asm/preproc.h>

#include <m3c/common/coltypes.h>
#include <m3c/common/utf8.h>
#include <m3c/common/macros.h>
#include <m3c/rt/alloc.h>

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

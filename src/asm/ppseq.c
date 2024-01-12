#include <m3c/asm/ppseq.h>

#include <m3c/asm/lex.h>

void __M3C_ASM_PPSeq_Init(M3C_ASM_PPSeq *ppSeq) {
    M3C_VEC_INIT(&ppSeq->toks);
    __M3C_Diagnostics_Init(&ppSeq->diags);
}

void __M3C_ASM_PPSeq_Deinit(M3C_ASM_PPSeq const *ppSeq) {
    M3C_VEC_DEINIT(&ppSeq->toks);
    __M3C_Diagnostics_Deinit(&ppSeq->diags);
}

#ifndef _M3C_INCGUARD_ASM_TYPES_H
#define _M3C_INCGUARD_ASM_TYPES_H

#include <m3c/common/types.h>
#include <m3c/common/coltypes.h>

/***************************************************************************************************
 * forward declarations from <m3c/asm/lex.h>
 **************************************************************************************************/

typedef struct __tagM3C_ASM_Token M3C_ASM_Token;

typedef M3C_VEC(M3C_ASM_Token) M3C_ASM_Tokens;

/***************************************************************************************************
 * forward declarations from <m3c/asm/preproc.h>
 **************************************************************************************************/

/**
 * \brief Handle of include information.
 */
typedef m3c_u16 M3C_hINCLUDE;

typedef struct __tagM3C_ASM_Document M3C_ASM_Document;

/***************************************************************************************************
 * some common types for this module
 **************************************************************************************************/

/**
 * \brief Position (in the source document).
 *
 * \note Implementation limits:
 * + a document can contain only \f$2^{16}-1\f$ lines: \ref M3C_ASM_Position::line "line" is `u16`
 * + each line of the document can contain only \f$2^{16}-1\f$ characters: \ref
 * M3C_ASM_Position::character "character" is `u16`
 */
typedef struct __tagM3C_ASM_Position {
    /**
     * \brief Zero-based index of the line.
     */
    m3c_u16 line;
    /**
     * \brief Zero-based index of the character in the #line.
     */
    m3c_u16 character;
} M3C_ASM_Position;

#endif /* _M3C_INCGUARD_ASM_TYPES_H */

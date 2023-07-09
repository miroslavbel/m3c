#ifndef _M3C_INCGUARD_MACRO_H
#define _M3C_INCGUARD_MACRO_H

#include <m3c/common/types.h>

#define m3c_min(x, y) ((x) < (y)) ? (x) : (y)
#define m3c_max(x, y) ((x) > (y)) ? (x) : (y)

/**
 * \brief Checks that `x` is in the range `[min..max]` (both inclusive).
 */
#define M3C_InRange(x, min, max) ((min) <= (x) && (x) <= (max))

#define M3C_LOOP while (m3c_true)

#define M3C_IfRet(res, call)                                                                       \
    res = (call);                                                                                  \
    if (res)                                                                                       \
    return res

#endif /* _M3C_INCGUARD_MACRO_H */

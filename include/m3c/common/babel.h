#ifndef _M3C_INCGUARD_BABEL_H
#define _M3C_INCGUARD_BABEL_H

#if __STDC_VERSION__ >= 199901L
#    define m3c_restrict restrict
#else
#    define m3c_restrict /* just ignore before C99 */
#endif

#endif /* _M3C_INCGUARD_BABEL_H */

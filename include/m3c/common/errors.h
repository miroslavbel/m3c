#ifndef _M3C_INCGUARD_ERRORS_H
#define _M3C_INCGUARD_ERRORS_H

/**
 * \brief General enum for errors.
 */
typedef enum __tagM3C_ERROR {
    /**
     * \brief There are no errors.
     */
    M3C_ERROR_OK = 0,
    /**
     * \brief Out Of Memory.
     */
    M3C_ERROR_OOM = 1,
    /**
     * \brief End Of File.
     */
    M3C_ERROR_EOF = 2,
    /**
     * \brief Invalid encoding.
     */
    M3C_ERROR_INVALID_ENCODING = 3,
    /**
     * \brief There is no object corresponding to this handle.
     */
    M3C_ERROR_BAD_HANDLE = 4
} M3C_ERROR;

#endif /* _M3C_INCGUARD_ERRORS_H */

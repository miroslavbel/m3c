#include <m3c/rt/mem.h>

void *m3c_memfill(
    void *m3c_restrict dest, const void *m3c_restrict src, m3c_size_t len, m3c_size_t count
) {
    const char *end = (char *)dest + len * count; /* P+N */
    char *ptr = dest;
    for (; ptr < end; ptr += len) {
        m3c_memcpy(ptr, src, len);
    }
    return dest;
}

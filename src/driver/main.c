#include <m3c/rt/main.h>

/**
 * \warning Not a stdlib `main` function. When the result is returned, the process is immediately
 * terminated (e.g. no `atexit` functions called).
 */
int main(int argc, char *argv[]) { return 0; }

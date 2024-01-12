#include <m3c/core/diagnostics.h>

void __M3C_Diagnostics_Init(M3C_Diagnostics *diagnostics) {
    M3C_VEC_INIT(&diagnostics->vec);
    diagnostics->warnings = 0;
    diagnostics->errors = 0;
}

void __M3C_Diagnostics_Deinit(M3C_Diagnostics const *diagnostics) {
    M3C_VEC_DEINIT(&diagnostics->vec);
}

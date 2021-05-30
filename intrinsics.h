#ifdef _MSC_VER
#include <intrin.h>

static inline int __builtin_ctz(unsigned x) {
    unsigned long ret;
    _BitScanForward(&ret, x);
    return (int)ret;
}

#endif
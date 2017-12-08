#ifndef KRNLSYMS_H
#define KRNLSYMS_H

#include <corelib/types.h>

struct symEntry {
    u64 addr;
    const char *func;
};

extern struct symEntry symTable[];

#endif // KRNLSYMS_H

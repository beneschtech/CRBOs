#ifndef VM86INTR_H
#define VM86INTR_H

#include <corelib/types.h>

struct vm86intr {
    u16 intnum;
    u16 di;
    u16 si;
    u16 bp;
    u16 sp;
    u16 bx;
    u16 dx;
    u16 cx;
    u16 ax;
    u16 ds;
    u16 es;
} __attribute__((packed));

void dovm86intr(struct vm86intr *);

#endif // VM86INTR_H

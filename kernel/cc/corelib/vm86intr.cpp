#include <CRBOs.h>

#include "vm86intr.h"

void dovm86intr(vm86intr *d)
{
    d->sp = 0x3ffa;
    memcpy((void *)(0x4000-sizeof(struct vm86intr)),d,sizeof(struct vm86intr));
    asm volatile("call 0x4000":::"rax","rbx","rcx","rdx","rsi","rdi","rbp");
    memcpy(d,(void *)(0x4000-sizeof(struct vm86intr)),sizeof(struct vm86intr));
    KernelInstance->interrupts.renewIDT();
}

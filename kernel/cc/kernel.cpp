#include <CRBOs.h>
#include <corelib/krnlsyms.h>

Kernel *KernelInstance;
void _start();

Kernel::~Kernel()
{
    printStackTrace("Kernel Object Destroyed");
    while (1) ;
}

Kernel::Kernel()
{

}

void Kernel::boot()
{
    console.initMsg();
    console.printf("Booting kernel at address: %p (%x+%x)\n",this,((u64)this-(u64)_start),sizeof(Kernel));
    mem.init();
    interrupts.init();
    acpi.init();
}

const char *_functionForAddr(u64 addr)
{
    struct symEntry *sp = symTable;
    if (addr < sp->addr)
        return NULL;
    while (sp->addr)
    {
        if (sp[1].addr)
        {
            if (addr >= sp->addr && addr <= sp[1].addr)
                return sp->func;
        } else {
            if (addr >= sp->addr && addr <= (u64)KernelInstance)
                return sp->func;
        }
        sp++;
    }
    return NULL;
}

void printStackTrace(const char *msg, i64 offst)
{
    KernelInstance->console.printf("\n*ERROR: %s*\nStack Trace:\n",msg);

    u64 sp = NULL,spbt = NULL;
    asm volatile("mov %%rbp,%0":"=r"(sp));
    spbt = sp;
    sp += offst;

    u64 immptr = *(u64 *)(sp + 8);
    KernelInstance->console.printf("==> %p (%s)\n",immptr,_functionForAddr(immptr));
    // Stack walk to where we are above the one we just printed
    while (*(u64 *)spbt < sp)
       spbt = *(u64 *)spbt;
    sp = spbt;
    while (immptr > 0x100000 && immptr < (u64)KernelInstance)
    {
        sp = *(u64 *)sp;
        if (sp > 0x100000 && sp < (u64)KernelInstance)
        {
           immptr = *(u64 *)(sp + 8);
           KernelInstance->console.printf("    %p (%s)\n",immptr,_functionForAddr(immptr));
        }
    }
}

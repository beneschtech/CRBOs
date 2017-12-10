#include <CRBOs.h>
#include <corelib/vm86intr.h>
extern unsigned long _mallocBase;

void _start()
{
    asm volatile ("mov %%RDI,%0":"=r" (_mallocBase));
    _mallocBase >>= 5;
    _mallocBase++;
    _mallocBase <<= 5;
    KernelInstance = new Kernel();
    KernelInstance->boot();

    while(1) asm volatile("hlt");

    delete KernelInstance;
}

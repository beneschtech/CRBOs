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
    KernelInstance->acpi.apic->startLocalApic();

    KernelInstance->acpi.apic->setIRQVector(1,0x21,false);
    KernelInstance->console.printf("Press any key to end (and test IRQ1 through the APIC)...\n");
    asm volatile ("hlt");
    delete KernelInstance;
}

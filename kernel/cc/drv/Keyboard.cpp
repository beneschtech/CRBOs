#include "Keyboard.h"

DrvKeyboard *DrvKeyboard::myInstance = NULL;

DrvKeyboard::DrvKeyboard(Driver *p)
{
    setParent(p);
    myInstance = this;
    KernelInstance->interrupts.addDriverToIRQChain(1,(void *)&DRIVERCHAINFUNC(DrvKeyboard));
    startupMessage(true);
}

void DrvKeyboard::handle()
{
    KernelInstance->console.printf("%s: ",type());
    while (inb(0x64) & 1)
        KernelInstance->console.printf("%x ",inb(0x60));
    KernelInstance->console.printf("\n");
}

void DRIVERCHAINFUNC(DrvKeyboard)()
{
    DrvKeyboard::instance()->handle();
}

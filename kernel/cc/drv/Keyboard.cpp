#include "Keyboard.h"

DrvKeyboard *DrvKeyboard::myInstance = NULL;

void _irqEntry1();

DrvKeyboard::DrvKeyboard(Driver *p)
{
    setParent(p);
    myInstance = this;
    KernelInstance->interrupts.addDriverToIRQChain(1,(void *)&DRIVERCHAINFUNC(DrvKeyboard));
    KernelInstance->apic.setIRQVector(1,0x21,false);
    KernelInstance->interrupts.setISR(0x21,(u64)_irqEntry1);
    startupMessage(true);

    setLEDs(7);
    msleep(750);
    setLEDs(0);
}

void DrvKeyboard::handle()
{
    u8 p = 0;
    u64 scode = 0;
    while ((p = inb(0x64)) & 1)
    {
        scode <<= 8;
        scode += inb(0x60);
        msleep(10);
    }
    if (scode == 0xfe || !scode)
        return;

    KernelInstance->console.printf("%x\n",scode);
}

void DRIVERCHAINFUNC(DrvKeyboard)()
{
    DrvKeyboard::instance()->handle();
}

bool DrvKeyboard::sendKBDCommand(u8 cmd, u8 data)
{
    int tries = 5;

    outb(0x64,cmd);
    msleep(10);
    while (tries)
    {
        outb(0x60,data);
        msleep(10);
        while (!(inb(0x64) & 1))
            msleep(10);
        u8 rv = inb(0x60);
        if (rv == 0xFA)
            return true;

        if (rv != 0xFE)
        {
            KernelInstance->console.printf("Keyboard Error! CMD: %x DATA: %x RESP: %x\n",cmd,data,rv);
            return false;
        }
        tries--;
    }
    return false;
}

void DrvKeyboard::setLEDs(u8 ledstat)
{
    sendKBDCommand(0xed,ledstat);
}

bool DrvKeyboard::echo()
{
    outb(0x64,0xee);
    msleep(10);
    while (!(inb(0x64) & 1))
        msleep(10);
    return (inb(0x60) == 0xee);
}

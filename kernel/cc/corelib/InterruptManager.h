#ifndef INTERRUPTMANAGER_H
#define INTERRUPTMANAGER_H

#include <corelib/types.h>

#define ISR_FUNCTION(isrname) \
    void ISR##isrname(); \
    void isrname()

#define ISR_ADDR(isrname) &::InterruptManager::ISR##isrname

class Driver;
struct irqChainDef {
    u8 nHooks;
    void(*hooks[24])();
};

class Kernel;
class InterruptManager
{
friend class Kernel;
public:
    InterruptManager();
    ~InterruptManager();
    void setISR(u8 inum,void (InterruptManager::*isr)());
    void setISR(u8 inum,u64 isr,u16 segment=0x28);
    void renewIDT();
    void setupIRQs();
    void addDriverToIRQChain(u8 irq,void *ptr);
    struct irqChainDef *irqTbl[32];
    static void irqChain(u64 irqno);

private:
    void init();
    void nullISR();

    ISR_FUNCTION(isr0);
    ISR_FUNCTION(isr1);
    ISR_FUNCTION(isr2);
    ISR_FUNCTION(isr3);
    ISR_FUNCTION(isr4);
    ISR_FUNCTION(isr5);
    ISR_FUNCTION(isr6);
    ISR_FUNCTION(isr7);
    ISR_FUNCTION(isr8);
    ISR_FUNCTION(isr9);
    ISR_FUNCTION(isr10);
    ISR_FUNCTION(isr11);
    ISR_FUNCTION(isr12);
    ISR_FUNCTION(isr13);
    ISR_FUNCTION(isr14);
    ISR_FUNCTION(isr16);
    ISR_FUNCTION(isr17);
    ISR_FUNCTION(isr18);
    ISR_FUNCTION(isr19);
    ISR_FUNCTION(isr20);
    ISR_FUNCTION(isr30);

};

#endif // INTERRUPTMANAGER_H

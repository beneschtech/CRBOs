#include <corelib/memutil.h>
#include <CRBOs.h>

struct IDTEntry
{
    u16 offsetLow;
    u16 selector;
    u8 ist;
    u8 type;
    u16 offsetMid;
    u32 offsetHi;
    u32 pad;
} __attribute__((packed));

struct IDT
{
    IDTEntry entries[256];
    u16 size;
    u64 addr;
} __attribute__((packed));

static struct IDT myIDT __attribute__ ((aligned(16)));

InterruptManager::~InterruptManager()
{

}

InterruptManager::InterruptManager() { }

void InterruptManager::init()
{
    outb(0xa1,0xff);
    outb(0x21,0xff);
    memset(&myIDT,0,sizeof(IDT));
    memset(irqTbl,0,sizeof(irqTbl));
    myIDT.size = sizeof(IDTEntry) * 256;
    myIDT.addr = (u64)&myIDT.entries;
    for (int i =1; i < 256; i++)
        setISR(i,&InterruptManager::nullISR);
    renewIDT();
    setISR(0,ISR_ADDR(isr0));
    setISR(1,ISR_ADDR(isr1));
    setISR(2,ISR_ADDR(isr2));
    setISR(3,ISR_ADDR(isr3));
    setISR(4,ISR_ADDR(isr4));
    setISR(5,ISR_ADDR(isr5));
    setISR(6,ISR_ADDR(isr6));
    setISR(7,ISR_ADDR(isr7));
    setISR(8,ISR_ADDR(isr8));
    setISR(9,ISR_ADDR(isr9));
    setISR(10,ISR_ADDR(isr10));
    setISR(11,ISR_ADDR(isr11));
    setISR(12,ISR_ADDR(isr12));
    setISR(13,ISR_ADDR(isr13));
    setISR(14,ISR_ADDR(isr14));
    setISR(16,ISR_ADDR(isr16));
    setISR(17,ISR_ADDR(isr17));
    setISR(18,ISR_ADDR(isr18));
    setISR(19,ISR_ADDR(isr19));
    setISR(20,ISR_ADDR(isr20));
    setISR(30,ISR_ADDR(isr30));

    setISR(0x21,&InterruptManager::_irqEntry1);
}
void InterruptManager::setISR(u8 inum, u64 isrAddr, u16 segment)
{
    struct IDTEntry *isrp = &myIDT.entries[inum];
    isrp->ist = 0;
    isrp->offsetHi = isrAddr >> 32;
    isrp->offsetLow = isrAddr & 0xffff;
    isrp->offsetMid = (isrAddr >> 16) & 0xffff;
    isrp->pad = 0;
    isrp->selector = segment;
    isrp->type = 0x8e;
}

void InterruptManager::setISR(u8 inum, void (InterruptManager::*isr)())
{
    u64 isrAddr = 0;
    // One of surprisingly few C++ workarounds, it cant cast pointers around like C can,but we can do it in ASM cause
    // the processor only cares about numbers, like my boss
    asm volatile ("movq %1,%0":"=r"(isrAddr):"r"(isr));
    struct IDTEntry *isrp = &myIDT.entries[inum];
    isrp->ist = 0;
    isrp->offsetHi = isrAddr >> 32;
    isrp->offsetLow = isrAddr & 0xffff;
    isrp->offsetMid = (isrAddr >> 16) & 0xffff;
    isrp->pad = 0;
    isrp->selector = 0x28;
    isrp->type = 0x8e;
}

void InterruptManager::renewIDT()
{
    asm volatile("cli\n"
                 "lidt %0\n"
                 "sti"::"m"(myIDT.size));
}

void InterruptManager::irqChain(u64 irqno)
{
    struct irqChainDef *cp = KernelInstance->interrupts.irqTbl[irqno];
    if (!cp)
        return;
    for (u8 i = 0; i < cp->nHooks; i++)
    {
        cp->hooks[i]();
    }
    KernelInstance->apic.EOI();
}

void InterruptManager::addDriverToIRQChain(u8 irq, void *driver)
{
    if (!irqTbl[irq])
        irqTbl[irq] = (struct irqChainDef *)kmalloc(sizeof(struct irqChainDef));
    volatile struct irqChainDef *cp = irqTbl[irq];
    cp->hooks[cp->nHooks++] =(void(*)())driver;
}

void InterruptManager::setupIRQs()
{
    KernelInstance->apic.startLocalApic();
    KernelInstance->apic.setIRQVector(1,0x21,0);
}

// The rest of these are just fatal CPU exceptions, modify after userspace to produce coredump, unless in kernel
void InterruptManager::isr0()
{
    printStackTrace("Division by zero",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr1()
{
    printStackTrace("Debug",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr2()
{
    printStackTrace("NMI",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr3()
{
    printStackTrace("Breakpoint",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr4()
{
    printStackTrace("Overflow",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr5()
{
    printStackTrace("Bound Range Exceeded",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr6()
{
    printStackTrace("Invalid Opcode",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr7()
{
    printStackTrace("Device not available",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr8()
{
    printStackTrace("Double Fault",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr9()
{
    printStackTrace("Coprocesor Segment Overrun",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr10()
{
    printStackTrace("Invalid TSS",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr11()
{
    printStackTrace("Segment Not Present",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr12()
{
    printStackTrace("Stack-Segment Fault",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr13()
{
    printStackTrace("General Protection Fault",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr14()
{
    u64 cr2 = 0;
    asm volatile ("mov %%cr2,%0":"=r"(cr2));
    KernelInstance->console.printf("Attempted access of %p",cr2);
    printStackTrace("Page Fault",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr16()
{
    printStackTrace("x87 Floating Point Exception",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr17()
{
    printStackTrace("Alignment Check",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr18()
{
    printStackTrace("Machine Check",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr19()
{
    printStackTrace("SIMD Floating Point Exception",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr20()
{
    printStackTrace("Virtualization Exception",24);
    asm volatile("cli\nhlt");
}

void InterruptManager::isr30()
{
    printStackTrace("Security Exception",24);
    asm volatile("cli\nhlt");
}

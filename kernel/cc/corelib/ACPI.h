#ifndef ACPI_H
#define ACPI_H

#include <corelib/types.h>

#define APIC_APICID 0x20
#define APIC_APICVER	0x30
#define APIC_TASKPRIOR	0x80
#define APIC_EOI	0x0B0
#define APIC_LDR	0x0D0
#define APIC_DFR	0x0E0
#define APIC_SPURIOUS	0x0F0
#define APIC_ESR	0x280
#define APIC_ICRL	0x300
#define APIC_ICRH	0x310
#define APIC_LVT_TMR	0x320
#define APIC_LVT_PERF	0x340
#define APIC_LVT_LINT0	0x350
#define APIC_LVT_LINT1	0x360
#define APIC_LVT_ERR	0x370
#define APIC_TMRINITCNT	0x380
#define APIC_TMRCURRCNT	0x390
#define APIC_TMRDIV	0x3E0
#define APIC_LAST	0x38F
#define APIC_DISABLE	0x10000
#define APIC_SW_ENABLE	0x100
#define APIC_CPUFOCUS	0x200
#define APIC_NMI	(4<<8)
#define TMR_PERIODIC	0x20000
#define TMR_BASEDIV	(1<<20)

#define LAPIC_WRITE(reg,val) *(u32 volatile *)((u64)KernelInstance->apic.localControllerAddress + reg) = val
#define LAPIC_READ(reg) *(u32 volatile *)((u64)KernelInstance->apic.localControllerAddress + reg)

class Kernel;

struct IOAPIC_Desc {
    u8 id;
    void *base;
    u32 irqBase;
    u32 irqEnd;
};

class APIC
{
    friend class Kernel;
public:
    APIC();
    ~APIC() { }
    void parse(void *);
    void startLocalApic();
    void writeIOApic(u8 index, u32 reg, u32 val);
    u32 readIOApic(u8 index, u32 reg);
    void setIRQVector(u8 irq,u8 vec,bool mask);
    void EOI();

    u32 irqOverrides[32];
    void *localControllerAddress;
    u8 numCPUs;
    u8 cpuAPICIds[256];
    u8 numIOAPICs;
    struct IOAPIC_Desc ioApics[256];
};

class ACPI
{
    friend class Kernel;

public:
    ACPI() {}
    ~ACPI() {}

private:
    void init();
    void *rsdp;
    void *rsdt;
    void *xsdt;
    void *tables[64];
};

#endif // ACPI_H

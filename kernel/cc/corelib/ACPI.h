#ifndef ACPI_H
#define ACPI_H

#include <corelib/types.h>

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

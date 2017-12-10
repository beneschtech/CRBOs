#include <CRBOs.h>

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

#define LAPIC_WRITE(reg,val) *(u32 volatile *)((u64)localControllerAddress + reg) = val
#define LAPIC_READ(reg) *(u32 volatile *)((u64)localControllerAddress + reg)

APIC::APIC()
{

}

void APIC::parse(void *p)
{
    memset(cpuAPICIds,0,sizeof(cpuAPICIds));
    numIOAPICs = 0;
    numCPUs = 0;
    memset(ioApics,0,sizeof(ioApics));
    for (int i =0; i < 32; i++)
        irqOverrides[i] = i;

    localControllerAddress = NULL;
    u64 addrv = (u64)p;
    u32 tlen = *(u32 *)(addrv + 4);
    u64 addre = addrv + tlen;
    localControllerAddress = dwtoptr(*(u32 *)(addrv + 36));
    KernelInstance->console.printf("    Local APIC Address: %p\n",localControllerAddress);
    KernelInstance->mem.mapIdentityPages((u64)localControllerAddress,4096,0x1B);
    if (*(u32 *)(addrv + 40) & 1)
        KernelInstance->console.printf("    8259 Compatibilty (already disabled)\n");
    addrv += 44;
    while (addrv < addre)
    {
        u8 len = *(u8 *)(addrv+1);
        u8 type = *(u8 *)addrv;
        switch (type)
        {
        case 0:
            if ((*(u32 *)(addrv + 4)) & 1)
            {
               cpuAPICIds[numCPUs] = *(u8 *)(addrv + 3);
               numCPUs++;
            }
            break;
        case 1:
            ioApics[numIOAPICs].id = *(u8 *)(addrv + 2);
            ioApics[numIOAPICs].base = dwtoptr(*(u32 *)(addrv + 4));
            KernelInstance->mem.mapIdentityPages((u64)dwtoptr(*(u32 *)(addrv + 4)),4096,0x1B);
            ioApics[numIOAPICs].irqBase = *(u32 *)(addrv + 8);
            ioApics[numIOAPICs].irqEnd = ioApics[numIOAPICs].irqBase + ((readIOApic(numIOAPICs,0x1) >> 16)&0xff);
            numIOAPICs++;
            break;

        case 2:
            irqOverrides[*(u8 *)(addrv+3)] = *(u32 *)(addrv + 4);
            break;

        default:   {}
        }
        addrv += len;
    }
    KernelInstance->console.printf("    %d CPUs found. APIC IDs: ",numCPUs);
    for (int i =0; i < numCPUs; i++)
        KernelInstance->console.printf("%x ",cpuAPICIds[i]);
    KernelInstance->console.printf("\n");
    KernelInstance->console.printf("    %d IOAPICS found:\n",numIOAPICs);
    for (int i =0; i < numIOAPICs; i++) {
        struct IOAPIC_Desc *ip = &ioApics[i];
        KernelInstance->console.printf("     ID: %d Base: %p IRQ Base: %d IRQ End: %d\n",ip->id,ip->base,ip->irqBase,ip->irqEnd);
    }
    KernelInstance->console.printf("    Redirection Entries Found:\n");
    for (u32 i = 0; i < 32; i++)
    {
        if (irqOverrides[i] != i)
            KernelInstance->console.printf("     %d => %d\n",i,irqOverrides[i]);
    }
}

void APIC::startLocalApic()
{
    // Initialize APIC to known state from wiki.osdev.org/APIC_timer
    LAPIC_WRITE(APIC_DFR,0xffffffff);
    u32 ldrval = LAPIC_READ(APIC_LDR);
    ldrval &= 0x0ffffff;
    ldrval |= 1;
    LAPIC_WRITE(APIC_LDR,ldrval);
    LAPIC_WRITE(APIC_LVT_TMR,APIC_DISABLE);
    LAPIC_WRITE(APIC_LVT_PERF,APIC_NMI);
    LAPIC_WRITE(APIC_LVT_LINT0,APIC_DISABLE);
    LAPIC_WRITE(APIC_LVT_LINT1,APIC_DISABLE);
    LAPIC_WRITE(APIC_TASKPRIOR,0);
    asm volatile (
                "movq $0x1b,%%rcx\n"
                "rdmsr\n"
                "bts $11,%%rax\n"
                "wrmsr"
                  :::"rax");
    LAPIC_WRITE(APIC_SPURIOUS,0x1ff);
}

void APIC::writeIOApic(u8 index, u32 reg, u32 val)
{
    u32 volatile *ioapic = (u32 *)ioApics[index].base;
    ioapic[0] = (reg & 0xff);
    ioapic[4] = val;
}

u32 APIC::readIOApic(u8 index, u32 reg)
{
    u32 volatile *ioapic = (u32 *)ioApics[index].base;
    ioapic[0] = (reg & 0xff);
    return ioapic[4];
}

void APIC::setIRQVector(u8 irq, u8 vec, bool mask)
{
    u8 realIRQ = irqOverrides[irq];
    u8 idx = 0;
    do {
        if (ioApics[idx].irqBase <= realIRQ && ioApics[idx].irqEnd >= realIRQ)
            break;
        idx++;
    } while (idx < numIOAPICs);

    realIRQ -= ioApics[idx].irqBase;
    u8 reg = 0x10 + (realIRQ * 2);
    u32 ioalo = vec; // + 0x100; // Vector + system management mode, all other fields, 0 is appropriate
    if (mask)
        ioalo |= (1 << 16);
    u32 ioahi = cpuAPICIds[0] << 24;
    writeIOApic(idx,reg,ioalo);
    writeIOApic(idx,reg+1,ioahi);
}

void APIC::EOI()
{
    LAPIC_WRITE(0xb0,0);
}

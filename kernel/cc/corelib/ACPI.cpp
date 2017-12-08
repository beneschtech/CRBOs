#include <CRBOs.h>

extern Kernel *KernelInstance;

struct _acpi_rsdp {
    char sig[8];
    u8 cksum;
    char oemid[6];
    u8 rev[1];
    u32 rsdtptr;
    u32 len;
    u64 xsdt;
};

void ACPI::init()
{
    KernelInstance->console.printf("ACPI Startup\n");
    rsdp = NULL;
    while ((u64)rsdp < 0x100000)
    {
        if (*(u64 *)rsdp == 0x2052545020445352)
            break;
        rsdp = &((u64 *)rsdp)[1];
    }
    if ((u64)rsdp >= 0x100000)
        return;
    KernelInstance->console.printf(" RSDP Found at %p\n",rsdp);
    struct _acpi_rsdp *rp = (struct _acpi_rsdp *)rsdp;
    rsdt = dwtoptr(rp->rsdtptr);
    char oem[7];
    memset(oem,0,sizeof(oem));
    memcpy(oem,rp->oemid,sizeof(rp->oemid));
    KernelInstance->console.printf("  Manufacturer: %s\n",oem);
    KernelInstance->console.printf("  RSDT at %p\n",rsdt);
    if (rp->len > 20)
    {
        xsdt = (void *)rp->xsdt;
        KernelInstance->console.printf("  XSDT at %p\n",xsdt);
    } else {
        xsdt = NULL;
        KernelInstance->console.printf("  XSDT not specified\n");
    }

   // We allow for 64 tables to be defined, most ACPIs will be much much less than this
    memset(tables,0,sizeof(tables));
    if (xsdt)
    {
        u32 cnt = *(u32 *)(rp->xsdt + 4);
        cnt -= 36;
        u64 * ptr = (u64 *)(rp->xsdt + 36);
        memcpy(tables,ptr,cnt);
    } else {
        u32 *ptr = (u32 *)dwtoptr(rp->rsdtptr + 36);
        u32 cnt = *(u32 *)dwtoptr(rp->rsdtptr + 4);
        cnt -= 36;
        cnt /= 4;
        for (u32 i = 0; i < cnt; i++)
        {
           // KernelInstance->console.printf("Adding table address: %p (%d/%d)\n",ptr[i],i,cnt);
            tables[i] = dwtoptr(ptr[i]);
        }
    }
    for (u8 i = 0; i < 64 && tables[i]; i++)
    {
        u32 id = *(u32 *)tables[i];
        switch(id)
        {
           case 0x43495041:
               KernelInstance->console.printf("   Parsing processor info at %p\n",tables[i]);
               apic = new APIC();
               KernelInstance->console.printf("     APIC Object at %p\n",apic);
               apic->parse((void *)tables[i]);
            break;
            default: {}
              // KernelInstance->console.printf("Table type: %x not handled\n",id);
        }
    }
}

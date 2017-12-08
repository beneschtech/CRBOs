#include <CRBOs.h>

static const char *_failmsg = "I'm sorry but this operating system is designed to work on modern hardware and less than 32MB is a joke."
                              "I cant work under these conditions! *dramatic*";
MMU::MMU(void *p)
{
    map = (struct MMU_e820_map *)p;
}

void MMU::init()
{
    KernelInstance->console.printf(" Memory Management Startup\n");
    KernelInstance->console.printf("  E820 Map: (%d entries)\n",map->n_entries);
    myMaxMem = 0;
    for (u16 i = 0; i < map->n_entries; i++)
    {
        const char *type = "Usable";
        if (map->entry[i].type != 1)
        {
            type = "Reserved";
        } else {
            if (myMaxMem < (map->entry[i].base + map->entry[i].length))
                myMaxMem = (map->entry[i].base + map->entry[i].length);
        }
        KernelInstance->console.printf("   Segment: %p - %p (%s)\n",map->entry[i].base,map->entry[i].base + map->entry[i].length,type);
    }
    KernelInstance->console.printf("  Maximum usable memory: %m\n",myMaxMem);
    KernelInstance->console.printf("  Loading kernel page tables\n");
    if (myMaxMem <= 32*MB) {
        KernelInstance->console.printf(_failmsg);
        asm volatile ("cli\nhlt");
    }
    loadKernelPageTables();

    // Use up the directories actually used, ends up being #GB + 2
    memset(pageDir,0,sizeof(pageDir));
    int gbs = myMaxMem / GB;
    u64 *p = pageDir;
    while (gbs > 60)
    {
        *p = 0xffffffffffffffff;
        p++;
        gbs-=64;
    }
    *p = (1 << (gbs + 3))-1;

    u64 mappedMem = ((myMaxMem >> 21) + 1) << 21;
    for (u16 i = 0; i < map->n_entries; i++)
    {
        if (map->entry[i].base > mappedMem)
            mapIdentityPages(map->entry[i].base,map->entry[i].length,0x1B);
    }

    // And finally set CR3 to the kernel pagedir
    loadCR3(0x800000);

    // Now that thats set, lets set up our page bitmap
    pgBmp = (u64 *)0x1000000;
    pgBmpSz = myMaxMem / 262144;
    memset(pgBmp,0,pgBmpSz * sizeof(u64));

    //Set the first 16 MB as reserved.
    for (int i =0; i < 64; i++)
        pgBmp[i] = 0xffffffffffffffff;
}

void MMU::mapIdentityPages(u64 start, u64 len, u8 attribs)
{
    u64 pml4 = 1;
    pml4 <<= 40;
    if (start >= pml4)
    {
        KernelInstance->console.printf("Address too high to map, wait for V2! %p\n",start);
        return;
    }
    u64 *gbidx = (u64 *)0x801000;
    u64 *p2dir;
    if (!gbidx[start >> 30])
    {
        p2dir = (u64 *)findNextFreeDir();
        gbidx[start >> 30] = ((u64)p2dir) + attribs;
    } else {
        p2dir = (u64 *)(gbidx[start >> 30] & 0xfffffffffffff000);
    }
    u64 st2m = (start >> 21) & 511; // Offset into the 2MB page directory

    u64 stpg = start & 0xfffffffffffff000;
    u64 stidx = (start >> 12) & 511; // Offset into the regualr page table
    while (len)
    {
       u64 *p3dir = (u64 *)findNextFreeDir();
       p2dir[st2m] = (u64)p3dir + attribs;
       while (len && stidx < 512)
       {
           p3dir[stidx] = stpg + attribs;
           stidx++;
           stpg += 0x1000;
           if (len > 0x1000)
           {
               len -= 0x1000;
           } else {
               len = 0;
           }
       }
       if (len)
       {
           st2m++;
           stidx = 0;
       }
    }
}

u64 MMU::findNextFreeDir()
{
    u64 poff = 0;
    u64 *p = pageDir;
    while (*p == 0xffffffffffffffff)
    {
        p++;
        poff += 64;
    }
    u64 pbits = *p;
    u64 bs = 1;
    while (pbits & 1)
    {
        pbits >>= 1;
        bs <<= 1;
        poff++;
    }
    *p |= bs;
    return (0x800000 + (poff * 0x1000));
}

void MMU::loadKernelPageTables()
{
    // We use 2MB pages for the kernel usable memory.
    // Sorry only supporting up to 512 GB, that should be good for a hobby OS
    i64 memToAlloc = myMaxMem;
    u64 memBase = 0;
    u64 *cr3 = (u64 *)0x800000;
    *cr3 = 0x801003;
    cr3 += 513;

    cr3[-1] = 0x802003;
    u64 pgdir = 0x803003;
    while (memToAlloc >= GB)
    {
        *cr3++ = pgdir;
        memToAlloc -= GB;
        pgdir += 0x1000;
    }

    cr3 = (u64 *)0x802000;
    memToAlloc = myMaxMem;
    memBase = 0;
    while (memToAlloc > 0)
    {
        *cr3++ = memBase + 0x83;
        memToAlloc -= 2*MB;
        memBase += 2*MB;
    }
    *(u64 *)0x802000 = 0x9B; // Set first 2 MB as write through
}

void MMU::loadCR3(u64 pgRoot)
{
    asm volatile ("movq %0,%%cr3"::"r"(pgRoot));
}

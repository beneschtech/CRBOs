#ifndef MMU_H
#define MMU_H

#include <corelib/types.h>

// Mostly for display purposes
#define KB 1024
#define MB 1024*1024
#define GB 1024*1024*1024

// For page sizes
#define PGLVL1 4096
#define PGLVL2 2048*1024
#define PGLVL3 GB

class Kernel;
struct MMU_e820_entry
{
    u64 base;
    u64 length;
    u32 type;
    u32 attribs;
} __attribute((packed));

struct MMU_e820_map
{
    u16 n_entries;
    struct MMU_e820_entry entry[];
} __attribute((packed));

class MMU
{
friend class Kernel;
public:
    MMU(void *map = (void *)0x7010);
    u64 maxMem() { return myMaxMem; }
    void mapIdentityPages(u64 start,u64 len,u8 attribs=3);

private:
    void init();
    struct MMU_e820_map *map;
    u64 myMaxMem;
    void loadKernelPageTables();
    u64 pgBmpSz;
    u64 *pgBmp;
    u64 pageDir[32]; // bitmap of available page directories from 8mb - 16mb
    //u64 findNextFreePage(u8 lvl=0);
    u64 findNextFreeDir();
    void loadCR3(u64);
};

#endif // MMU_H

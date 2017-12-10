#include <corelib/io.h>

u8 inb(u16 port)
{
    u8 ret;
    asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
    return ret;
}

void outb(u16 port, u8 val)
{
    asm volatile ("outb %%al,%%dx"::"a" (val),"d" (port));
}

u16 inw(u16 port)
{
    u16 ret;
    asm volatile ("inw %%dx,%%ax":"=a" (ret):"d" (port));
    return ret;
}

void outw(u16 port, u16 val)
{
    asm volatile ("outw %%ax,%%dx"::"a" (val),"d" (port));
}

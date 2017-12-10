#ifndef IO_H
#define IO_H

#include <corelib/types.h>

void outb(u16,u8);
u8 inb(u16);
void outw(u16,u16);
u16 inw(u16);

#endif // IO_H

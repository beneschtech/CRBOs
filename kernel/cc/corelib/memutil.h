#ifndef MEMUTIL_H
#define MEMUTIL_H

#include <corelib/types.h>

void memset(void *,u8,u64);
void *kmalloc(unsigned long) __attribute__((noinline));
void kfree(void *) __attribute__((noinline));
void *dwtoptr(u32);
void memcpy(void *dst,void *src,u64 len);
void *operator new(unsigned long sz);
void operator delete(void *p);

#endif // MEMUTIL_H

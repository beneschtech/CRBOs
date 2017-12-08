volatile unsigned long _mallocBase;
#define KHEAP_BASE _mallocBase
#define KHEAP_END  0x7fffff // Up to 8MB, we get another 8 for other stuff as needed, ie page tables, etc..
#define KMALLOCMAGIC 0xdeadbeef1550900d // Dead beef is so good
struct kmallocdef {
  unsigned long magic;
  void *mem;
  unsigned long size;
  struct kmallocdef *next;
};

void *kmalloc(unsigned long len)
{
   if (len % 16 != 0) // align on 16 byte boundary
   {
      len >>= 4;
      len++;
      len <<= 4;
   }
   struct kmallocdef *ptr = (struct kmallocdef *)KHEAP_BASE;
   while (ptr->magic == KMALLOCMAGIC)
   {
      if (((unsigned long)ptr + sizeof(kmallocdef) + len) > KHEAP_END) // are we off the end of the heap?
         return (void *)0;
      unsigned long gap = (unsigned long)ptr->next - ((unsigned long)ptr + sizeof(kmallocdef) + ptr->size);
      if (gap > len + sizeof(kmallocdef)) // In case something inside was freed and can fit inside
      {
         struct kmallocdef *np = (struct kmallocdef *)((unsigned long)ptr->mem + ptr->size);
         np->mem = (void *)((unsigned long)np + sizeof(kmallocdef));
         np->next = ptr->next;
         ptr->next = np;
         np->size = len;
         np->magic = KMALLOCMAGIC;
         return np->mem;
      }
      ptr = ptr->next;
   }
   // Off the end of the list
   ptr->mem = (void *)((unsigned long)ptr + sizeof(kmallocdef));
   ptr->next = (struct kmallocdef *)((unsigned long)ptr + sizeof(kmallocdef) + len);
   ptr->size = len;
   ptr->magic = KMALLOCMAGIC;
   return ptr->mem;
}

void kfree(void *mem)
{
   struct kmallocdef *ptr = (struct kmallocdef *)KHEAP_BASE;
   while (ptr->magic == KMALLOCMAGIC)
   {
      if (ptr->mem == mem)
      {
         ptr->size = 0;
         return;
      }
      if (ptr->next->mem == mem) { // remove an entry from the middle
         ptr->next = ptr->next->next;
         return;
      }
      ptr = ptr->next;
   }
}

// Need these for C++ new and delete
void *operator new(unsigned long sz)
{
    return kmalloc(sz);
}

void operator delete(void *p)
{
    kfree(p);
}

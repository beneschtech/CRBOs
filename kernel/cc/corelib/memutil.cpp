#include <corelib/memutil.h>

void memset(void *addr, u8 val, u64 size)
{
    char *p = (char *)addr;
    for (u64 i = 0; i < size; i++)
        p[i] = val;
}

void *dwtoptr(u32 p)
{
    u64 rv = p;
    return (void *)rv;
}

void memcpy(void *dst, void *src, u64 len)
{
    u8 *dm = (u8 *)dst;
    u8 *sm = (u8 *)src;
    for (u64 i = 0; i < len; i++)
        dm[i] = sm[i];
}

u64 strlen(char *s)
{
    u64 rv = 0;
    while (*s++)
    {
        rv++;
    }
    return rv;
}

bool strequal(char *s1, char *s2)
{
    while (*s1++ == *s2++ && *s1 && *s2);
    return (*s1 == *s2);
}

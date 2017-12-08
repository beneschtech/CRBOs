#include <stdarg.h>
#include "VTConsole.h"

static const char *_startup_banner =
" #####   ######   ######   #######   #####   \n"
"#     #  #     #  #     #  #     #  #     #  \n"
"#        #     #  #     #  #     #  #        \n"
"#        ######   ######   #     #   #####   \n"
"#        #   #    #     #  #     #        #  \n"
"#     #  #    #   #     #  #     #  #     #  \n"
" #####   #     #  ######   #######   #####   \n"
"\n";

void VTConsole::initMsg()
{
    memset((void *)0xb8000,0,4000);
    putstring((char *)_startup_banner);
}

VTConsole::VTConsole(u16 *base)
{
    myBaseAddr = base;
    myCol = 0;
    myRow = 0;
    myMaxCols = 80;
    myMaxRows = 25;
    myAttribute = 0x0f;
}

void VTConsole::putchar(char c)
{
    u16 offst = (myRow * myMaxCols) + myCol;
    switch (c)
    {
    case '\r':
        myCol = 0;
        break;

    case '\n':
        myRow++;
        myCol = 0;
        break;

    case '\b':
        myBaseAddr[offst] = (myAttribute << 8) + 0x20;
        if (!myCol)
        {
            myCol = myMaxCols-1;
            if (!myRow)
            {
                myCol = myRow = 0;
            } else {
                myRow--;
            }
        } else {
            myCol--;
        }
        break;

    case '\t':
        myCol <<= 3;
        myCol++;
        myCol >>= 3;
        if (myCol >= myMaxCols)
        {
            myRow++;
            myCol -= myMaxCols;
        }
        break;

    default:
        myBaseAddr[offst] = (myAttribute << 8) + c;
        myCol++;
        if (myCol >= myMaxCols)
        {
            myCol = 0;
            myRow++;
        }
    }

    if (myRow == myMaxRows)
    {
        for (int i = 0; i < ((myMaxRows - 1) * myMaxCols); i++)
        {
            myBaseAddr[i] = myBaseAddr[i + myMaxCols];
        }
        for (int i = ((myMaxRows - 1) * myMaxCols); i < (myMaxCols * myMaxRows); i++)
            myBaseAddr[i] = (myAttribute << 8) + 0x20;
        myRow--;
    }
    offst = (myRow * myMaxCols) + myCol;
    outb(0x3d4,0x0f);
    outb(0x3d5,offst & 0xff);
    outb(0x3d4,0x0e);
    outb(0x3d5,offst >> 8);

}

void VTConsole::putstring(char *s)
{
    if (!s)
    {
        putstring((char *)"(NULL)");
        return;
    }
    while (*s)
    {
        putchar(*s++);
    }
}

static const char *_nums = "0123456789ABCDEF";

void VTConsole::putptr(void *ptr)
{
    char obuf[18];
    memset(obuf,0,sizeof(obuf));
    char *p = &obuf[16];
    u64 pv = (u64)ptr;
    while (p >= obuf)
    {
        *p-- = _nums[pv & 15];
        pv >>= 4;
    }
    p++;
    putstring((char *)"0x");
    putstring(p);
}

void VTConsole::putnum(long long v, int base)
{
    char obuf[65];
    if (base > 16)
        base = 16;
    memset(obuf,0,sizeof(obuf));
    char *p = &obuf[63];
    bool isNeg = (v < 0);
    if (isNeg)
        v = 0-v;

    do
    {
        *p-- = _nums[v % base];
        v/=base;
    } while (p >= obuf && v);
    if (isNeg) {
        *p = '-';
    } else {
        p++;
    }
    putstring(p);
}

void VTConsole::printf(const char *fmt,...)
{
    va_list l;
    va_start(l,fmt);
    while (*fmt)
    {
        if (*fmt != '%')
        {
            putchar(*fmt++);
        } else {
            fmt++;
            switch(*fmt)
            {
            case 's':
            {
                char *str = va_arg(l,char *);
                putstring(str);
                break;
            }
            case 'd':
            {
                long long v = va_arg(l,long long);
                putnum(v,10);
                break;
            }
            case 'p':
            {
                void *ptr = va_arg(l,void *);
                putptr(ptr);
                break;
            }
            case 'x':
            {
                long long v = va_arg(l,long long);
                putnum(v,16);
                break;
            }
            case 'm':
            {
                long long v = va_arg(l,long long);
                const char *un = " b";
                if (v > 2000)
                {
                    v /= 1024;
                    un = " kb";
                }
                if (v > 2000)
                {
                    v /= 1024;
                    un = " mb";
                }
                if (v > 2000)
                {
                    v /= 1024;
                    un = " gb";
                }
                putnum(v,10);
                putstring((char *)un);
            }
            default:
            {}
            }
            fmt++;
        }
    }
    va_end(l);
}

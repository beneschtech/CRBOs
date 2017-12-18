#include "Keyboard.h"

DrvKeyboard *DrvKeyboard::myInstance = NULL;

void _irqEntry1();
struct kbdscancodes *_index[256];
static u64 finalscode = 0;
static int expect = 0;

/* Make     Break       Normal  Caps    Shift   NumLock     Flags */
static struct kbdscancodes _dfltTblCodes[] = {
{ 0xe052,   0xe0d2, KBD_INSERT, KBD_INSERT, KBD_INSERT, KBD_INSERT, 0 },
{ 0xe047,   0xe0c7, KBD_HOME,   KBD_HOME,   KBD_HOME,   KBD_HOME,   0 },
{ 0xe049,   0xe0c9, KBD_PGUP,   KBD_PGUP,   KBD_PGUP,   KBD_PGUP,   0 },
{ 0xe051,   0xe0d1, KBD_PGDOWN, KBD_PGDOWN, KBD_PGDOWN, KBD_PGDOWN, 0 },
{ 0xe053,   0xe0d3, KBD_DEL,    KBD_DEL,    KBD_DEL,    KBD_DEL,    0 },
{ 0xe04f,   0xe0cf, KBD_END,    KBD_END,    KBD_END,    KBD_END,    0 },
{ 0xe048,   0xe0c8, KBD_UP,     KBD_UP,     KBD_UP,     KBD_UP,     0 },
{ 0xe04b,   0xe0cb, KBD_LEFT,   KBD_LEFT,   KBD_LEFT,   KBD_LEFT,   0 },
{ 0xe050,   0xe0d0, KBD_DOWN,   KBD_DOWN,   KBD_DOWN,   KBD_DOWN,   0 },
{ 0xe04d,   0xe0cd, KBD_RIGHT,  KBD_RIGHT,  KBD_RIGHT,  KBD_RIGHT,  0 },
{ 0x52,     0xd2,   KBD_INSERT,   KBD_INSERT,   KBD_INSERT,   '0', 0 },
{ 0x47,     0xc7,   KBD_HOME,   KBD_HOME,   KBD_HOME,   '7', 0 },
{ 0x49,     0xc9,   KBD_PGUP,   KBD_PGUP,   KBD_PGUP,   '9', 0 },
{ 0x51,     0xd1,   KBD_PGDOWN, KBD_PGDOWN, KBD_PGDOWN, '3', 0 },
{ 0x53,     0xd3,   KBD_DEL,    KBD_DEL,    KBD_DEL,    '.', 0 },
{ 0x4f,     0xcf,   KBD_END,    KBD_END,    KBD_END,    '1', 0 },
{ 0x48,     0xc8,   KBD_UP,     KBD_UP,     KBD_UP,     '8', 0 },
{ 0x4b,     0xcb,   KBD_LEFT,   KBD_LEFT,   KBD_LEFT,   '4', 0 },
{ 0x50,     0xd0,   KBD_DOWN,   KBD_DOWN,   KBD_DOWN,   '2', 0 },
{ 0x4d,     0xcd,   KBD_RIGHT,  KBD_RIGHT,  KBD_RIGHT,  '6', 0 },
{ 0x4c,     0xcc,       0,      0,       0,      '5',       0 },
{ 0x1e,     0x9e,       'a',    'A',     'A',    'a',       0 },
{ 0x30,     0xb0,       'b',    'B',     'B',    'b',       0 },
{ 0x2e,     0xae,       'c',    'C',     'C',    'c',       0 },
{ 0x20,     0xa0,       'd',    'D',     'D',    'd',       0 },
{ 0x12,     0x92,       'e',    'E',     'E',    'e',       0 },
{ 0x21,     0xa1,       'f',    'F',     'F',    'f',       0 },
{ 0x22,     0xa2,       'g',    'G',     'G',    'g',       0 },
{ 0x23,     0xa3,       'h',    'H',     'H',    'h',       0 },
{ 0x17,     0x97,       'i',    'I',     'I',    'i',       0 },
{ 0x24,     0xa4,       'j',    'J',     'J',    'j',       0 },
{ 0x25,     0xa5,       'k',    'K',     'K',    'k',       0 },
{ 0x26,     0xa6,       'l',    'L',     'L',    'l',       0 },
{ 0x32,     0xb2,       'm',    'M',     'M',    'm',       0 },
{ 0x31,     0xb1,       'n',    'N',     'N',    'n',       0 },
{ 0x18,     0x98,       'o',    'O',     'O',    'o',       0 },
{ 0x19,     0x99,       'p',    'P',     'P',    'p',       0 },
{ 0x10,     0x90,       'q',    'Q',     'Q',    'q',       0 },
{ 0x13,     0x93,       'r',    'R',     'R',    'r',       0 },
{ 0x1f,     0x9f,       's',    'S',     'S',    's',       0 },
{ 0x14,     0x94,       't',    'T',     'T',    't',       0 },
{ 0x16,     0x96,       'u',    'U',     'U',    'u',       0 },
{ 0x2f,     0xaf,       'v',    'V',     'V',    'v',       0 },
{ 0x11,     0x91,       'w',    'W',     'W',    'w',       0 },
{ 0x2d,     0xad,       'x',    'X',     'X',    'x',       0 },
{ 0x15,     0x95,       'y',    'Y',     'Y',    'y',       0 },
{ 0x2c,     0xac,       'z',    'Z',     'Z',    'z',       0 },
{ 0x0b,     0x8b,       '0',    '0',     ')',    '0',       0 },
{ 0x02,     0x82,       '1',    '1',     '!',    '1',       0 },
{ 0x03,     0x83,       '2',    '2',     '@',    '2',       0 },
{ 0x04,     0x84,       '3',    '3',     '#',    '3',       0 },
{ 0x05,     0x85,       '4',    '4',     '$',    '4',       0 },
{ 0x06,     0x86,       '5',    '5',     '%',    '5',       0 },
{ 0x07,     0x87,       '6',    '6',     '^',    '6',       0 },
{ 0x08,     0x88,       '7',    '7',     '&',    '7',       0 },
{ 0x09,     0x89,       '8',    '8',     '*',    '8',       0 },
{ 0x0a,     0x8a,       '9',    '9',     '(',    '9',       0 },
{ 0x29,     0xa9,       '`',    '`',     '~',    '`',       0 },
{ 0x0c,     0x8c,       '-',    '-',     '_',    '-',       0 },
{ 0x0d,     0x8d,       '=',    '=',     '+',    '=',       0 },
{ 0x2b,     0xab,       '\\',   '\\',    '|',    '\\',      0 },
{ 0x0e,     0x8e,       8,      8,        8,      8,        0 },
{ 0x39,     0xb9,       ' ',    ' ',     ' ',    ' ',       0 },
{ 0x0f,     0x8f,       '\t',   '\t',    '\t',   '\t',      0 },
{ 0x1c,     0x9c,       '\n',   '\n',    '\r',   '\n',      0 },
{ 0x01,     0x81,       27,     27,       27,     27,       0 },
{ 0x1a,     0x9a,       '[',    '[',      '{',    '[',      0 },
{ 0x1b,     0x9b,       ']',    ']',      '}',    ']',      0 },
{ 0x27,     0xa7,       ';',    ';',      ':',    ';',      0 },
{ 0x28,     0xa8,       '\'',   '\'',     '"',    '\'',     0 },
{ 0x35,     0xb5,       '/',    '/',      '?',    '/',      0 },
{ 0x33,     0xb3,       ',',    ',',      '<',    ',',      0 },
{ 0x34,     0xb4,       '.',    '.',      '>',    '.',      0 },
{ 0xe01c,   0xe09c,     '\n',   '\n',    '\r',   '\n',      0 },
{ 0xe035,   0xe0b5,     '/',    '/',      '/',    '/',      0 },
{ 0x37,     0xb7,       '*',    '*',      '*',    '*',      0 },
{ 0x4e,     0xce,       '+',    '+',      '+',    '+',      0 },
{ 0x4a,     0xca,       '-',    '-',      '-',    '-',      0 },
{ 0x3b,     0xbb,       KBD_F1, KBD_F1,   KBD_F1, KBD_F1,   0 },
{ 0x3c,     0xbc,       KBD_F2, KBD_F2,   KBD_F2, KBD_F2,   0 },
{ 0x3d,     0xbd,       KBD_F3, KBD_F3,   KBD_F3, KBD_F3,   0 },
{ 0x3e,     0xbe,       KBD_F4, KBD_F4,   KBD_F4, KBD_F4,   0 },
{ 0x3f,     0xbf,       KBD_F5, KBD_F5,   KBD_F5, KBD_F5,   0 },
{ 0x40,     0xc0,       KBD_F6, KBD_F6,   KBD_F6, KBD_F6,   0 },
{ 0x41,     0xc1,       KBD_F7, KBD_F7,   KBD_F7, KBD_F7,   0 },
{ 0x42,     0xc2,       KBD_F8, KBD_F8,   KBD_F8, KBD_F8,   0 },
{ 0x43,     0xc3,       KBD_F9, KBD_F9,   KBD_F9, KBD_F9,   0 },
{ 0x44,     0xc4,       KBD_F10, KBD_F10, KBD_F10, KBD_F10, 0 },
{ 0x57,     0xd7,       KBD_F11, KBD_F11, KBD_F11, KBD_F11, 0 },
{ 0x58,     0xd8,       KBD_F12, KBD_F12, KBD_F12, KBD_F12, 0 },
{ 0x3a,     0xba,       0,      0,        0,      0,        KBD_LED_CAPS },
{ 0x45,     0xc5,       0,      0,        0,      0,        KBD_LED_NUMLOCK },
{ 0x46,     0xc6,       0,      0,        0,      0,        KBD_LED_SCROLL },
{ 0x2a,     0xaa,         KBD_KEY_LSHIFT, KBD_KEY_LSHIFT, KBD_KEY_LSHIFT, KBD_KEY_LSHIFT, KBD_SHIFT },
{ 0x1d,     0x9d,         KBD_KEY_LCTRL,  KBD_KEY_LCTRL,  KBD_KEY_LCTRL,  KBD_KEY_LCTRL,  KBD_CTRL },
{ 0x38,     0xb8,         KBD_KEY_LALT,   KBD_KEY_LALT,   KBD_KEY_LALT,   KBD_KEY_LALT,   KBD_ALT },
{ 0xe05b,   0xe0db,       KBD_KEY_LWIN,   KBD_KEY_LWIN,   KBD_KEY_LWIN,   KBD_KEY_LWIN,   KBD_WIN },
{ 0x36,     0xb6,         KBD_KEY_RSHIFT, KBD_KEY_RSHIFT, KBD_KEY_RSHIFT, KBD_KEY_RSHIFT, KBD_SHIFT },
{ 0xe01d,   0xe09d,       KBD_KEY_RCTRL,  KBD_KEY_RCTRL,  KBD_KEY_RCTRL,  KBD_KEY_RCTRL,  KBD_CTRL },
{ 0xe038,   0xe0b8,       KBD_KEY_RALT,   KBD_KEY_RALT,   KBD_KEY_RALT,   KBD_KEY_RALT,   KBD_ALT },
{ 0xe05c,   0xe0dc,       KBD_KEY_RWIN,   KBD_KEY_RWIN,   KBD_KEY_RWIN,   KBD_KEY_RWIN,   KBD_WIN },
{ 0xe02ae037, 0xe0b7e0aa, KBD_PRTSCRN, KBD_PRTSCRN, KBD_PRTSCRN, KBD_PRTSCRN, 0 },
{ 0xe11d45e19dc5, 0,      KBD_PAUSE, KBD_PAUSE, KBD_PAUSE, KBD_PAUSE, 0 },
{ 0,0,0,0,0,0,0 }
};

DrvKeyboard::DrvKeyboard(Driver *p)
{
    setParent(p);
    myInstance = this;
    myState = 0;
    myTable = _dfltTblCodes;
    KernelInstance->interrupts.addDriverToIRQChain(1,(void *)&DRIVERCHAINFUNC(DrvKeyboard));
    KernelInstance->apic.setIRQVector(1,0x21,false);
    KernelInstance->interrupts.setISR(0x21,(u64)_irqEntry1);
    startupMessage(true);
}

void DrvKeyboard::handle()
{
    u8 scode = inb(0x60);

    if (scode > 0xe1) // No scancodes are bigger than this
        return;
    u8 ledstate = myState & 7;
    finalscode += scode;
    if (!expect && scode == 0xe0)
    {
        finalscode <<= 8;
        return;
    }
    if (finalscode == 0xe02a || finalscode == 0xe0b7)
    {
        finalscode <<= 8;
        expect = 1;
        return;
    }

    if (!expect && scode == 0xe1)
    {
        finalscode <<= 8;
        expect = 4;
        return;
    }
    if (expect)
    {
        finalscode <<= 8;
        expect--;
        return;
    }

    u8 outchar = 0;
    struct kbdscancodes *p = myTable;
    while (p->make)
    {
        if (p->make != finalscode && p->brk != finalscode)
        {
            p++;
            continue;
        }


        if (p->brk == finalscode)
        {
            if (p->flags & (KBD_SHIFT | KBD_CTRL | KBD_ALT | KBD_WIN))
                myState ^= p->flags;
            finalscode = expect = 0;
            return;
        }

        myState ^= p->flags;
        if ((p->caps == p->shift) && p->caps >= 'A' && p->caps <= 'Z')
        {
            u8 mask = KBD_SHIFT | KBD_LED_CAPS;
            if (myState ^ mask && (myState ^ mask) != mask)
            {
                outchar = p->caps;
            } else {
                outchar = p->norm;
            }
        } else {
            outchar = p->norm;
            if (myState & KBD_SHIFT && p->shift != p->norm)
                outchar = p->shift;
            if (myState & KBD_LED_NUMLOCK && p->num != p->norm)
                outchar = p->num;
        }
        break;
    }
    finalscode = expect = 0;
    if ((myState & 7) != ledstate)
        setLEDs(myState & 7);
    if (outchar)
        KernelInstance->console.printf("%c",outchar);
}

void DRIVERCHAINFUNC(DrvKeyboard)()
{
    DrvKeyboard::instance()->handle();
}

bool DrvKeyboard::sendKBDCommand(u8 cmd, u8 data)
{
    outb(0x60,cmd);
    while (inb(0x64) & 2);
    outb(0x60,data);
    while (inb(0x64) & 2);
    return true;
}

void DrvKeyboard::setLEDs(u8 ledstat)
{
    sendKBDCommand(0xed,ledstat);
}

bool DrvKeyboard::echo()
{
    outb(0x64,0xee);
    msleep(10);
    while (!(inb(0x64) & 1))
        msleep(10);
    return (inb(0x60) == 0xee);
}

#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <CRBOs.h>

// Flags
#define KBD_LED_SCROLL  1
#define KBD_LED_NUMLOCK 2
#define KBD_LED_CAPS    4
#define KBD_SHIFT       8
#define KBD_CTRL        16
#define KBD_ALT         32
#define KBD_WIN         64

// Special KBD codes
#define KBD_UP          0x80
#define KBD_DOWN        0x81
#define KBD_LEFT        0x82
#define KBD_RIGHT       0x83
#define KBD_PGUP        0x84
#define KBD_PGDOWN      0x85
#define KBD_HOME        0x86
#define KBD_END         0x87
#define KBD_PRTSCRN     0x88
#define KBD_KEY_LSHIFT  0x89
#define KBD_KEY_LCTRL   0x8a
#define KBD_KEY_RSHIFT  0x8b
#define KBD_KEY_RCTRL   0x8c
#define KBD_KEY_LALT    0x8d
#define KBD_KEY_RALT    0x8e
#define KBD_KEY_LWIN    0x8f
#define KBD_KEY_RWIN    0x90
#define KBD_PAUSE       0x91
#define KBD_F1          0x92
#define KBD_F2          0x93
#define KBD_F3          0x94
#define KBD_F4          0x95
#define KBD_F5          0x96
#define KBD_F6          0x97
#define KBD_F7          0x98
#define KBD_F8          0x99
#define KBD_F9          0x9a
#define KBD_F10         0x9b
#define KBD_F11         0x9c
#define KBD_F12         0x9d
#define KBD_INSERT      0x9e
#define KBD_DEL         0x7f

struct kbdscancodes {
    u64 make;
    u64 brk;
    u8 norm;
    u8 caps;
    u8 shift;
    u8 num;
    u8 flags;
};

class DrvKeyboard : public Driver
{
public:
    DrvKeyboard(Driver *p);
    ~DrvKeyboard() { }
    void handle();
    const char *type() { return "PS/2 Keyboard"; }
    static DrvKeyboard *instance() { return myInstance; }
    bool echo();

private:
    u8 myState;
    bool sendKBDCommand(u8 cmd,u8 data);
    void setLEDs(u8 ledstat);
    static DrvKeyboard *myInstance;
    struct kbdscancodes *myTable;
};

void DRIVERCHAINFUNC(DrvKeyboard)();

#endif // KEYBOARD_H

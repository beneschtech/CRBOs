#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <CRBOs.h>

#define LED_SCROLL  0
#define LED_NUMLOCK 1
#define LED_CAPS    2

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
    bool sendKBDCommand(u8 cmd,u8 data);
    void setLEDs(u8 ledstat);
    static DrvKeyboard *myInstance;
};

void DRIVERCHAINFUNC(DrvKeyboard)();

#endif // KEYBOARD_H

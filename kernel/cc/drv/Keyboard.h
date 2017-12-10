#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <CRBOs.h>

class DrvKeyboard : public Driver
{
public:
    DrvKeyboard(Driver *p);
    ~DrvKeyboard() { }
    void handle();
    const char *type() { return "PS/2 Keyboard"; }
    static DrvKeyboard *instance() { return myInstance; }

private:
    static DrvKeyboard *myInstance;
};

void DRIVERCHAINFUNC(DrvKeyboard)();

#endif // KEYBOARD_H

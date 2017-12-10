#ifndef DRIVER_H
#define DRIVER_H
/**
  * This is the driver root and base class implementation
  *
  * The general idea is that a driver can be either something that deals with hardware directly, or a parent group or controller class, ie Root->PS/2->keyboard
  * A controller, like for example the PS/2 controller can look for a keyboard and mouse under it and create driver objects accordingly, which will themselves
  * handle the interrupts
  *
  * Object oriented design in a kernel, the cornerstone of a microkernel
  */

// Dead Beef is not bad
#define DRIVERMAGIC 0xdeadbeef15407bad
#define DRIVERCHAINFUNC(t) IRQ##t
class Kernel;

// Base class, not directly used
class Driver
{
    friend class Kernel;
public:
    Driver(Driver *p = NULL):_magic(DRIVERMAGIC),parent(NULL),children(NULL),next(NULL),prev(NULL) { if (p) setParent(p); }
    virtual ~Driver() { }
    virtual void handle();
    virtual const char *type() { return "Driver"; }
    volatile u64 _magic;
    Driver *parent;
    Driver *children;
    Driver *next;
    Driver *prev;
    void setParent(Driver *);
    void startupMessage(bool bconf=false);
};

class DriverRoot : public Driver {
    friend class Kernel;
public:
    DriverRoot(): Driver::Driver() {}
    const char *type() { return "DriverRoot"; }
    void init();
};
#endif // DRIVER_H

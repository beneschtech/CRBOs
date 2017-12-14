#ifndef CRBOS_H
#define CRBOS_H

#include <corelib/io.h>

#include <corelib/ACPI.h>
#include <mem/MMU.h>
#include <corelib/InterruptManager.h>
#include <corelib/VTConsole.h>
#include <drv/driver.h>

// The Kernel class is really just a collection of all the other static kernel objects in one place
class Kernel
{
public:
   Kernel();
   ~Kernel(); // Theoretically should never reach here
   VTConsole console;
   MMU mem;
   ACPI acpi;
   APIC apic;
   InterruptManager interrupts;
   DriverRoot *rootDriver;

   void boot();
   void startKernelTask();

private:

};

extern Kernel *KernelInstance;

void printStackTrace(const char *msg = "",i64 offst=0) __attribute__((noinline));

#endif // CRBOS_H

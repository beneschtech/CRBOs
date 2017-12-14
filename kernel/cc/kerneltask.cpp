#include <CRBOs.h>

// All of the kernel ongoing management functions will be here as a seperate process, known in the unix world as process 0
// Its main purpose is to launch process 1 (rc) and cleanup behind the scenes, swapping cache in and out, etc...
void Kernel::startKernelTask()
{
    while (1) {  asm volatile ("hlt"); }
}

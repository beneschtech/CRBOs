#include <CRBOs.h>
#include "Keyboard.h"
#include "Timer.h"

// Sets the parent and next/prev pointers correctly so that we do have a doubly linked list tree format

void Driver::setParent(Driver *p)
{
    // No need to set it to null
    if (!p)
        return;

    // Best way to make sure its actually a Driver object without RTTI
    if (p->_magic != _magic)
        return;

    parent = p;
    // This makes it easy if this is the case
    if (!p->children)
    {
        p->children = this;
        return;
    }
    p = p->children;
    while (p->next)
        p = p->next;
    prev = p;
    p->next = this;
}

void Driver::startupMessage(bool bconf)
{
    Driver *p = this;
    while (p)
    {
        KernelInstance->console.printf(" ");
        p = p->parent;
    }
    KernelInstance->console.printf("%s",type());
    if (bconf) {
        u8 attrib = KernelInstance->console.attribute();
        KernelInstance->console.printf(": ");
        KernelInstance->console.setAttribute(2);
        KernelInstance->console.printf("%p\n",this);
        KernelInstance->console.setAttribute(attrib);
    } else {
        KernelInstance->console.printf("\n");
    }
}

void Driver::handle()
{
    // Do nothing.  Some drivers are just placeholders
}

// Root Driver implementation
void DriverRoot::init()
{
    KernelInstance->console.printf(" %s\n",type());
    new Timer(this);
    new DrvKeyboard(this);
}

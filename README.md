# CRBOs
My Hobby OS 64 bit, APIC, SMP, C++

This is just a hobby OS to tinker with on the side to keep my other skills sharp.  Its the end, it will implement vm86, user mode,
multiple processors and all modern technology and ideas.  I am also writing the meat of the kernel in C++ instead of the usual C.  It allows me (and others) to learn the internals of C++ and how things like new/delete, exceptions, and RTTI are implemented.

Required Software:

Linux / *BSD for creating ELF images.  Maybe someone wants to write a PE loader??
Bochs - obviously
QT Creator
Qt5 - Some of the utility programs are written with Qt5 as their main library
nasm
clang (optional)  Its what I am using, but I think gcc should work too

Subdirs:
floppy - Top level directory which in the end creates a bootable floppy image to test with
kernel - The kernel itself

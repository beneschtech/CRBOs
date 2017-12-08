TEMPLATE = aux

DISTFILES = \
   floppy.asm \
    bootsect.asm \
    loader.asm \
    loaderconstants.inc

floppybld.depends = $${DISTFILES}
floppybld.commands = nasm -f bin -o loader.bin loader.asm && nasm -f bin -o floppy floppy.asm

PRE_TARGETDEPS += floppybld
QMAKE_EXTRA_TARGETS += floppybld

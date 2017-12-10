TEMPLATE = app
CONFIG += console c++11 object_parallel_to_source
CONFIG -= app_bundle
CONFIG -= qt
CONFIG -= release
QMAKE_CXXFLAGS_APP -= -fPIC
QMAKE_CXXFLAGS += -fno-builtin -nostdinc++ -ffreestanding -fno-exceptions -fno-rtti -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -O3
QMAKE_LFLAGS = -dN -Ttext=0x102000 -e _Z6_startv -nostdlib
QMAKE_LINK = ld
INCLUDEPATH += $$_PRO_FILE_PWD_
OBJECTS_DIR=.
TARGET=../kernel

SOURCES += \
    entry.cpp \
    kernel.cpp \
    corelib/io.cpp \
    corelib/VTConsole.cpp \
    corelib/memutil.cpp \
    corelib/kalloc.cpp \
    mem/MMU.cpp \
    corelib/ACPI.cpp \
    corelib/InterruptManager.cpp \
    corelib/symtbl.cpp \
    corelib/vm86intr.cpp \
    corelib/apic.cpp \
    drv/driver.cpp \
    drv/Keyboard.cpp

HEADERS += \
    CRBOs.h \
    corelib/io.h \
    corelib/types.h \
    corelib/VTConsole.h \
    corelib/memutil.h \
    mem/MMU.h \
    corelib/ACPI.h \
    corelib/InterruptManager.h \
    corelib/krnlsyms.h \
    corelib/vm86intr.h \
    drv/driver.h \
    drv/Keyboard.h

DISTFILES += \
    symbols.awk

LIBS = -L../asmlib -lasm



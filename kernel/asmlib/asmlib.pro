TEMPLATE = lib
CONFIG += staticlib

TARGET=asm

ASMSOURCES = \
	InterruptManagerISR.asm

asm.commands=nasm -f elf64 -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
asm.output=$$OUT_PWD/${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.o
asm.input=ASMSOURCES
asm.variable_out=OBJECTS
asm.name=nasm ${QMAKE_FILE_IN}
QMAKE_EXTRA_COMPILERS += asm


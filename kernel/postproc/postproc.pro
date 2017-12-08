TEMPLATE=aux

_add_vm86.depends = ../vm86intr/vm86intr ../kernel
_add_vm86.commands = objcopy --add-section vm86intr=../vm86intr/vm86intr --change-section-address vm86intr=0x4000 --set-section-flags vm86intr=code,alloc,load ../kernel

PRE_TARGETDEPS += _add_vm86
QMAKE_EXTRA_TARGETS += _add_vm86

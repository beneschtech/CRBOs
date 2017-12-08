TEMPLATE = aux

DISTFILES =  \
	vm86intr.asm

vm86intr.depends = $${DISTFILES}
_vm86intr.commands = nasm -f bin -o vm86intr vm86intr.asm

PRE_TARGETDEPS += _vm86intr
QMAKE_EXTRA_TARGETS += _vm86intr

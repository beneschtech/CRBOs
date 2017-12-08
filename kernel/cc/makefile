first:
	-rm ../kernel
	make -f Makefile
	./symbols.awk > corelib/symtbl.cpp
	make -f Makefile
	strip ../kernel

clean:
	make -f Makefile clean

qmake_all:
	make -f Makefile qmake_all

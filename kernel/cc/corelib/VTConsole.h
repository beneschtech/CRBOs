#ifndef VTCONSOLE_H
#define VTCONSOLE_H

#include <corelib/types.h>
#include <corelib/io.h>
#include <corelib/memutil.h>

class Kernel;
class VTConsole
{
friend class Kernel;
public:
    VTConsole(u16 *base = (u16 *)0xb8000);
    void printf(const char *,...);

    u8 attribute() { return myAttribute; }
    void setAttribute(u8 v) { myAttribute = v; }
    u8 col() { return myCol; }
    void setCol(u8 v) { myCol = v; }
    u8 row() { return myRow; }
    void setRow(u8 v) { myRow = v; }
    u8 maxCols() { return myMaxCols; }
    void setMaxCols(u8 v) { myMaxCols = v; }
    u8 maxRows() { return myMaxRows; }
    void setMaxRows(u8 v) {myMaxRows = v; }

private:
    u16 *myBaseAddr;
    u8 myAttribute;
    u8 myCol;
    u8 myRow;
    u8 myMaxCols;
    u8 myMaxRows;

    void putchar(char);
    void putstring(char *);
    void putnum(long long,int);
    void putptr(void *);
    void initMsg();
};

#endif // VTCONSOLE_H

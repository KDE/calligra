#ifndef WINWORDDOC_H
#define WINWORDDOC_H

#include <qstring.h>
#include <myfile.h>
#include <fib.h>
#include <kdebug.h>


class WinWordDoc {

public:
    WinWordDoc(const myFile &mainStream, const myFile &table0Stream,
               const myFile &table1Stream, const myFile &dataStream);
    ~WinWordDoc();

private:
    void FIBInfo();
    void readFIB();
    inline const unsigned short read16(const unsigned char *d);
    inline const unsigned long read32(const unsigned char *d);

    bool success;
    FIB *fib;
    myFile main, table, data;
};
#endif // WINWORDDOC_H

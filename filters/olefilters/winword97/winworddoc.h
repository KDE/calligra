#ifndef WINWORDDOC_H
#define WINWORDDOC_H

#include <qstring.h>
#include <qstrlist.h>
#include <paragraph.h>
#include <stylesheet.h>
#include <myfile.h>
#include <misc.h>
#include <fib.h>
#include <pcd.h>
#include <kdebug.h>

class WinWordDoc {

public:
    WinWordDoc(const myFile &mainStream, const myFile &table0Stream,
               const myFile &table1Stream, const myFile &dataStream);
    ~WinWordDoc();

    const bool isOk() { return success; }

    const bool convert();
    const QString part();

private:
    WinWordDoc(const WinWordDoc &);
    const WinWordDoc &operator=(const WinWordDoc &);

    void FIBInfo();
    void readFIB();
    const PCD readPCD(const long &pos);
    const bool locatePieceTbl();
    const bool checkBinTables();

    bool success, ready;
    FIB *fib;
    myFile main, table, data;
    QString _part;
    QStrList mainParas;
    StyleSheet *styleSheet;

    // piece table (pt)
    long ptCPBase, ptSize, ptCount, ptPCDBase;
};
#endif // WINWORDDOC_H

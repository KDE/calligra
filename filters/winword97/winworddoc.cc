#include "winworddoc.h"

WinWordDoc::WinWordDoc(const myFile &mainStream, const myFile &table0Stream,
                       const myFile &table1Stream, const myFile &dataStream) :
                       main(mainStream), data(dataStream) {

    success=true;
    fib=0L;

    readFIB();

    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((int)fib->wIdent));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((long)fib->fcMin));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((long)fib->fcMac));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number(read32(main.data+24)));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number(read32(main.data+28)));

    if(fib->fWhichTblStm==0) {
        kdebug(KDEBUG_INFO, 31000, "Table 0");
        table=table0Stream;
    }
    else {
        kdebug(KDEBUG_INFO, 31000, "Table 1");
        table=table1Stream;
    }

    // more to come...
}

WinWordDoc::~WinWordDoc() {

    if(main.data) {
        delete [] main.data;
        main.data=0L;
    }
    if(table.data) {
        delete [] table.data;
        table.data=0L;
    }
    if(data.data) {
        delete [] data.data;
        data.data=0L;
    }
    if(fib) {
        char *tmp=(char*)fib;
        delete [] tmp;
        fib=0L;
    }
}

void WinWordDoc::readFIB() {

    char *tmp=new char[898];
    fib=(FIB*)tmp;
    unsigned short *tmpS;
    unsigned long *tmpL;
    int i;

    fib->wIdent=read16(main.data);
    fib->nFib=read16(main.data+2);
    fib->nProduct=read16(main.data+4);
    fib->lid=read16(main.data+6);
    fib->pnNext=read16(main.data+8);
    tmpS=(unsigned short*)fib;
    tmpS+=5;
    *tmpS=read16(main.data+10);
    fib->nFibBack=read16(main.data+12);

    fib->lKey=read32(main.data+14);

    fib->envr=*(main.data+18);
    *((unsigned char*)fib+19)=*(main.data+19);

    fib->chs=read16(main.data+20);
    fib->chsTables=read16(main.data+22);

    fib->fcMin=read32(main.data+24);
    fib->fcMac=read32(main.data+28);
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((long)fib->fcMac));

    fib->csw=read16(main.data+32);

    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((long)fib->fcMac));

    tmpS=(unsigned short*)fib->wMagicCreated;
    for(i=0; i<15; ++i, ++tmpS)
        *tmpS=read16(main.data+34+2*i);

    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((long)fib->fcMac));

    tmpL=(unsigned long*)fib->cbMac;
    for(i=0; i<22; ++i, ++tmpL)
        *tmpL=read32(main.data+64+4*i);

    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((long)fib->fcMac));

    fib->cfclcb=read16(main.data+152);

    tmpL=(unsigned long*)fib->fcStshfOrig;
    for(i=0; i<186; ++i, ++tmpL)
        *tmpL=read32(main.data+154+4*i);
}

inline const unsigned short WinWordDoc::read16(const unsigned char *d) {
    return ( (*(d+1) << 8) + *d );
}

inline const unsigned long WinWordDoc::read32(const unsigned char *d) {
    return ( (read16(d+2) << 16) + read16(d) );
}

#include <winworddoc.h>

WinWordDoc::WinWordDoc(const myFile &mainStream, const myFile &table0Stream,
                       const myFile &table1Stream, const myFile &dataStream) :
                       main(mainStream), data(dataStream) {

    success=true;
    fib=0L;
    readFIB();

    if(fib->fWhichTblStm==0)
        table=table0Stream;
    else
        table=table1Stream;

    // print some debug info
    FIBInfo();
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
        delete fib;
        fib=0L;
    }
}

void WinWordDoc::FIBInfo() {

    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((unsigned int)fib->wIdent));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->nFib));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->nProduct));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->lid));
    kdebug(KDEBUG_INFO, 31000, "some bits -----------------");
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fDot));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fGlsy));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fComplex));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fHasPic));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->cQuickSaves));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fEncrypted));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fWhichTblStm));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fReadOnlyRecommended));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fWriteReservation));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fExtChar));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fLoadOverride));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fFarEast));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fCrypto));
    kdebug(KDEBUG_INFO, 31000, "--------------------------");
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->nFibBack));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((long)fib->lKey));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->envr));
    kdebug(KDEBUG_INFO, 31000, "bits - bits - bits -------");
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fMac));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fEmptySpecial));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fLoadOverridePage));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fFutureSavedUndo));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fWord97Saved));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->fSpare0));
    kdebug(KDEBUG_INFO, 31000, "--------------------------");
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->chs));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((short)fib->chsTables));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((long)fib->fcMin));
    kdebug(KDEBUG_INFO, 31000, (const char*)QString::number((long)fib->fcMac));
    kdebug(KDEBUG_INFO, 31000, "T-e-x-t-------------------");

    char *str=new char[fib->fcMac - fib->fcMin];
    int i, j;

    for(i=fib->fcMin, j=0;i<fib->fcMac;++i, ++j)
        str[j]=*(main.data+i);

    kdebug(KDEBUG_INFO, 31000, (const char*)str);
    kdebug(KDEBUG_INFO, 31000, "--------------------------");
    delete [] str;
}

void WinWordDoc::readFIB() {

    fib=new FIB;
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

    fib->csw=read16(main.data+32);

    tmpS=(unsigned short*)&fib->wMagicCreated;
    for(i=0; i<15; ++i, ++tmpS)
        *tmpS=read16(main.data+34+2*i);

    tmpL=(unsigned long*)&fib->cbMac;
    for(i=0; i<22; ++i, ++tmpL)
        *tmpL=read32(main.data+64+4*i);

    fib->cfclcb=read16(main.data+152);

    tmpL=(unsigned long*)&fib->fcStshfOrig;
    for(i=0; i<186; ++i, ++tmpL)
        *tmpL=read32(main.data+154+4*i);
}

inline const unsigned short WinWordDoc::read16(const unsigned char *d) {
    return ( (*(d+1) << 8) + *d );
}

inline const unsigned long WinWordDoc::read32(const unsigned char *d) {
    return ( (read16(d+2) << 16) + read16(d) );
}

#include <winworddoc.h>

WinWordDoc::WinWordDoc(const myFile &mainStream, const myFile &table0Stream,
                       const myFile &table1Stream, const myFile &dataStream) :
                       main(mainStream), data(dataStream) {

    success=true;
    ready=false;
    fib=0L;
    styleSheet=0L;
    ptSize=-1;      // safer that way
    readFIB();

    if(fib->fEncrypted==1) {
        kdebug(KDEBUG_ERROR, 31000, "WinWordDoc::WinWordDoc(): Sorry - the document is encrypted.");
        success=false;
    }
    if(fib->fWhichTblStm==0)
        table=table0Stream;
    else
        table=table1Stream;

    styleSheet=new StyleSheet(table, fib);
    // print some debug info
    // FIBInfo();
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
    if(styleSheet) {
        delete styleSheet;
        styleSheet=0L;
    }
}

const bool WinWordDoc::convert() {

    if(!success || ready)
        return false;
    if(!locatePieceTbl())
        return false;
    if(!checkBinTables())
        return false;

    if(fib->fComplex==0) {
    }
    else {
    }

    ready=true;
    success=false;  // only now :)
    return true;
}

const QString WinWordDoc::part() {
    if(ready && success)
        return _part;
    else
        return QString("");
}

void WinWordDoc::FIBInfo() {

    kdebug(KDEBUG_INFO, 31000, "WinWordDoc::FIBInfo() - start -----------------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->wIdent))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->nFib))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->nProduct))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->lid))));
    kdebug(KDEBUG_INFO, 31000, "some bits -----------------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fDot))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fGlsy))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fComplex))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fHasPic))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->cQuickSaves))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fEncrypted))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fWhichTblStm))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fReadOnlyRecommended))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fWriteReservation))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fExtChar))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fLoadOverride))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fFarEast))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fCrypto))));
    kdebug(KDEBUG_INFO, 31000, "--------------------------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->nFibBack))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->lKey))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->envr))));
    kdebug(KDEBUG_INFO, 31000, "bits - bits - bits -------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fMac))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fEmptySpecial))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fLoadOverridePage))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fFutureSavedUndo))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fWord97Saved))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fSpare0))));
    kdebug(KDEBUG_INFO, 31000, "--------------------------");
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->chs))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->chsTables))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fcMin))));
    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(QString::number(static_cast<long>(fib->fcMac))));
    kdebug(KDEBUG_INFO, 31000, "T-e-x-t-------------------");

    char *str=new char[fib->fcMac - fib->fcMin];
    int i, j;

    for(i=fib->fcMin, j=0;i<fib->fcMac;++i, ++j)
        str[j]=*(main.data+i);

    kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(str));
    delete [] str;
    kdebug(KDEBUG_INFO, 31000, "WinWordDoc::FIBInfo() - end -----------------");
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

const PCD WinWordDoc::readPCD(const long &pos) {

    PCD ret;

    if(ptSize==-1)
        return ret;       // undefined, so don't call this one
                          // before you call locatePieceTbl()!

    long tmpPos=ptPCDBase+pos*8;
    unsigned short *tmp=(unsigned short*)&ret;

    *tmp=read16(table.data+tmpPos);
    ret.fc=read32(table.data+tmpPos+2);
    if((ret.fc & 0x40000000) == 0x40000000) {
        ret.fc=(ret.fc & 0xBFFFFFFF)/2;
        ret.unicode=false;
    }
    else
        ret.unicode=true;
    ret.prm=read16(table.data+tmpPos+6);
    return ret;
}

const bool WinWordDoc::locatePieceTbl() {

    long tmp=fib->fcClx;
    QString r;
    bool found=false;

    while(*(table.data+tmp)==1 && tmp<static_cast<long>(fib->fcClx+fib->lcbClx))
        tmp+=read16(table.data+tmp+1)+3;

    if(*(table.data+tmp)==2) {
        kdebug(KDEBUG_INFO, 31000, "WinWordDoc::locatePieceTbl(): Hoohoo! Found pclfpcd :)");
        found=true;
        ptCPBase=tmp+1;
        ptSize=read32(table.data+ptCPBase);
        ptCPBase+=4;
        if((ptSize-4)%12!=0) {
            kdebug(KDEBUG_ERROR, 31000, "WinWordDoc::locatePieceTbl(): Sumting Wong (inside joke(tm))");
            found=false;
        }
        ptCount=static_cast<long>((ptSize-4)/12);
        ptPCDBase=ptCount*4+4+ptCPBase;
    }
    else {
        success=false;
        kdebug(KDEBUG_ERROR, 31000, "WinWordDoc::locatePieceTbl(): Can't locate the piece table");
    }
    return found;
}

const bool WinWordDoc::checkBinTables() {

    bool notCompressed=false;
    if(fib->pnFbpChpFirst==0xfffff && fib->pnFbpPapFirst==0xfffff &&
       fib->pnFbpLvcFirst==0xfffff)
        notCompressed=true;
    else {
        kdebug(KDEBUG_INFO, 31000, "WinWordDoc::checkBinTables(): Sigh! It's compressed...");
        success=false;
    }
    return notCompressed;
}

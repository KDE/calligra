#include <stylesheet.h>

StyleSheet::StyleSheet(const myFile &t, const FIB * const f) : table(t),
                       fib(f), lastSti(0xffff) {

    stdOffset=fib->fcStshf+read16(table.data+fib->fcStshf);
    cstd=read16(table.data+fib->fcStshf+2);
    stdBaseLength=read16(table.data+fib->fcStshf+4);

    // for testing purposes
    // kdebug(KDEBUG_INFO, 31000, "################## StyleSheet::chain_rec() ####################");
    // kdebug(KDEBUG_INFO, 31000, styleName(65));
    // kdebug(KDEBUG_INFO, 31000, styleName(0));
    // kdebug(KDEBUG_INFO, 31000, "################## StyleSheet::chain_rec() ####################");
}

StyleSheet::~StyleSheet() {
    _chain.clear();
}

const QValueList<STYLE> StyleSheet::chain(const unsigned short &sti) {

    if(lastSti==sti)
        return _chain;

    _chain.clear();
    if(chain_rec(sti))
        lastSti=sti;
    else {
        _chain.clear();
        lastSti=0xffff;
    }
    return _chain;
}

const bool StyleSheet::chain_rec(const unsigned short &sti) {

    if(sti==0 || sti==65)     // end of the recursion
        return true;

    unsigned long offset;
    unsigned short tmp;
    bool success=true;
    QString d;

    if(findSTD(sti, offset)) {
        d="StyleSheet::chain_rec(): found sti(";
        d+=QString::number((long)sti);
        d+="), offset=";
        d+=QString::number((long)offset);
        kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(d));

        tmp=read16(table.data+offset+2);
        unsigned short sgc=tmp & 0x000f;
        unsigned short istdBase=tmp & 0xfff0;
        tmp=read16(table.data+offset+4);
        unsigned short cupx=tmp & 0x000f;
        unsigned short istdNext=tmp & 0xfff0;

        d="sgc=";
        d+=QString::number((long)sgc);
        kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(d));
        d="isdtBase=";
        d+=QString::number((long)istdBase);
        kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(d));
        d="cupx=";
        d+=QString::number((long)cupx);
        kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(d));
        d="istdNext=";
        d+=QString::number((long)istdNext);
        kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(d));

        success=chain_rec(istdBase);   // next step towards NULL style
        // "read" STD, fill STYLE, and add it to the chain! (TODO)
    }
    else {
        d="StyleSheet::chain_rec(): ERROR: Didn't find sti(";
        d+=QString::number((long)sti);
        d+=")!";
        kdebug(KDEBUG_INFO, 31000, static_cast<const char*>(d));
        success=false;
    }
    return success;
}

const QString StyleSheet::styleName(const unsigned short &sti) {

    unsigned long offset;

    if(findSTD(sti, offset))
        return QString("");    // get the string (TODO)
    else
        return QString("");
}

void StyleSheet::align2(long &adr) {
    if( (long)(table.data+adr+1)/2 != (long)(table.data+adr)/2 )
        ++adr;
}

const bool StyleSheet::findSTD(const unsigned short &sti, unsigned long &offset) {

    unsigned long i=stdOffset, limit=fib->fcStshf+fib->lcbStshf;
    unsigned short j=0, len, tmp;

    do {
        len=read16(table.data+i);                   // length of this STD
        i+=2;                                       // set offset to beginning of STD
        if(len!=0) {
            tmp=read16(table.data+i) & 0x0fff;      // read the sti (lower 12 bits)
            if(tmp==sti) {
                offset=i;
                return true;                        // we've found it!
            }
            i+=len;                                 // not the right one -> next STD
        }
        ++j;                                        // two conditions for safety :)
    } while(i<limit && j<cstd);
    return false;                                   // not found!
}

inline const unsigned short StyleSheet::read16(const unsigned char *d) {
    return ( (*(d+1) << 8) + *d );
}

inline const unsigned long StyleSheet::read32(const unsigned char *d) {
    return ( (read16(d+2) << 16) + read16(d) );
}

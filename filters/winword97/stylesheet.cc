#include <stylesheet.h>

StyleSheet::StyleSheet(const myFile &t, const FIB * const f) : table(t),
                       fib(f), lastSti(0xffff) {

    stdOffset=fib->fcStshf+read16(table.data+fib->fcStshf);
    cstd=read16(table.data+fib->fcStshf+2);
    stdBaseLength=read16(table.data+fib->fcStshf+4);
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

    unsigned long offset;
    unsigned short tmp;
    bool success=true;

    if(findSTD(sti, offset)) {
        tmp=read16(table.data+offset+2);
        unsigned short sgc=tmp & 0x000f;
        unsigned short istdBase=(tmp & 0xfff0) >> 4;
        tmp=read16(table.data+offset+4);
        unsigned short cupx=tmp & 0x000f;

        if(istdBase!=0x0fff)
            success=chain_rec(istdBase);   // next step towards stiNil (end of recursion)

        // leave out the name (you can get it via StyleSheet::styleName(...)!)
        offset+=stdBaseLength;
        align2(offset);
        offset+=(read16(table.data+offset)<<1)+4;

        STYLE myStyle;
        if(sgc==1)
            myStyle.paragStyle=true;
        else if(sgc==2)
            myStyle.paragStyle=false;
        else {
            kdebug(KDEBUG_ERROR, 31000, "StyleSheet::chain_rec(): Error: Don't know this style-format!");
            return false;
        }

        if(sgc==1 && cupx==2) {                            // only for paragraph styles
            tmp=read16(table.data+offset);                 // read the length of the UPX
            offset+=2;                                     // Adjust offset (2 bytes length info)
            if(read16(table.data+offset)!=sti)
                kdebug(KDEBUG_WARN, 31000, "StyleSheet::chain_rec(): Sigh - istd and sti are different!?!");
            myStyle.fcPAPX=offset+2;
            myStyle.lenPAPX=tmp;
            offset+=tmp;
            tmp=read16(table.data+offset);
            offset+=2;
            myStyle.fcCHPX=offset;
            myStyle.lenCHPX=tmp;
        }
        else if(sgc==2 && cupx==1) {                       // for character styles
            tmp=read16(table.data+offset);
            offset+=2;
            myStyle.fcCHPX=offset;
            myStyle.lenCHPX=tmp;
            myStyle.fcPAPX=0;
            myStyle.lenPAPX=0;
        }
        else {
            kdebug(KDEBUG_ERROR, 31000, "StyleSheet::chain_rec(): Error: Don't know this UPX-format!");
            return false;
        }
        _chain.append(myStyle);
    }
    else {
        QString d="StyleSheet::chain_rec(): ERROR: Didn't find sti(";
        d+=QString::number((long)sti);
        d+=")!";
        kdebug(KDEBUG_ERROR, 31000, static_cast<const char*>(d));
        success=false;
    }
    return success;
}

const QString StyleSheet::styleName(const unsigned short &sti) {

    unsigned long offset;

    if(findSTD(sti, offset)) {
        QString ret;
        offset+=stdBaseLength;
        align2(offset);
        unsigned short len=read16(table.data+offset);
        offset+=2;
        len<<=1;
        for(unsigned int i=0; i<len; i=i+2)
            ret+=QChar(char2uni(read16(table.data+offset+i)));
        return ret;
    }
    else
        return QString("");
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
    } while(i<limit && j<=cstd);
    return false;                                   // not found!
}

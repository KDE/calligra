#include <stylesheet.h>

StyleSheet::StyleSheet(const myFile &t, const FIB * const f) : table(t),
                       fib(f), lastSti(0xffff) {

    stdOffset=fib->fcStshf+read16(table.data+fib->fcStshf);
    cstd=read16(table.data+fib->fcStshf+2);
    stdBaseLength=read16(table.data+fib->fcStshf+4);

    unsigned long offset=stdOffset, limit=fib->fcStshf+fib->lcbStshf, tmpOffset;
    unsigned short istd=0, len, sti, tmp;
    STD mySTD;

    QString d;

    do {
        kdebug(KDEBUG_INFO, 31000, "###################################");
        d="offset=";
        d+=QString::number((long)offset);
        kdebug(KDEBUG_INFO, 31000, (const char*)d);
        len=read16(table.data+offset);              // length of this STD
        d="len=";
        d+=QString::number((long)len);
        kdebug(KDEBUG_INFO, 31000, (const char*)d);
        offset+=2;                                  // set offset to beginning of STD
        if(len!=0) {
            sti=read16(table.data+offset) & 0x0fff; // read the sti (lower 12 bits)
            d="sti=";
            d+=QString::number((long)sti);
            kdebug(KDEBUG_INFO, 31000, (const char*)d);
            tmp=read16(table.data+offset+2);
            unsigned short sgc=tmp & 0x000f;        // read the sgc
            d="sgc=";
            d+=QString::number((long)sgc);
            kdebug(KDEBUG_INFO, 31000, (const char*)d);
            mySTD.istdBase=(tmp & 0xfff0) >> 4;     // and the istdBase
            d="istdBase=";
            d+=QString::number((long)mySTD.istdBase);
            kdebug(KDEBUG_INFO, 31000, (const char*)d);
            tmp=read16(table.data+offset+4);
            unsigned short cupx=tmp & 0x000f;       // we also need the cupx
            d="cupx=";
            d+=QString::number((long)cupx);
            kdebug(KDEBUG_INFO, 31000, (const char*)d);

            tmpOffset=offset+stdBaseLength;         // use it as temporary offset :)
            d="tmpOffset=";
            d+=QString::number((long)tmpOffset);
            kdebug(KDEBUG_INFO, 31000, (const char*)d);
            align2(tmpOffset);
            d="aligned tmpOffset=";
            d+=QString::number((long)tmpOffset);
            kdebug(KDEBUG_INFO, 31000, (const char*)d);
            tmp=read16(table.data+tmpOffset);       // the length
            d="len (in chars)=";
            d+=QString::number((long)tmp);
            kdebug(KDEBUG_INFO, 31000, (const char*)d);
            tmpOffset+=2;                           // length field is a short
            tmp<<=1;                                // we use 2-byte chars!
            mySTD.name="";                          // reset name
            for(unsigned int i=0; i<tmp; i=i+2)
                mySTD.name+=QChar(char2uni(read16(table.data+tmpOffset+i)));

            d="name=";
            d+=mySTD.name;
            kdebug(KDEBUG_INFO, 31000, (const char*)d);

            tmpOffset+=tmp+2;

            d="new tmpOffset=";
            d+=QString::number((long)tmpOffset);
            kdebug(KDEBUG_INFO, 31000, (const char*)d);

            if(sgc==1 && cupx==2) {
                kdebug(KDEBUG_INFO, 31000, "PARAG");
                mySTD.style.paragStyle=true;
                tmp=read16(table.data+tmpOffset);   // read the length of the UPX
                d="len=";
                d+=QString::number((long)tmp);
                kdebug(KDEBUG_INFO, 31000, (const char*)d);
                tmpOffset+=2;                       // Adjust offset (2 bytes length info)
                if(read16(table.data+tmpOffset)+1!=istd)
                    kdebug(KDEBUG_WARN, 31000, "StyleSheet::chain_rec(): istd is not correct!?!");
                mySTD.style.fcPAPX=tmpOffset+2;
                d="fcPAPX=";
                d+=QString::number((long)mySTD.style.fcPAPX);
                kdebug(KDEBUG_INFO, 31000, (const char*)d);
                mySTD.style.lenPAPX=tmp;
                d="lenPAPX=";
                d+=QString::number((long)mySTD.style.lenPAPX);
                kdebug(KDEBUG_INFO, 31000, (const char*)d);
                tmpOffset+=tmp;
                tmp=read16(table.data+tmpOffset);
                tmpOffset+=2;
                mySTD.style.fcCHPX=tmpOffset;
                d="fcCHPX=";
                d+=QString::number((long)mySTD.style.fcCHPX);
                kdebug(KDEBUG_INFO, 31000, (const char*)d);
                mySTD.style.lenCHPX=tmp;
                d="lenCHPX=";
                d+=QString::number((long)mySTD.style.lenCHPX);
                kdebug(KDEBUG_INFO, 31000, (const char*)d);
            }
            else if(sgc==2 && cupx==1) {
                kdebug(KDEBUG_INFO, 31000, "CHAR");
                mySTD.style.paragStyle=false;
                tmp=read16(table.data+tmpOffset);
                tmpOffset+=2;
                mySTD.style.fcCHPX=tmpOffset;
                d="fcCHPX=";
                d+=QString::number((long)mySTD.style.fcCHPX);
                kdebug(KDEBUG_INFO, 31000, (const char*)d);
                mySTD.style.lenCHPX=tmp;
                d="lenCHPX=";
                d+=QString::number((long)mySTD.style.lenCHPX);
                kdebug(KDEBUG_INFO, 31000, (const char*)d);
                mySTD.style.fcPAPX=0;
                mySTD.style.lenPAPX=0;
            }
            else
                kdebug(KDEBUG_ERROR, 31000, "StyleSheet::chain_rec(): Error: Don't know this stylesheet-format!");
            offset+=len;                            // next STD
        }
        else {                                      // empty slot!
            kdebug(KDEBUG_INFO, 31000, "EMPTY SLOT");
            sti=0xffff;
            mySTD.istdBase=0xffff;
            mySTD.name="";
            mySTD.style.paragStyle=false;
            mySTD.style.fcPAPX=0xffffffff;
            mySTD.style.lenPAPX=0xffff;
            mySTD.style.fcCHPX=0xffffffff;
            mySTD.style.lenCHPX=0xffff;
        }
        styleMap.insert(sti, mySTD);
        ++istd;
    } while(offset<limit && istd<=cstd);
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

const bool StyleSheet::chain_rec(const unsigned short &/*sti*/) {
/*
    unsigned long offset;
    unsigned short tmp;
    bool success=true;

    if(findSTD(sti, offset)) {


        if(istdBase!=0x0fff)
            success=chain_rec(istdBase);   // next step towards stiNil (end of recursion)


        STYLE myStyle;


        if(sgc==1) {                            // only for paragraph styles

        }
        else if(sgc==2) {                       // for character styles

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
*/ return true;
}

const QString StyleSheet::styleName(const unsigned short &/*sti*/) {

 /*   unsigned long offset;

    if(findSTD(sti, offset)) {

    }
    else */
        return QString("");
}

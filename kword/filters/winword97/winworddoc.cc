#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "winworddoc.h"

WinWordDoc::WinWordDoc(myFile in) {
    
    doc=in;

    // fib=new FIB;

    // memcpy(fib, doc.data, sizeof(struct FIB));

    fib=(FIB*)doc.data;  // nasty cast ;)

    QString foo;
    foo.setNum(fib->wIdent);
    kdebug(KDEBUG_INFO, 31000, (const char*)foo);

    kdebug(KDEBUG_INFO, 31000, "Text");
    foo.truncate(0);
    
    for(long i=fib->fcMin; i<fib->fcMac;++i)
        foo+=QChar(doc.data[i]);
    
    kdebug(KDEBUG_INFO, 31000, (const char*)foo);

    kdebug(KDEBUG_INFO, 31000, "fcMin/fcMac");
    
    foo.setNum(fib->fcMin);
    kdebug(KDEBUG_INFO, 31000, (const char*)foo);
    foo.setNum(fib->fcMac);
    kdebug(KDEBUG_INFO, 31000, (const char*)foo);
}

WinWordDoc::~WinWordDoc() {
    // if(fib) {
    //     delete fib;
    //     fib=0L;
    // }
}

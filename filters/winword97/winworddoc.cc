#include "winworddoc.h"
#include "winworddoc.moc"

WinWordDoc::WinWordDoc(myFile in) : QObject() {

    success=true;
    mainStream=0L;
    doc=in;

    KLaola myLaola(in);

    if(!getMainStream(&myLaola)) {
        kdebug(KDEBUG_ERROR, 31000, "Could not read main stream.");
        success=false;
        emit signalFilterError();
    }
    else {
        fib=(FIB*)mainStream;
        // more to come...
    }
}

WinWordDoc::~WinWordDoc() {
    if(mainStream) {
        delete mainStream;
        mainStream=0L;
    }
}

void WinWordDoc::slotFilterError() {
    success=false;
    emit signalFilterError();
}

bool WinWordDoc::getMainStream(KLaola *) {
    return true;
}

#include "winworddoc.h"
#include "winworddoc.moc"

WinWordDoc::WinWordDoc(const myFile &mainStream, const myFile &table0Stream,
                       const myFile &table1Stream, const myFile &dataStream) :
                       QObject() {

    success=true;
    main=mainStream;
    table0=table0Stream;
    table1=table1Stream;
    data=dataStream;

    // more to come...
}

WinWordDoc::~WinWordDoc() {

    kdebug(KDEBUG_INFO, 31000, "WinWordDoc - DTOR - Anfang");

    if(main.data) {
        delete [] main.data;
        main.data=0L;
    }
    if(table0.data) {
        delete [] table0.data;
        table0.data=0L;
    }
    if(table1.data) {
        delete [] table1.data;
        table1.data=0L;
    }
    if(data.data) {
        delete [] data.data;
        data.data=0L;
    }

    kdebug(KDEBUG_INFO, 31000, "WinWordDoc - DTOR - Ende");
}

void WinWordDoc::slotFilterError() {
    success=false;
    emit signalFilterError();
}

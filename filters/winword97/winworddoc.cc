#include "winworddoc.h"
#include "winworddoc.moc"

WinWordDoc::WinWordDoc(const myFile &, const myFile &, const myFile &,
                       const myFile &) : QObject() {

    success=true;
    main.data=0L;
    table0.data=0L;
    table1.data=0L;
    data.data=0L;

    // more to come...
}

WinWordDoc::~WinWordDoc() {
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
}

void WinWordDoc::slotFilterError() {
    success=false;
    emit signalFilterError();
}

#include "winworddoc.h"
#include "winworddoc.moc"

WinWordDoc::WinWordDoc(const myFile &, const myFile &, const myFile &) : QObject() {

    success=true;
    main=0L;
    table=0L;

    // more to come...
}

WinWordDoc::~WinWordDoc() {
    if(main) {
        delete main;
        main=0L;
    }
    if(table) {
        delete table;
        table=0L;
    }
}

void WinWordDoc::slotFilterError() {
    success=false;
    emit signalFilterError();
}

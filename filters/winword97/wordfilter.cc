#include "wordfilter.h"
#include "wordfilter.moc"

WordFilter::WordFilter(const myFile &mainStream, const myFile &tableStream,
                       const myFile &dataStream) : FilterBase() {

    success=false; // normally true; only at the moment...

    myDoc=0L;
    myDoc=new WinWordDoc(mainStream, tableStream, dataStream);
    connect(myDoc, SIGNAL(signalFilterError()), this, SLOT(slotFilterError()));

    myKwd=0L;
    myKwd=new KWordDoc();
    connect(myKwd, SIGNAL(signalFilterError()), this, SLOT(slotFilterError()));
}

WordFilter::~WordFilter() {
    if(myDoc) {
        delete myDoc;
        myDoc=0L;
    }
    if(myKwd) {
        delete myKwd;
        myKwd=0L;
    }
}

void WordFilter::slotFilterError() {
    success=false;
}

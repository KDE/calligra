#include "wordfilter.h"
#include "wordfilter.moc"

WordFilter::WordFilter(const QString &mainStream, const QString &tableStream) : QObject() {

    success=false; //true; only at the moment...

    myDoc=0L;
    myDoc=new WinWordDoc(mainStream, tableStream);
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

bool WordFilter::filter() {
    // not very interesting at the moment
    return success;
}

QString WordFilter::kwdFile() {
    return myKwd->kwdFile();
}

void WordFilter::slotFilterError() {
    success=false;
}

#include "wordfilter.h"
#include "wordfilter.moc"

WordFilter::WordFilter(myFile in) : QObject() {

    success=false; //true; only at the moment...

    myDoc=new WinWordDoc(in);
    connect(myDoc, SIGNAL(signalFilterError()), this, SLOT(slotFilterError()));

    myKwd=new KWordDoc();
    connect(myKwd, SIGNAL(signalFilterError()), this, SLOT(slotFilterError()));
}

WordFilter::~WordFilter() {
    delete myDoc;
    myDoc=0L;
    delete myKwd;
    myKwd=0L;
}

bool WordFilter::filterIt() {
    // not very interesting at the moment
    return success;
}

QString &WordFilter::kwdFile() {
    return myKwd->kwdFile();
}

void WordFilter::slotFilterError() {
    success=false;
}

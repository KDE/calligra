#include "wordfilter.h"
#include "wordfilter.moc"

WordFilter::WordFilter(const myFile &mainStream, const myFile &table0Stream,
                       const myFile &table1Stream, const myFile &dataStream) :
                       FilterBase() {

    myDoc=0L;
    myDoc=new WinWordDoc(mainStream, table0Stream, table1Stream, dataStream);
    myKwd=0L;
    myKwd=new KWordDoc();
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

const QString WordFilter::part() {

    if(ready && success) {
        // return real file from KWordDoc...
    }
    else
        return FilterBase::part();
    return FilterBase::part();  // just to keep the compiler quiet
}

#include <wordfilter.h>
#include <wordfilter.moc>

WordFilter::WordFilter(const myFile &mainStream, const myFile &table0Stream,
                       const myFile &table1Stream, const myFile &dataStream) :
                       FilterBase() {

    myDoc=0L;
    myDoc=new WinWordDoc(mainStream, table0Stream, table1Stream, dataStream);
    myKwd=0L;
    myKwd=new KWordDoc();
}

WordFilter::~WordFilter() {

    delete myDoc;
    myDoc=0L;
    delete myKwd;
    myKwd=0L;
}

const QString WordFilter::part() {

    if(ready && success)
        return myKwd->kwdFile();
    else
        return FilterBase::part();
}

const bool WordFilter::filter() {

    success=myDoc->convert();
    myKwd->part(myDoc->part());
    success=myDoc->isOk() & myKwd->isOk();
    ready=true;
    return success;
}

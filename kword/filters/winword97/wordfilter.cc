#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "wordfilter.h"

WordFilter::WordFilter(myFile in) {
    doc=in;
    myDoc=new WinWordDoc(doc);
    myKwd=new KWordDoc();
}

WordFilter::~WordFilter() {
    delete myDoc;
    myDoc=0L;
    delete myKwd;
    myKwd=0L;
}

bool WordFilter::filterIt() {
    // not very interesting at the moment ;)
    return true;
}

QString WordFilter::kwdFile() {
    return myKwd->kwdFile();
}

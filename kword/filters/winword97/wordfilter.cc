#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "wordfilter.h"

WordFilter::WordFilter(myFile in) {
    myDoc=new WinWordDoc(in);
    myKwd=new KWordDoc();
}

WordFilter::~WordFilter() {
    delete myDoc;
    myDoc=0L;
    delete myKwd;
    myKwd=0L;
}

bool WordFilter::filterIt() {
    // not very interesting at the moment
    return false;  // was not able to convert ;)
}

QString WordFilter::kwdFile() {
    return myKwd->kwdFile();
}

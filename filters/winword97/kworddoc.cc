#include <kworddoc.h>

KWordDoc::KWordDoc() {
    success=true;
    ready=false;
}

KWordDoc::~KWordDoc() {
}

void KWordDoc::part(const QString &part) {

    ready=true;
    success=false; //at the moment
}

const QString KWordDoc::kwdFile() {

    if(ready && success)
        return kwd;
    else
        return QString("");
}

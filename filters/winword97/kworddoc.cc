#include "kworddoc.h"

KWordDoc::KWordDoc() {
    success=true;
}

KWordDoc::~KWordDoc() {
}

const QString KWordDoc::kwdFile() {
    return kwd;
}

#include "kworddoc.h"
#include "kworddoc.moc"

KWordDoc::KWordDoc() : QObject() {
    success=true;
}

KWordDoc::~KWordDoc() {
}

const QString KWordDoc::kwdFile() {
    return kwd;
}

void KWordDoc::slotFilterError() {
    success=false;
    emit signalFilterError();
}

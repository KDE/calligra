#include "kworddoc.h"
#include "kworddoc.moc"

KWordDoc::KWordDoc() : QObject() {
    success=true;
}

KWordDoc::~KWordDoc() {
    kdebug(KDEBUG_INFO, 31000, "KWordDoc - DTOR");
}

const QString KWordDoc::kwdFile() {
    return kwd;
}

void KWordDoc::slotFilterError() {
    success=false;
    emit signalFilterError();
}

#include <filterbase.h>
#include <filterbase.moc>


FilterBase::FilterBase() : QObject() {
    success=true;
    ready=false;
}

const bool FilterBase::filter() {
    success=false;
    ready=true;
    return success;
}

void FilterBase::slotSavePic(Picture *pic) {
    emit signalSavePic(pic);
}

void FilterBase::slotPart(const char *nameIN, char **nameOUT) {
    emit signalPart(nameIN, nameOUT);
}

void FilterBase::slotFilterError() {
    success=false;
}

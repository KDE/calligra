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

void FilterBase::slotSavePic(const char *data, const char *type, const unsigned long size,
                             char **nameOUT) {
    emit signalSavePic(data, type, size, nameOUT);
}

void FilterBase::slotFilterError() {
    success=false;
}

#include "olefilter.h"
#include "olefilter.moc"

OLEFilter::OLEFilter(const myFile &in, const QString &nameOut, const IN i,
                     const OUT o) : QObject(), fileIn(in),
                     fileOutName(nameOut), in(i), out(o) {

    docfile=0L;
    fileOut=0L;
    success=false; // later -> true;

    docfile=new KLaola(fileIn);
    if(!docfile->isOk())
        success=false;
    else {
        fileOut=new KTar(fileOutName);
        fileOut->open(IO_WriteOnly);
    }
}

OLEFilter::~OLEFilter() {

    fileOut->close();
    delete fileOut;
    fileOut=0L;

    if(docfile) {
        delete docfile;
        docfile=0L;
    }
}

const bool OLEFilter::filter() {
    return success;
}

void OLEFilter::slotSavePic(const char *, const char *) {
}

void OLEFilter::slotPart(const char *, const char *) {
}

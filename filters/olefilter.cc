#include "olefilter.h"
#include "olefilter.moc"

OLEFilter::OLEFilter(const myFile &in, const Document d) : QObject(),
                     file(in), document(d) {

    docfile=new KLaola(file);
    parseFile();
}

OLEFilter::~OLEFilter() {

    if(docfile) {
        delete docfile;
        docfile=0L;
    }
}

bool OLEFilter::filter() {
    return false;
}

bool OLEFilter::store() {
    return false;
}

void OLEFilter::parseFile() {
}

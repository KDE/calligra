#include "olefilter.h"
#include "olefilter.moc"

OLEFilter::OLEFilter(myFile in) : QObject() {
    wordFilter=new WordFilter(in);
}

OLEFilter::~OLEFilter() {
    delete wordFilter;
}

bool OLEFilter::filterIt() {
    return false;
}

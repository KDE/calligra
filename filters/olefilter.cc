#include "olefilter.h"
#include "olefilter.moc"

OLEFilter::OLEFilter(myFile) : QObject() {
}

OLEFilter::~OLEFilter() {
}

bool OLEFilter::filter() {
    return false;
}

bool OLEFilter::store() {
    return false;
}

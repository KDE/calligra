#include <stylesheet.h>
#include <stylesheet.moc>

StyleSheet::StyleSheet(const myFile &) : QObject() {
}

StyleSheet::~StyleSheet() {
}

void StyleSheet::align2(long &adr) {
    if( (long)(adr+1)/2 != (long)adr/2 )
        ++adr;
}

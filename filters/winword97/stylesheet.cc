#include <stylesheet.h>

StyleSheet::StyleSheet(const myFile &m) : main(m) {
}

StyleSheet::~StyleSheet() {
    _chain.clear();
}

const QValueList<long> StyleSheet::chain(const long &styleID) {
    _chain.clear();
    chain_rec(styleID);
    return _chain;
}

void StyleSheet::chain_rec(const long &) { //styleID) {
    // go one level deeper (towards NULL-Style)
    // add own value to list
}

void StyleSheet::align2(long &adr) {
    if( (long)(adr+1)/2 != (long)adr/2 )
        ++adr;
}

#include <stylesheet.h>

StyleSheet::StyleSheet(const myFile &m) : main(m), lastSti(0xffff) {
}

StyleSheet::~StyleSheet() {
    _chain.clear();
}

const QValueList<STYLE> StyleSheet::chain(const unsigned short &sti) {

    if(lastSti==sti)
        return _chain;

    _chain.clear();
    if(chain_rec(sti))
        lastSti=sti;
    else
        _chain.clear();

    return _chain;
}

bool StyleSheet::chain_rec(const unsigned short &) { //sti) {
    // go one level deeper (towards NULL-Style)
    // calculate STYLE and add it to the list
    return true;  // everything alright?
}

void StyleSheet::align2(long &adr) {
    if( (long)(adr+1)/2 != (long)adr/2 )
        ++adr;
}

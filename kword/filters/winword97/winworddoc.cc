#include "winworddoc.h"

WinWordDoc::WinWordDoc(myFile in) {
    doc=in;
    KLaola myLaola(in);  // for testing purpose
}

WinWordDoc::~WinWordDoc() {
}

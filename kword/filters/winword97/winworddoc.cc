#include "winworddoc.h"

WinWordDoc::WinWordDoc(myFile in) {
    doc=in;
    KLaola myLaola(in);  // for testing purpose
    myLaola.testIt();    // some output
}

WinWordDoc::~WinWordDoc() {
}

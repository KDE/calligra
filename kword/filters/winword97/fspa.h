// FSPA

#ifndef FSPA_H
#define FSPA_H

struct FSPA {
    long spid;
    long xaLeft;
    long yaTop;
    long xaRight;
    long yaBottom;
    unsigned short fHdr:1;
    unsigned short bx:2;
    unsigned short by:2;
    unsigned short wr:4;
    unsigned short wrk:4;
    unsigned short fRcaSimple:1;
    unsigned short fBelowText:1;
    unsigned short fAnchorLock:1;
    long cTxbx;
};
#endif //FSPA _H

// PGD, Page Descriptor

#ifndef PGD_H
#define PGD_H

struct PGD {
    short fContinue:1;
    short fUnk:1;
    short fRight:1;
    short fPgnRestart:1;
    short fEmptyPage:1;
    short fAllFtn:1;
    short unused:1;
    short fTableBreaks:1;
    short fMarked:1;
    short fColumnBreaks:1;
    short fTableHeader:1;
    short fNewPage:1;
    short bkc:4;
    unsigned short lnn;
    unsigned short pgn;
    long dym;
};
#endif // PGD_H

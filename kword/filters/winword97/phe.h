// PHE, Paragraph Height

#ifndef PHE_H
#define PHE_H

struct PHE {
    short fSpare:1;
    short fUnk:1;
    short fDiffLines:1;
    short reserved:5;
    short clMac:8;
    short reserved2;
    long dxaCol;
    long dymLine;
    long dymHeight;
};

struct PHE2 {    // see page 114
    long fSpare:1;
    long fUnk:1;
    long dcpTtpNext:30;
    long dxaCol;
    long dymTableHeight;
};
#endif // PHE_H

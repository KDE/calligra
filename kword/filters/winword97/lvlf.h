// LVLF List LeVel (on File)

#ifndef LVLF_H
#define LVLF_H

struct LVLF {
    long iStartAt;
    char nfc;
    unsigned char jc:2;
    unsigned char fLegal:1;
    unsigned char fNoRestart:1;
    unsigned char fPrev:1;
    unsigned char fPrevSpace:1;
    unsigned char fWord6:1;
    short rgbxchNums[9];
    unsigned char ixchFollow;
    long dxaSpace;
    long daxIndent;
    char cbGrpprlChpx;
    char cbGrpprlPapx;
    short reserved;
};
#endif // LVLF_H

// ANLD, Autonumbered List Data Descriptor

#ifndef ANLD_H
#define ANLD_H

struct ANLD {
    unsigned char nfc;
    unsigned char cxchTextBefore;
    unsigned char cxchTextAfter;
    unsigned char jc:2;
    unsigned char fPrev:1;
    unsigned char fHang:1;
    unsigned char fSetBold:1;
    unsigned char fSetItalic:1;
    unsigned char fSetSmallCaps:1;
    unsigned char fSetCaps:1;
    unsigned char fSetStrike:1;
    unsigned char fSetKul:1;
    unsigned char fPrevSpace:1;
    unsigned char fBold:1;
    unsigned char fItalic:1;
    unsigned char fSmallCpas:1;
    unsigned char fCaps:1;
    unsigned char fStrike:1;
    unsigned char kul:3;
    unsigned char ico:5;
    short ftc;
    unsigned short hps;
    unsigned short iStartAt;
    unsigned short dxaIndent;
    unsigned short dxaSpace;
    unsigned char fNumber1;
    unsigned char fNumberAcross;
    unsigned char fRestartHdn;
    unsigned char fSpareX;
    short rgxch[32];
};
#endif // ANLD_H

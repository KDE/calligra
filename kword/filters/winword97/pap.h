// PAP, Paragraph Properties

#ifndef PAP_H
#define PAP_H

#include "lspd.h"
#include "phe.h"
#include "tap.h"
#include "brc.h"
#include "shd.h"
#include "dcs.h"
#include "anld.h"
#include "dttm.h"
#include "numrm.h"

struct PAP {
    unsigned short istd;
    unsigned char jc;
    unsigned char fKeep;
    unsigned char fKeepFollow;
    unsigned char fPageBreakBefore;
    char fBrLnAbove:1;
    char fBrLnBelow:1;
    char fUnused:2;
    char pcVert:2;
    char pcHorz:2;
    unsigned char brcp;
    unsigned char brcl;
    unsigned char reserved;
    unsigned char ilvl;
    unsigned char fNoLnn;
    short ilfo;
    unsigned char nLvlAnm;
    unsigned char reserved2;
    unsigned char fSideBySide;
    unsigned char reserved3;
    unsigned char fNoAutoHyph;
    unsigned char fWindowControl;
    long dxaRight;
    long dxaLeft;
    long dxaLeft1;
    LSPD lspd;
    unsigned long dyaBefore;
    unsigned long dyaAfter;
    PHE phe;
    unsigned char fCrLf;
    unsigned char fUsePgsuSettings;
    unsigned char fAdjustRight;
    unsigned char reserved4;
    unsigned char fKinsoku;
    unsigned char fWordWarp;
    unsigned char fOverflowPunct;
    unsigned char fTopLinePunct;
    unsigned char fAutoSpaceDE;
    unsigned char fAutoSpaceDN;
    unsigned short fAlignFont;
    short fVertical:1;
    short fBackward:1;
    short fRotateFont:1;
    short reserved5:13;
    short reserved6;
    char fInTable;
    char fTtp;
    char wr;
    char fLocked;
    TAP ptap;   // pointer???
    long dxaAbs;
    long dyaAbs;
    long dxaWidth;
    BRC brcTop;
    BRC brcLeft;
    BRC brcBottom;
    BRC brcRight;
    BRC brcBetween;
    BRC brcBar;
    long dxaFromText;
    long dyaFromText;
    short dyaHeigth:15;
    short fMinHeight:1;
    SHD shd;
    DCS dcs;
    char lvl;
    char fNumRMIns;
    ANLD anld;
    short fPropRMark;
    short ibstPropRMark;
    DTTM dttmPropRMark;
    NUMRM numrm;
    short itbdMac;
    short rgdxaTab[64];
    char rgtbd[64];
};
#endif // PAP_H

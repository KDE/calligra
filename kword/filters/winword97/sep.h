// SEP, Section Properties

#ifndef SEP_H
#define SEP_H

#include "brc.h"
#include "dttm.h"
#include "olst.h"

struct SEP {
    unsigned char bck;
    unsigned char fTitlePage;
    char fAutoPgn;
    unsigned char nfcPgn;
    unsigned char fUnlocked;
    unsigned char cnsPgn;
    unsigned char fPgnRestart;
    unsigned char fEndNote;
    char lnc;
    char grpfIhdt;
    unsigned short nLnnMod;
    long dxaLnn;
    short dxaPgn;
    short dyaPgn;
    char fLBetween;
    char vjc;
    unsigned short dmBinFirst;
    unsigned short dmBinOther;
    unsigned short dmPaperReq;
    BRC brcTop;
    BRC brcLeft;
    BRC brcBottom;
    BRC brcRight;
    short fPropRMark;
    short ibstPropRMark;
    DTTM dttmPropRMark;
    long dxtCharSpace;
    long dyaLinePitch;
    unsigned short clm;
    short reserved;
    unsigned char dmOrientPage;
    unsigned char iHeadingPgn;
    unsigned short pgnStart;
    short lnnMin;
    unsigned short wTextFlow;
    short reserved2;
    short pgbProp;
    short pgbApplyTo:3;
    short pgbPageDepth:2;
    short pgbOffsetFrom:3;
    short reserved3:8;
    unsigned long xaPage;
    unsigned long yaPage;
    unsigned long xaPageNUp;
    unsigned long yaPageNUp;
    unsigned long dxaLeft;
    unsigned long dxaRight;
    long dyaTop;
    long dyaBottom;
    unsigned long dzaGutter;
    unsigned long dyaHdrTop;
    short ccolMl;
    char fEvenlySpaced;
    char reserved4;
    long dxaColumns;
    long rgdxaColumnWidthSpacing[89];
    long dxaColumnWidth;
    unsigned char dmOrientFirst;
    unsigned char fLayout;
    short reserved5;
    OLST olstAnm;
};
#endif // SEP_H

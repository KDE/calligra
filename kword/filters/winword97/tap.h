// TAP, Table Properties

#ifndef TAP_H
#define TAP_H

#include "tlp.h"
#include "tc.h"
#include "shd.h"
#include "brc.h"

struct TAP {
    short jc;
    long dxaGapHalf;
    long dyaRowHeight;
    unsigned char fCantSplit;
    unsigned char fTableHeader;
    TLP tlp;
    long lwHTMLProps;
    short fCaFull:1;
    short fFirstRow:1;
    short fLastRow:1;
    short fOutline:1;
    short reserved:12;
    short itcMac;
    long dxaAdjust;
    long dxaScale;
    long dxsInch;
    short rgdxaCenter[65];
    short rgdxaCenterPrint[65];
    TC rgtc[64];
    SHD rgshd[64];
    BRC rgbrcTable[6];
};
#endif // TAP_H

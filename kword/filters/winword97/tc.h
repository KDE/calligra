// TC, Table Cell Descriptor

#ifndef TC_H
#define TC_H

#include "brc.h"

struct TC {
    short fFirstMerged:1;
    short fMerged:1;
    short fVertical:1;
    short fBackward:1;
    short fRotateFont;
    short fVertMerge:1;
    short fVertRestart:1;
    short vertAlign:2;
    short fUnused:7;
    unsigned short wUnused;
    BRC brcTop;
    BRC brcLeft;
    BRC brcBottom;
    BRC brcRight;
};
#endif // TC_H

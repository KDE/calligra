// NUMRM, Number Revision Mark Data

#ifndef NUMRM_H
#define NUMRM_H

#include "dttm.h"

struct NUMRM {
    unsigned char fNumRM;
    unsigned char Spare;
    short ibstNumRM;
    DTTM dttmNumRM;
    unsigned char rgbxchNums[9];
    unsigned char rgnfc[9];
    short Spare2;
    int PNBR[9];
    short xst[32];
};
#endif // NUMRM_H

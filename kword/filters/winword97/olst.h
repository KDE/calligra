// OLST, Outline List Data

#ifndef OLST_H
#define OLST_H

#include "anlv.h"

struct OLST {
    ANLV rganlv[9];
    unsigned char fRestartHdr;
    unsigned char fSpareOlst2;
    unsigned char fSpareOlst3;
    unsigned char fSpraeOlst4;
    short rgxch[32];
};
#endif // OLST_H

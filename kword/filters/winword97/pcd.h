// PCD, Piece Descriptor

#ifndef PCD_H
#define PCD_H

#include "prm.h"

struct PCD {
    short fNoParaLast:1;
    short fPaphNil:1;
    short fCopied:1;
    short reserved:5;
    short fn:8;
    long fc;
    PRM prm;
};
#endif // PCD_H

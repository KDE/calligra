// PAPX, Paragraph Property Exceptions

#ifndef PAPX_H
#define PAPX_H

struct PAPX {
    char cb;
    unsigned short istd;
    char *grpprl;
};

struct PAPX2 {   // see page 120
    short cw;
    unsigned short istd;
    char *grpprl;
};
#endif // PAPX_H

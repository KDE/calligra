// LFOLVL, List Format Override for a single LeVeL

#ifndef LFOLVL_H
#define LFOLVL_H

struct LFOLVL {
    long iStartAt;
    unsigned char ilvl:4;
    unsigned char fStartAt:1;
    unsigned char fFormatting:1;
    unsigned char reserved:2;
    char reserved2[3];
};
#endif // LFOLVL_H

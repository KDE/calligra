// FFN, Font Family Name

#ifndef FFN_H
#define FFN_H

struct FFN {
    unsigned char cbFfnM1;
    unsigned char prq:2;
    unsigned char fTrueType:1;
    unsigned char reserved:1;
    unsigned char ff:3;
    unsigned char reserved2:1;
    short wWeight;
    unsigned char chs;
    unsigned char ixchSzAlt;
    char panose[10];   // don't know ;(
    char fs[24];       // see above
    short *xszFfn;
};
#endif // FFN_H

// PCD, Piece Descriptor

#ifndef PCD_H
#define PCD_H

struct PCD {
    unsigned short fNoParaLast:1;
    unsigned short fPaphNil:1;
    unsigned short fCopied:1;
    unsigned short reserved:5;
    unsigned short fn:8;
    long fc;
    unsigned short prm;
    bool unicode;
};
#endif // PCD_H

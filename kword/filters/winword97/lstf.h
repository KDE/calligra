// LSTF, LiST Data (on File)

#ifndef LSTF_H
#define LSTF_H

struct LSTF {
    long lsid;
    long tplc;
    short rgistd[9];
    unsigned char fSimpleList:1;
    unsigned char fRestartHdn:1;
    unsigned char reserved:6;
    unsigned char reserved2;
};
#endif // LSTF_H

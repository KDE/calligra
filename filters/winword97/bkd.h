// BKD, BreaK Descriptor

#ifndef BKD_H
#define BKD_H

struct BKD {
    short ipgd;
    short itxbxs;
    short dcpDepend;
    unsigned short icol:8;
    unsigned short fTableBreak:1;
    unsigned short fColumnBreak:1;
    unsigned short fMarked:1;
    unsigned short fUnk:1;
    unsigned short fTextOverflow:1;
    unsigned short rest:3;
};
#endif // BKD_H

// BRC, Border Code

#ifndef BRC_H
#define BRC_H

struct BRC {
    short dptLineWidth:8;
    short brcType:8;
    short ico:8;
    short dptSpace:5;
    short fShadow:1;
    short fFrame:1;
    short unused:1;
};
#endif // BRC_H

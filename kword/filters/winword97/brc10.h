// BRC10, Border Code for Windows Word 1.0

#ifndef BRC10_H
#define BRC10_H

struct BRC10 {
    short dxpLine2Width:3;
    short dxpSpaceBetween:3;
    short dxpLine1Width:3;
    short dxpSpace:5;
    short fShadow:1;
    short fSpare:1;
};
#endif // BRC10_H

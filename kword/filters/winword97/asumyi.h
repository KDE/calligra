// ASUMYI, AutoSummary Info

#ifndef ASUMYI_H
#define ASUMYI_H

struct ASUMYI {
    short fValid:1;
    short fView:1;
    short fViewBy:2;
    short fUpdateProps:1;
    short reserved:11;
    short wDlgLevel;
    long lHighestLevel;
    long lCurrentLevel;
};
#endif // ASUMYI_H

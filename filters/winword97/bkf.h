// BKF, BooKmark First descriptor

#ifndef BKF_H
#define BKF_H

struct BKF {
    short ibkl;
    unsigned short itcFirst:7;
    unsigned short fPub:1;
    unsigned short itcLim:7;
    unsigned short fCol:1;
};
#endif // BKF_H

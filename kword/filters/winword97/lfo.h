// LFO, List Format Override

#ifndef LFO_H
#define LFO_H

struct LFO {
    long lsid;
    long reserved;
    long reserved2;
    unsigned char clfolvl;
    char reserved3[3];
};
#endif // LFO_H

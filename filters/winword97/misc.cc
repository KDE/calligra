#include <misc.h>

const unsigned short read16(const unsigned char *d) {
    return ( (*(d+1) << 8) + *d );
}

const unsigned long read32(const unsigned char *d) {
    return ( (read16(d+2) << 16) + read16(d) );
}

const short char2uni(const unsigned char &c) {
    if(c<=0x7f || c>=0xa0)
        return static_cast<short>(c);
    else
        return CP2UNI[c-0x80];
}

void align2(unsigned long &adr) {
    if((adr%2)!=0)
        ++adr;
}


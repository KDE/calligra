// PICF, Picture Descriptor (on File)

#ifndef PICF_H
#define PICF_H

#include "brc.h"

struct PICF {
    long lcb;
    unsigned short cbHeader;
    short mfp_mm;
    short mfp_xExt;
    short mfp_yExt;
    short mfp_hMF;
    char bm[14];    // char rcWinMF[14];???
    short dxaGoal;
    short dyaGoal;
    unsigned short mx;
    unsigned short my;
    short dxaCropLeft;
    short dyaCropTop;
    short dxaCropRight;
    short dyaCropBottom;
    short brcl:4;
    short fFrameEmpty:1;
    short fBitmap:1;
    short fDrawHatch:1;
    short fError:1;
    short bpp:8;
    BRC brcTop;
    BRC brcLeft;
    BRC brcBottom;
    BRC brcRight;
    short dxaOrigin;
    short dyaOrigin;
    short cProps;
    short rgb;
};
#endif // PICF_H

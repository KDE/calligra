// CHP, Character Properties

#ifndef CHP_H
#define CHP_H

#include "dttm.h"
#include "shd.h"
#include "brc.h"

struct CHP {
    short fBold:1;
    short fItalic:1;
    short fRMarkDel:1;
    short fOutline:1;
    short fFldVanish:1;
    short fSmallCaps:1;
    short fCaps:1;
    short fVanish:1;
    short fRMark:1;
    short fSpec:1;
    short fStrike:1;
    short fObj:1;
    short fShadow:1;
    short fLowerCase:1;
    short fData:1;
    short fOle2:1;
    short fEmboss:1;
    short fImprint:1;
    short fDStrike:1;
    short fUsePgsuSettings:1;
    short unused:12;
    long reserved;
    short ftc;
    short ftcAscii;
    short ftcFE;
    short fctOther;
    unsigned short hps;
    long dxaSpace;
    short iss:3;
    short kul:4;
    short fSpecSymbol:1;
    short ico:5;
    short reserved2:1;
    short fSysVanish:1;
    short hpsPos:1;
    short myhpsPos;
    unsigned short lid;
    unsigned short lidDefault;
    unsigned short lidFE;
    unsigned char idct;
    unsigned char idctHint;
    unsigned short wCharScale;
    //unsigned long fcPic;  // I don't know if there is an error in the descr.
    //unsigned long fcObj;  // here too
    unsigned long lTagObj;
    short ibstRMark;
    short ibstRMarkDel;
    DTTM dttmRMark;
    DTTM dttmRMarkDel;
    short reserved3;
    unsigned short istd;
    short ftcSym;
    short xchSym;
    short idslRMReason;
    short idslReasonDel;
    unsigned char ysr;
    unsigned char chYsr;
    unsigned short cpg;
    unsigned short hpsKern;
    short icoHighlight:5;
    short fHighlight:1;
    short kcd:3;
    short fNavHighlight:1;
    short fChsDiff:1;
    short fMacChs:1;
    short fFtcAsciSym:1;
    short reserved4:3;
    unsigned short fPropMark;
    short ibstPropRMark;
    DTTM ddtmPropRMark;
    unsigned char sfxtText;
    unsigned char reserved5;
    unsigned char reserved6;
    unsigned short reserved7;
    short reserved8;
    DTTM reserved9;
    unsigned char fDispFldRMark;
    short ibstDispFldRMark;
    DTTM dttmDispFldRMark;
    short xstDispFldRMark[16];
    SHD shd;
    BRC brc;
};
#endif // CHP_H

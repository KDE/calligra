/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@carinthia.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

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
    int reserved;
    short ftc;
    short ftcAscii;
    short ftcFE;
    short fctOther;
    unsigned short hps;
    int dxaSpace;
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
    //unsigned int fcPic;  // I don't know if there is an error in the descr.
    //unsigned int fcObj;  // here too
    unsigned int lTagObj;
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

    bool ok;
};
#endif // CHP_H

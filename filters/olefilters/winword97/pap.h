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

// PAP, Paragraph Properties

#ifndef PAP_H
#define PAP_H

#include "lspd.h"
#include "phe.h"
#include "tap.h"
#include "brc.h"
#include "shd.h"
#include "dcs.h"
#include "anld.h"
#include "dttm.h"
#include "numrm.h"

struct PAP {
    unsigned short istd;
    unsigned char jc;
    unsigned char fKeep;
    unsigned char fKeepFollow;
    unsigned char fPageBreakBefore;
    char fBrLnAbove:1;
    char fBrLnBelow:1;
    char fUnused:2;
    char pcVert:2;
    char pcHorz:2;
    unsigned char brcp;
    unsigned char brcl;
    unsigned char reserved;
    unsigned char ilvl;
    unsigned char fNoLnn;
    short ilfo;
    unsigned char nLvlAnm;
    unsigned char reserved2;
    unsigned char fSideBySide;
    unsigned char reserved3;
    unsigned char fNoAutoHyph;
    unsigned char fWindowControl;
    long dxaRight;
    long dxaLeft;
    long dxaLeft1;
    LSPD lspd;
    unsigned long dyaBefore;
    unsigned long dyaAfter;
    PHE phe;
    unsigned char fCrLf;
    unsigned char fUsePgsuSettings;
    unsigned char fAdjustRight;
    unsigned char reserved4;
    unsigned char fKinsoku;
    unsigned char fWordWarp;
    unsigned char fOverflowPunct;
    unsigned char fTopLinePunct;
    unsigned char fAutoSpaceDE;
    unsigned char fAutoSpaceDN;
    unsigned short fAlignFont;
    short fVertical:1;
    short fBackward:1;
    short fRotateFont:1;
    short reserved5:13;
    short reserved6;
    char fInTable;
    char fTtp;
    char wr;
    char fLocked;
    TAP ptap;   // pointer???
    long dxaAbs;
    long dyaAbs;
    long dxaWidth;
    BRC brcTop;
    BRC brcLeft;
    BRC brcBottom;
    BRC brcRight;
    BRC brcBetween;
    BRC brcBar;
    long dxaFromText;
    long dyaFromText;
    short dyaHeigth:15;
    short fMinHeight:1;
    SHD shd;
    DCS dcs;
    char lvl;
    char fNumRMIns;
    ANLD anld;
    short fPropRMark;
    short ibstPropRMark;
    DTTM dttmPropRMark;
    NUMRM numrm;
    short itbdMac;
    short rgdxaTab[64];
    char rgtbd[64];

    bool ok;
};
#endif // PAP_H

/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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

// SEP, Section Properties

#ifndef SEP_H
#define SEP_H

#include "brc.h"
#include "dttm.h"
#include "olst.h"

struct SEP {
    unsigned char bck;
    unsigned char fTitlePage;
    char fAutoPgn;
    unsigned char nfcPgn;
    unsigned char fUnlocked;
    unsigned char cnsPgn;
    unsigned char fPgnRestart;
    unsigned char fEndNote;
    char lnc;
    char grpfIhdt;
    unsigned short nLnnMod;
    int dxaLnn;
    short dxaPgn;
    short dyaPgn;
    char fLBetween;
    char vjc;
    unsigned short dmBinFirst;
    unsigned short dmBinOther;
    unsigned short dmPaperReq;
    BRC brcTop;
    BRC brcLeft;
    BRC brcBottom;
    BRC brcRight;
    short fPropRMark;
    short ibstPropRMark;
    DTTM dttmPropRMark;
    int dxtCharSpace;
    int dyaLinePitch;
    unsigned short clm;
    short reserved;
    unsigned char dmOrientPage;
    unsigned char iHeadingPgn;
    unsigned short pgnStart;
    short lnnMin;
    unsigned short wTextFlow;
    short reserved2;
    short pgbProp;
    short pgbApplyTo:3;
    short pgbPageDepth:2;
    short pgbOffsetFrom:3;
    short reserved3:8;
    unsigned int xaPage;
    unsigned int yaPage;
    unsigned int xaPageNUp;
    unsigned int yaPageNUp;
    unsigned int dxaLeft;
    unsigned int dxaRight;
    int dyaTop;
    int dyaBottom;
    unsigned int dzaGutter;
    unsigned int dyaHdrTop;
    short ccolMl;
    char fEvenlySpaced;
    char reserved4;
    int dxaColumns;
    int rgdxaColumnWidthSpacing[89];
    int dxaColumnWidth;
    unsigned char dmOrientFirst;
    unsigned char fLayout;
    short reserved5;
    OLST olstAnm;

    bool ok;
};
#endif // SEP_H

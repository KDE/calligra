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

// TAP, Table Properties

#ifndef TAP_H
#define TAP_H

#include "tlp.h"
#include "tc.h"
#include "shd.h"
#include "brc.h"

struct TAP {
    short jc;
    int dxaGapHalf;
    int dyaRowHeight;
    unsigned char fCantSplit;
    unsigned char fTableHeader;
    TLP tlp;
    int lwHTMLProps;
    short fCaFull:1;
    short fFirstRow:1;
    short fLastRow:1;
    short fOutline:1;
    short reserved:12;
    short itcMac;
    int dxaAdjust;
    int dxaScale;
    int dxsInch;
    short rgdxaCenter[65];
    short rgdxaCenterPrint[65];
    TC rgtc[64];
    SHD rgshd[64];
    BRC rgbrcTable[6];

    bool ok;
};
#endif // TAP_H

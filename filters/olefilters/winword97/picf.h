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

// PICF, Picture Descriptor (on File)

#ifndef PICF_H
#define PICF_H

#include "brc.h"

struct PICF {
    int lcb;
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

    bool ok;
};
#endif // PICF_H

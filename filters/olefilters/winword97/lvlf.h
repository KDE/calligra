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

// LVLF List LeVel (on File)

#ifndef LVLF_H
#define LVLF_H

struct LVLF {
    long iStartAt;
    char nfc;
    unsigned char jc:2;
    unsigned char fLegal:1;
    unsigned char fNoRestart:1;
    unsigned char fPrev:1;
    unsigned char fPrevSpace:1;
    unsigned char fWord6:1;
    short rgbxchNums[9];
    unsigned char ixchFollow;
    long dxaSpace;
    long daxIndent;
    char cbGrpprlChpx;
    char cbGrpprlPapx;
    short reserved;

    bool ok;
};
#endif // LVLF_H

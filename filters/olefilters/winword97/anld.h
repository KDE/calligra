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

// ANLD, Autonumbered List Data Descriptor

#ifndef ANLD_H
#define ANLD_H

struct ANLD {
    unsigned char nfc;
    unsigned char cxchTextBefore;
    unsigned char cxchTextAfter;
    unsigned char jc:2;
    unsigned char fPrev:1;
    unsigned char fHang:1;
    unsigned char fSetBold:1;
    unsigned char fSetItalic:1;
    unsigned char fSetSmallCaps:1;
    unsigned char fSetCaps:1;
    unsigned char fSetStrike:1;
    unsigned char fSetKul:1;
    unsigned char fPrevSpace:1;
    unsigned char fBold:1;
    unsigned char fItalic:1;
    unsigned char fSmallCpas:1;
    unsigned char fCaps:1;
    unsigned char fStrike:1;
    unsigned char kul:3;
    unsigned char ico:5;
    short ftc;
    unsigned short hps;
    unsigned short iStartAt;
    unsigned short dxaIndent;
    unsigned short dxaSpace;
    unsigned char fNumber1;
    unsigned char fNumberAcross;
    unsigned char fRestartHdn;
    unsigned char fSpareX;
    short rgxch[32];

    bool ok;
};
#endif // ANLD_H

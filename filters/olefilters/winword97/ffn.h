/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

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

// FFN, Font Family Name

#ifndef FFN_H
#define FFN_H

struct FFN {
    unsigned char cbFfnM1;
    unsigned char prq:2;
    unsigned char fTrueType:1;
    unsigned char reserved:1;
    unsigned char ff:3;
    unsigned char reserved2:1;
    short wWeight;
    unsigned char chs;
    unsigned char ixchSzAlt;
    char panose[10];   // don't know ;(
    char fs[24];       // see above
    short *xszFfn;

    bool ok;
};
#endif // FFN_H

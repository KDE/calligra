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

// PHE, Paragraph Height

#ifndef PHE_H
#define PHE_H

struct PHE {
    short fSpare:1;
    short fUnk:1;
    short fDiffLines:1;
    short reserved:5;
    short clMac:8;
    short reserved2;
    long dxaCol;
    long dymLine;
    long dymHeight;

    bool ok;
};

struct PHE2 {    // see page 114
    long fSpare:1;
    long fUnk:1;
    long dcpTtpNext:30;
    long dxaCol;
    long dymTableHeight;

    bool ok;
};
#endif // PHE_H

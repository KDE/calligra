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

// FSPA

#ifndef FSPA_H
#define FSPA_H

struct FSPA {
    int spid;
    int xaLeft;
    int yaTop;
    int xaRight;
    int yaBottom;
    unsigned short fHdr:1;
    unsigned short bx:2;
    unsigned short by:2;
    unsigned short wr:4;
    unsigned short wrk:4;
    unsigned short fRcaSimple:1;
    unsigned short fBelowText:1;
    unsigned short fAnchorLock:1;
    int cTxbx;

    bool ok;
};
#endif //FSPA _H

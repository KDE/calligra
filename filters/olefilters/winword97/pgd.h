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

// PGD, Page Descriptor

#ifndef PGD_H
#define PGD_H

struct PGD {
    short fContinue:1;
    short fUnk:1;
    short fRight:1;
    short fPgnRestart:1;
    short fEmptyPage:1;
    short fAllFtn:1;
    short unused:1;
    short fTableBreaks:1;
    short fMarked:1;
    short fColumnBreaks:1;
    short fTableHeader:1;
    short fNewPage:1;
    short bkc:4;
    unsigned short lnn;
    unsigned short pgn;
    int dym;

    bool ok;
};
#endif // PGD_H

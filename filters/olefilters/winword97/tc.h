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

// TC, Table Cell Descriptor

#ifndef TC_H
#define TC_H

#include "brc.h"

struct TC {
    short fFirstMerged:1;
    short fMerged:1;
    short fVertical:1;
    short fBackward:1;
    short fRotateFont;
    short fVertMerge:1;
    short fVertRestart:1;
    short vertAlign:2;
    short fUnused:7;
    unsigned short wUnused;
    BRC brcTop;
    BRC brcLeft;
    BRC brcBottom;
    BRC brcRight;

    bool ok;
};
#endif // TC_H

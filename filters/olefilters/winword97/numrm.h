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

// NUMRM, Number Revision Mark Data

#ifndef NUMRM_H
#define NUMRM_H

#include "dttm.h"

struct NUMRM {
    unsigned char fNumRM;
    unsigned char Spare;
    short ibstNumRM;
    DTTM dttmNumRM;
    unsigned char rgbxchNums[9];
    unsigned char rgnfc[9];
    short Spare2;
    int PNBR[9];
    short xst[32];

    bool ok;
};
#endif // NUMRM_H

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

// DOPTYPOGRAPHY, Document Typography Info

#ifndef DOPTYPOGRAPHY_H
#define DOPTYPOGRAPHY_H

struct DOPTYPOGRAPHY {
    short fKerningPunct:1;
    short iJustification:2;
    short iLevelOfKinsoku:2;
    short f2on1:1;
    short reserved:10;
    short cchFollowingPunct;
    short cchLeadingPunct;
    short rgxchFPunct[101];
    short rgxchLPunct[51];

    bool ok;
};
#endif // DOPTYPOGRAPHY_H

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

// DOGRID, Drawing Object Grid

#ifndef DOGRID_H
#define DOGRID_H

struct DOGRID {
    short xaGrid;
    short yaGrid;
    short dxaGrid;
    short dyaGrid;
    short dyGridDisplay:7;
    short fTurnItOff:1;
    short dxGridDisplay:7;
    short fFollowMargins:1;

    bool ok;
};
#endif // DOGRID_H

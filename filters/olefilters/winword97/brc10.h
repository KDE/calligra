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

// BRC10, Border Code for Windows Word 1.0

#ifndef BRC10_H
#define BRC10_H

struct BRC10 {
    short dxpLine2Width:3;
    short dxpSpaceBetween:3;
    short dxpLine1Width:3;
    short dxpSpace:5;
    short fShadow:1;
    short fSpare:1;

    bool ok;
};
#endif // BRC10_H

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

// LFOLVL, List Format Override for a single LeVeL

#ifndef LFOLVL_H
#define LFOLVL_H

struct LFOLVL {
    int iStartAt;
    unsigned char ilvl:4;
    unsigned char fStartAt:1;
    unsigned char fFormatting:1;
    unsigned char reserved:2;
    char reserved2[3];

    bool ok;
};
#endif // LFOLVL_H

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

#include <misc.h>

const unsigned short read16(const unsigned char *d) {
    return ( (*(d+1) << 8) + *d );
}

const unsigned long read32(const unsigned char *d) {
    return ( (read16(d+2) << 16) + read16(d) );
}

const short char2uni(const unsigned char &c) {
    if(c<=0x7f || c>=0xa0)
        return static_cast<short>(c);
    else
        return CP2UNI[c-0x80];
}

void align2(unsigned long &adr) {
    if((adr%2)!=0)
        ++adr;
}


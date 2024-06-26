/* This file is part of the wvWare 2 project
   SPDX-FileCopyrightText: 2002-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#include "headers97.h"

using namespace wvWare;

Headers97::Headers97(U32 ccpHdd, U32 fcPlcfhdd, U32 lcbPlcfhdd, U32 fcPlcfsed, U32 lcbPlcfsed, OLEStreamReader *tableStream)
    : Headers(ccpHdd, fcPlcfhdd, lcbPlcfhdd, fcPlcfsed, lcbPlcfsed, tableStream, Word8)
{
}

std::pair<U32, U32> Headers97::findHeader(int sectionNumber, unsigned char mask) const
{
    // NOTE: An empty header/footer story specifies that the previous section's
    // header/footer of the corresponding type is used.  For the first section,
    // an empty header/footer story specifies that it does not have a
    // header/footer of this type.  MS-DOC, p.34

    U32 start = 0;
    U32 lim = 0;
    const int offset = maskToOffset(mask);
    do {
        const int tmp = sectionNumber * headerTypes + offset;
        start = m_headers[tmp];
        lim = m_headers[tmp + 1];
        --sectionNumber;
    } while (start == lim && sectionNumber >= 0);

    return std::make_pair(start, lim);
}

int Headers97::maskToOffset(unsigned char mask) const
{
    int offset = 0;
    while (mask != 0 && (mask & 1) == 0) {
        ++offset;
        mask >>= 1;
    }
    return offset;
}

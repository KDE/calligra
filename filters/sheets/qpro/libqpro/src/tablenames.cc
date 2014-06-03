/*
 *  Copyright (c) 2001 Graham Short.  <grahshrt@netscape.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <qpro/common.h>

#include <string.h>

#include "qpro/tablenames.h"


// ------------------------------------------------------------------

QpTableNames::QpTableNames()
{
    for (int lIdx = 0; lIdx < cNameCnt; ++lIdx) {
        cName[lIdx] = 0;
    }
}

QpTableNames::~QpTableNames()
{
    for (int lIdx = 0; lIdx < cNameCnt; ++lIdx) {
        delete [] cName[lIdx];
        cName[lIdx] = 0;
    }
}


void
QpTableNames::name(unsigned pIdx, const char* pName)
{
    if (pIdx < cNameCnt) {
        delete [] cName[pIdx];
        cName[pIdx] = strcpy(new char[strlen(pName)+1], pName);
    }
}

const char*
QpTableNames::name(unsigned pIdx)
{
    char* lResult = 0;

    if (pIdx < cNameCnt) {
        if ((lResult = cName[pIdx]) == 0) {
            if (pIdx < 26) {
                lResult = cName[pIdx] = new char[2];
                lResult[0] = (char)('A' + pIdx);
                lResult[1] = '\0';
            } else {
                lResult = cName[pIdx] = new char[2];
                lResult[0] = (char)('A' - 1 + pIdx / 26);
                lResult[1] = (char)('A' + pIdx % 26);
                lResult[2] = '\0';
            }
        }
    }

    return lResult;
}


int
QpTableNames::allocated(unsigned pIdx)
{
    return (pIdx >= cNameCnt ? 0 : (cName[pIdx] != 0));
}


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

#include <iostream>

#include <qpro/record_factory.h>

#define NEWFUNC(x) static QpRec* NEW_##x (QP_INT16 pLen, QpIStream& pIn) { return new x (pLen, pIn); }

NEWFUNC(QpRecBof)
NEWFUNC(QpRecBop)
NEWFUNC(QpRecEof)
NEWFUNC(QpRecEmptyCell)
NEWFUNC(QpRecFloatingPointCell)
NEWFUNC(QpRecFormulaCell)
NEWFUNC(QpRecIntegerCell)
NEWFUNC(QpRecLabelCell)
NEWFUNC(QpRecPageName)
NEWFUNC(QpRecPassword)
NEWFUNC(QpRecRecalcMode)
NEWFUNC(QpRecRecalcOrder)

struct Record {
    QP_INT16  Type;
    QP_INT16  Len;
    QpRec*(*Func)(QP_INT16, QpIStream&);
};


// The functions in the Record table below (NEW_QpRecBof etc.)
// come from the NEWFUNC #define above

static Record gRecord[] = {
    {QpBof,                2,  NEW_QpRecBof},
    {QpEof,                0,  NEW_QpRecEof},
    {QpRecalcMode,         1,  NEW_QpRecRecalcMode},
    {QpRecalcOrder,        1,  NEW_QpRecRecalcOrder},
    {QpEmptyCell,          6,  NEW_QpRecEmptyCell},
    {QpIntegerCell,        8,  NEW_QpRecIntegerCell},
    {QpFloatingPointCell, 14,  NEW_QpRecFloatingPointCell},
    {QpLabelCell,          0,  NEW_QpRecLabelCell},
    {QpFormulaCell,        0,  NEW_QpRecFormulaCell},
    {QpBop,                0,  NEW_QpRecBop},
    {QpPageName,           0,  NEW_QpRecPageName},
    {QpPassword,           0,  NEW_QpRecPassword},
    {0,                    0,  0}
};

QpRecFactory::QpRecFactory(QpIStream& pIn)
        : cIn(pIn)
{
}

QpRecFactory::~QpRecFactory()
{
}

QpRec*
QpRecFactory::nextRecord()
{
    QP_INT16  lType;
    QP_INT16  lLen;
    QpRec* lResult = 0;

    cIn >> lType >> lLen;

    for (Record* lRecord = gRecord; lResult == 0 ; ++lRecord) {
        if (lRecord->Func == 0) {
            lResult = new QpRecUnknown(lType, lLen, cIn);
        } else if (lRecord->Type == lType) {
            // ??? check length
            lResult = lRecord->Func(lLen, cIn);
        }
    }

    return lResult;
}

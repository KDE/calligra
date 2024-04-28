/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2014 Lassi Nieminen <lassniem@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "UnitConversions.h"

qreal ptToHalfPt(qreal point)
{
    return point * (qreal)2.0;
}

qreal inToHalfPt(qreal in)
{
    return in * (qreal)72.0;
}

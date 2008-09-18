/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "krscriptline.h"
#include <krlinedata.h>

namespace Scripting
{

Line::Line(KRLineData* l)
{
    _line = l;
}

Line::~Line()
{
}


QColor Line::lineColor()
{
    return _line->_lnColor->value().value<QColor>();
}

void Line::setLineColor(const QColor& c)
{
    _line->_lnColor->setValue(c);
}

int Line::lineWeight()
{
    return _line->_lnWeight->value().toInt();
}

void Line::setLineWeight(int w)
{
    _line->_lnWeight->setValue(w);
}

int Line::lineStyle()
{
    return _line->_lnStyle->value().toInt();
}
void Line::setLineStyle(int s)
{
    if (s < 0 || s > 5) {
        s = 1;
    }
    _line->_lnStyle->setValue(s);
}

QPointF Line::startPosition()
{
    return _line->_start.toPoint();
}

void Line::setStartPosition(const QPointF& p)
{
    _line->_start.setPointPos(p);
}

QPointF Line::endPosition()
{
    return _line->_end.toPoint();
}

void Line::setEndPosition(const QPointF& p)
{
    _line->_end.setPointPos(p);
}
}

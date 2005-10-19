/* This file is part of the KDE project

   Copyright 2005 Robert Knight <robertknight@gmail.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "highlight_range.h"

HighlightRange::HighlightRange(const HighlightRange& rhs)
{
    rhs._firstCell ? _firstCell=new KSpreadPoint(*(rhs._firstCell )) : _firstCell=0;
    rhs._lastCell  ? _lastCell=new KSpreadPoint(*(rhs._lastCell )) : _lastCell=0;
    _color=QColor(rhs._color);
}

void HighlightRange::getRange(KSpreadRange& rg)
{
    if (!_firstCell)
    {
        rg=KSpreadRange();
        return;
    }

    if (_lastCell)
    {
        rg=KSpreadRange(*_firstCell,*_lastCell); 	
    }
    else
    {
        rg=KSpreadRange(*_firstCell,*_firstCell);
    }
}

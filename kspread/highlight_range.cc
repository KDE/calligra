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


using namespace KSpread;


HighlightRange::HighlightRange() : Range() , _color(Qt::black) {}
HighlightRange::HighlightRange(const QString& rangeReference) : Range(rangeReference) , _color(Qt::black) {}
HighlightRange::HighlightRange(const QString& rangeReference, Map* workbook, Sheet* sheet)
: Range(rangeReference,workbook,sheet) , _color(Qt::black) {}
HighlightRange::HighlightRange(const Point& upperLeft, const Point& lowerRight) 
: Range(upperLeft,lowerRight) , _color(Qt::black) {}

HighlightRange::HighlightRange(const HighlightRange& rhs) 
: Range(rhs) , KShared() , _color(rhs.color()) {}




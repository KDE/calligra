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

#ifndef KSPREAD_HIGHLIGHT_RANGE_H
#define KSPREAD_HIGHLIGHT_RANGE_H

#include <qcolor.h>

#include "ksharedptr.h"

#include "kspread_util.h"


namespace KSpread
{

/**
 * Holds information about a range of cells to be highlighted (ie. a coloured border is to be
 * drawn around them).  This is used to highlight areas on the sheet which are referenced in
 * a formula that is being edited.
 *
 * The drawing of highlighted ranges is performed by the @ref Canvas class.
 */
class HighlightRange : public Range , public KShared
{
    public:
        /**
        * Constructs a new HighlightRange object.  The new HighlightRange will
        * have no area associated with it and its color will be set to black.
        */
        HighlightRange();
        HighlightRange(const QString& rangeReference);
        HighlightRange(const QString& rangeReference, Map* workbook, Sheet* sheet);
        HighlightRange(const Point& upperLeft, const Point& lowerRight);
       
	/**
	* Creates a copy of a HighlightRange.  The new object will have the same range and
	* colour as the existing HighlightRange.
	*/
        HighlightRange(const HighlightRange& rhs);
        

        /**
        * Sets the colour which should be used to highlight the range.
        */
        void setColor(QColor& clr) {_color=clr;}
        QColor color() const {return _color;}

    protected:
        QColor   _color; //Colour to highlight this range with
};


} // namespace KSpread

#endif

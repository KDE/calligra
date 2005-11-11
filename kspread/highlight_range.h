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
class HighlightRange
{
    public:
        /**
        * Constructs a new HighlightRange object.  The new HighlightRange will
        * have no area associated with it and its color will be set to black.
        */
        HighlightRange() : _firstCell(0),_lastCell(0),_color(QColor(0,0,0)) {}

	/**
	* Creates a copy of a HighlightRange.  The new object will have the same range and
	* colour as the existing HighlightRange.
	*/
        HighlightRange(const HighlightRange& rhs);

        virtual ~HighlightRange() {delete _firstCell; delete _lastCell;
            _firstCell=0;_lastCell=0;}


        /**
        * Returns the range which should be highlighted.
        *
        * @param rg A Range object which receives the range to be highlighted.
        */
        void getRange(Range& rg);

        /**
        * Sets the first cell in the range
	*
	* @param fc A KSpreadPoint object containing the co-ordinates (in rows and columns)
	* of the first cell in the highlighted range.
        */
        void setFirstCell(Point* fc) {_firstCell=fc;}
        Point* firstCell() {return _firstCell;}

        /**
        * Sets the last cell in the range.
	*
	* @param lc A KSpreadPoint object containing the co-ordinates (in rows and columns) of 
	* the last cell in the highlighted range.  This pointer can be null, in which case
	* only the first cell (set with @ref setFirstCell ) will be highlighted
        */
        void setLastCell(Point* lc) {_lastCell=lc;}
        Point* lastCell() {return _lastCell;}

        /**
        * Sets the colour which should be used to highlight the range.
        */
        void setColor(QColor& clr) {_color=clr;}
        QColor color() {return _color;}

	/**
	* Convenience function to change the area of a highlighted range.  This will cause an areaChanged() signal to be emitted.
	* @param newArea The area in rows and columns that the range should occupy.  
	*/
	void setArea(const QRect& newArea);

    signals:
	/** Emitted when the area of the range has been changed via a call to @ref setArea.*/
	void areaChanged(Sheet* sheet, QRect& oldArea, QRect& newArea);

    protected:
        Point* _firstCell; //First cell in range, will either be in same row or same col as last cell
        Point* _lastCell; //Last cell in range, will either be in same row or same col as first cell, Will be 0 for single-cells
        QColor   _color; //Colour to highlight this range with
};


} // namespace KSpread

#endif

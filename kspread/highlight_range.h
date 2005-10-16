/* This file is part of the KDE project

   Copyright 1999-2004 The KSpread Team <koffice-devel@mail.kde.org>

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

   Original Author: Robert Knight <robertknight@gmail.com>
*/



//Qt
#include <qcolor.h>


//KSpread
#include "kspread_util.h"

/**
 * Holds information about a range of cells to be highlighted (ie. a coloured border is to be
 * drawn around them).  This is used to highlight areas on the sheet which are referenced in
 * a formula that is being edited.
 * 
 * The drawing of highlighted ranges is performed by the @ref KSpreadCanvas class.
 */
class HighlightRange
{
	public:
		/**
		* Constructs a new HighlightRange object.  The new HighlightRange will
		* have no area associated with it and its color will be set to black.
		*/
		HighlightRange() : _firstCell(0),_lastCell(0),_color(QColor(0,0,0)) {}
		HighlightRange(const HighlightRange& rhs);

		virtual ~HighlightRange() {delete _firstCell; delete _lastCell;
			_firstCell=0;_lastCell=0;}
		

		/**
		* Returns the range which should be highlighted.
		*
		* @param rg A KSpreadRange object which receives the range to be highlighted.
		*/
		void getRange(KSpreadRange& rg); 

		/**
		* Sets the first cell in the range 
		*/
		void setFirstCell(KSpreadPoint* fc) {_firstCell=fc;}
		KSpreadPoint* firstCell() {return _firstCell;}
		
		/**
		* Sets the last cell in the range.
		*/
		void setLastCell(KSpreadPoint* lc) {_lastCell=lc;}
		KSpreadPoint* lastCell() {return _lastCell;}

		/**
		* Sets the colour which should be used to highlight the range.
		*/
		void setColor(QColor& clr) {_color=clr;}
		QColor color() {return _color;}
	
	protected:
		KSpreadPoint* _firstCell; //First cell in range, will either be in same row or same col as last cell
		KSpreadPoint* _lastCell; //Last cell in range, will either be in same row or same col as first cell, Will be 0 for single-cells 
		QColor	 _color; //Colour to highlight this range with	
};

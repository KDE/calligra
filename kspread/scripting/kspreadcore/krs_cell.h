/*
 *  Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KROSS_KSPREADCOREKRSCELL_H
#define KROSS_KSPREADCOREKRSCELL_H

#include <kspread_sheet.h>
#include <kspread_cell.h>
#include <kspread_value.h>

#include <api/class.h>

namespace Kross { namespace KSpreadCore {

class Cell : public Kross::Api::Class<Cell>
{
    public:
        Cell(KSpread::Cell* cell, KSpread::Sheet* sheet, uint col, uint row);
        virtual ~Cell();
        virtual const QString getClassName() const;
    private:
	/**
	 * Return the value of the cell
	 */
	QVariant value();
	/**
	 * Return the text of the cell (the formula if there is one,
	 * the value otherwise)
	 */
	const QString text() const;
	/**
	 * Set the text of the cell. If asString is true, the text
	 * will be handled as string else we try to parse the
	 * string to the expected value.
	 */
	bool setText(const QString& text, bool asString = false);

#if 0
	/**
	 * Set the text color of the cell
	 */
	Kross::Api::Object::Ptr setTextColor(Kross::Api::List::Ptr);
	/**
	 * Set the background color of the cell
	 */
	Kross::Api::Object::Ptr setBackgroundColor(Kross::Api::List::Ptr);
#endif

    private:
	KSpread::Cell* m_cell;
	KSpread::Sheet* m_sheet;
	uint m_col;
	uint m_row;
};
}
}


#endif

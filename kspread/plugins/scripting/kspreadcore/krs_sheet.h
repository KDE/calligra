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

#ifndef KROSS_KSPREADCOREKRSSHEET_H
#define KROSS_KSPREADCOREKRSSHEET_H

#include <kspread_sheet.h>
#include <kspread_value.h>

#include <api/class.h>

namespace Kross { namespace KSpreadCore {

class Cell;

class Sheet : public Kross::Api::Class<Sheet>
{
    public:
        Sheet(KSpread::Sheet* sheet, KSpread::Doc* doc = 0);
        virtual ~Sheet();
        virtual const QString getClassName() const;
    private:

	/**
	 * Return the name of the sheet
	 */
	const QString getName() const;
	/**
	 * Set the name of the sheet
	 */
	void setName(const QString& name);
	/**
	 * Return a given cell
	 */
	Cell* cell(uint col, uint row);
	/**
	 * Add a new row
	 */
	bool insertRow(uint row);
	/**
	 * Add a new column
	 */
	bool insertColumn(uint col);
	/**
	 * Remove a row
	 */
	void removeRow(uint row);
	/**
	 * Remove a column
	 */
	void removeColumn(uint col);

    private:
	KSpread::Sheet* m_sheet;
	KSpread::Doc* m_doc;
};
}
}

#endif

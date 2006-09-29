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

/**
 * A sheet in a document.
 *
 * Example (in Ruby) :
 * @code
 * doc = krosskspreadcore::get("KSpreadDocument")
 * sheet1 = doc.sheetByName("Sheet1")
 * sheet2 = doc.sheetByName("Sheet2")
 * cell1 = sheet1.firstCell()
 * while cell1
 *     colnr = cell1.column()
 *     rownr = cell1.row()
 *     cell2 = sheet2.cell(colnr, rownr)
 *     cell2.setValue( cell1.value() )
 *     cell1 = cell1.nextCell()
 * end
 * @endcode
 */
class Sheet : public Kross::Api::Class<Sheet>
{
    public:
        Sheet(KSpread::Sheet* sheet, KSpread::Doc* doc = 0);
        virtual ~Sheet();
        virtual const QString getClassName() const;
    private:

	/**
	 * Return the name of the sheet.
	 */
	const QString name() const;
	/**
	 * Set the name of the sheet.
	 */
	void setName(const QString& name);

	/**
	 * Return the currently maximum defined number of columns.
	 */
	int maxColumn() const;
	/**
	 * Return the currently maximum defined number of rows.
	 */
	int maxRow() const;

	/**
	 * Return the first cell. Use the firstCell() and nextCell()
	 * methods as outlined in the example to iterate only through
	 * filled cells (aka cells with content).
	 *
	 * Example (in Python) :
	 * @code
	 * import krosskspreadcore
	 * doc = krosskspreadcore.get("KSpreadDocument")
	 * sheet = doc.currentSheet()
	 * cell = sheet.firstCell()
	 * while cell:
	 *     print "Cell col=%s row=%s value=%s" % (cell.column(),cell.row(),cell.value())
	 *     cell = cell.nextCell()
	 * @endcode
	 */
	Cell* firstCell() const;

	/**
	 * Return the given cell. The first parameter is the column-number
	 * while the second defines the rownumber. The first cell starts
	 * with 0,0. If you like to iterate over all cells that have content,
	 * use the firstCell() and nextCell() methods which is much faster
	 * cause empty cells are ignored.
	 *
	 * Example (in Python) :
	 * @code
	 * import krosskspreadcore
	 * doc = krosskspreadcore.get("KSpreadDocument")
	 * sheet = doc.currentSheet()
	 * for colnr in range( sheet.maxColumn() ):
	 *     for rownr in range( sheet.maxRow() ):
	 *         cell = sheet.cell(colnr, rownr)
	 *         if cell.value() != None:
	 *             print "Cell col=%s row=%s value=%s" % (colnr,rownr,cell.value())
	 * @endcode
	 */
	Cell* cell(uint col, uint row);

	/**
	 * Add a new row.
	 */
	bool insertRow(uint row);
	/**
	 * Add a new column.
	 */
	bool insertColumn(uint col);

	/**
	 * Remove a row.
	 */
	void removeRow(uint row);
	/**
	 * Remove a column.
	 */
	void removeColumn(uint col);

    private:
	KSpread::Sheet* m_sheet;
	KSpread::Doc* m_doc;
};
}
}

#endif

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

#include <Sheet.h>
#include <Cell.h>
#include <Value.h>

#include <api/class.h>

#include <QString>
#include <QColor>

namespace Kross { namespace KSpreadCore {

/**
 * For every cell in the spread sheet there is a Cell object.
 *
 * Cell contains format information and algorithm and it
 * contains the calculation algorithm.
 *
 * Example (in Ruby) :
 * @code
 * doc = krosskspreadcore::get("KSpreadDocument")
 * sheet = doc.currentSheet()
 * cellA1 = sheet.cell(0, 0)
 * cellA2 = sheet.cell(0, 1)
 * cellA2.setValue( cellA1.value() )
 * cellA2.setTextColor( "#ff0000" )
 * @endcode
 */
class Cell : public Kross::Api::Class<Cell>
{
	public:
		Cell(KSpread::Cell* cell, KSpread::Sheet* sheet, uint col, uint row);
		virtual ~Cell();
		virtual const QString getClassName() const;
	private:

		/**
		* Return the value of the cell.
		*/
		QVariant value() const;
		/**
		* Set the value the cell has.
		*/
		bool setValue(const QVariant& value);

		/**
		* Return the column number.
		*/
		int column() const;
		/**
		* Return the row number.
		*/
		int row() const;

		/**
		* Return the previous cell if there is any.
		*/
		Cell* previousCell() const;
		/**
		* Return the next cell if there is any.
		*/
		Cell* nextCell() const;
		/**
		* Set the previous cell.
		*/
		void setPreviousCell(Cell* c);
		/**
		* Set the next cell.
		*/
		void setNextCell(Cell* c);

		/**
		* Returns the name of the cell. For example, the cell in first column
		* and first row is "A1".
		*/
		const QString name() const;
		/**
		* Returns the full name of the cell, i.e. including the worksheet name.
		* Example: "Sheet1!A1"
		*/
		const QString fullName() const;

		/**
		* Returns the comment for the cell.
		*/
		const QString comment() const;
		/**
		* Set the comment for the cell.
		*/
		void setComment(const QString& c);

#if 0
		bool isFormula() const;
		Formula *formula () const;
#endif

		/**
		* Returns the format of the cell, e.g. #.##0.00, dd/mmm/yyyy,...
		*/
		const QString getFormatString() const;
		/**
		* Sets the format of the cell, e.g. #.##0.00, dd/mmm/yyyy,...
		*/
		void setFormatString(const QString& format);

		/**
		* Return the text of the cell (the formula if there is one,
		* the value otherwise). This could be a value (e.g. "14.03")
		* or a formula (e.g. "=SUM(A1:A10)")
		*/
		const QString text() const;
		/**
		* Set the text of the cell. If asString is true, the text
		* will be handled as string else we try to parse the
		* string to the expected value.
		*/
		bool setText(const QString& text, bool asString = false);

		/**
		* Return the textcolor as RGB-value in the format "#RRGGBB".
		*/
		const QString textColor();
		/**
		* Set the textcolor to the RGB-value in the format "#RRGGBB"
		* where each of R, G, and B is a single hex digit.
		*/
		void setTextColor(const QString& textcolor);

		/**
		* Return the backgroundcolor as RGB-value in the format "#RRGGBB".
		*/
		const QString backgroundColor();
		/**
		* Set the backgroundcolor to the RGB-value in the format "#RRGGBB"
		* where each of R, G, and B is a single hex digit.
		*/
		void setBackgroundColor(const QString& backgroundcolor);

	private:
		KSpread::Cell* m_cell;
		KSpread::Sheet* m_sheet;
		uint m_col, m_row;
	
		QVariant toVariant(const KSpread::Value& value) const;
};
}
}


#endif

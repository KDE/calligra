/*
**
** Copyright (C) 2002 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __KSPREAD_LATEX_CELL_H__
#define __KSPREAD_LATEX_CELL_H__

#include <QString>
//Added by qt3to4:
#include <QTextStream>

#include "config.h"
#include "format.h"
#include "xmlparser.h"

/***********************************************************************/
/* Class: Cell                                                        */
/***********************************************************************/

class Table;
class Column;

/**
 * This class hold a cell.
 */
class Cell: public Format
{
	
	/* USEFULL DATA */
	long _row;
	long _col;
	QString _text;
	QString _textDataType;
	QString _result;
	QString _resultDataType;

		
	public:
		/**
		 * Constructors
		 *
		 */

		/**
		 * Creates a new instance of Cell.
		 */
		Cell();

		Cell(long row, long col)
		{
			_row = row;
			_col = col;
		}
		
		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of frames.
		 */

		virtual ~Cell();

		/* ==== getters ==== */

		long     getRow() const { return _row; }
		long     getCol() const { return _col; }
		QString  getText() const { return _text; }
		QString  getTextDataType() const { return _textDataType; }
		QString  getResult() const { return _result; }
		QString  getResultDataType() const { return _resultDataType; }

		/* ==== setters ==== */
		void setRow(int r) { _row = r; }
		void setCol(int c) { _col = c; }
		void setText(QString text) { _text = text; }
		void setTextDataType(QString dt) { _textDataType = dt; }
		void setResult(QString result) { _result = result; }
		void setResultDataType(QString dt) { _resultDataType = dt; }
	
		/**
		 * Helpfull functions
		 */
		void     analyse (const QDomNode);
		void     analyseText (const QDomNode);
		void     analyseResult (const QDomNode);
		void     generate  (QTextStream&, Table*);
		
	private:

};

#endif /* __KSPREAD_LATEX_CELL_H__ */



/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2002, 2003 Robert JACOLIN
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

#ifndef __KSPREAD_LATEX_TABLE_H__
#define __KSPREAD_LATEX_TABLE_H__

#include <QString>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QTextStream>

#include "xmlparser.h"
#include "config.h"
//#include "cell.h"

class Cell;
class Column;
class Row;

/***********************************************************************/
/* Class: Table                                                        */
/***********************************************************************/

/**
 * This class hold a table. That is a table of frame (text frame, picture
 * frame, ...). It use a special latex package.
 * The color table and the border of the tables is not yet supported.
 */
class Table: public XmlParser, Config
{
	Q3PtrList<Row> _rows;
	Q3PtrList<Column> _columns;
	Q3PtrList<Cell> _cells;
	
	/* USEFULL DATA */
	int _maxRow, _maxCol;	/* Size of the table (nb of cell) */
	bool _columnNumber;
	bool _borders;
	bool _hide;
	bool _hideZero;
	bool _firstletterupper;
	bool _grid;
	bool _printGrid;
	bool _printCommentIndicator;
	bool _printFormulaIndicator;
	bool _showFormula;
	bool _showFormulaIndicator;
	bool _lcMode;
	QString _name;

	/** PAPER DATA */
	QString _format;
	QString _orientation;
	long _borderRight;
	long _borderLeft;
	long _borderBottom;
	long _borderTop;

	public:
		/**
		 * Constructors
		 *
		 */

		/**
		 * Creates a new instance of Table.
		 */
		Table();
		
		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of frames.
		 */

		virtual ~Table();

		/**
		 * getters
		 */

		int getMaxRow() const { return _maxRow; }
		int getMaxColumn() const { return _maxCol; }
		QString getName() const { return _name; }
		QString getFormat() const { return _format; }
		QString getOrientation() const { return _orientation; }
		long getBorderRight() const { return _borderRight; }
		long getBorderLeft() const { return _borderLeft; }
		long getBorderBottom() const { return _borderBottom; }
		long getBorderTop() const { return _borderTop; }

		bool isColumnNumber() const { return _columnNumber; }
		bool isBorders() const { return _borders; }
		bool isHide() const { return _hide; }
		bool isHideZero() const { return _hideZero; }
		bool isFirstletterupper() const { return _firstletterupper; }
		bool isGrid() const { return _grid; }
		bool isPrintGrid() const { return _printGrid; }
		bool isPrintCommentIndicator() const { return _printCommentIndicator; }
		bool isPrintFormulaIndicator() const { return _printFormulaIndicator; }
		bool isShowFormula() const { return _showFormula; }
		bool isShowFormulaIndicator() const { return _showFormulaIndicator; }
		bool isLCMode() const { return _lcMode; }

		/**
		 * setters
		 */
		void setMaxRow(int r);
		void setMaxColumn(int c);
		void setName(QString name) { _name = name; }
		void setFormat(QString format) { _format = format; }
		void setOrientation(QString orient) { _orientation = orient; }
		void setBorderRight(long br) { _borderRight = br; }
		void setBorderLeft(long bl) { _borderLeft = bl; }
		void setBorderBottom(long bb) { _borderBottom = bb; }
		void setBorderTop(long bt) { _borderTop = bt; }

		void setColumnNumber() { _columnNumber = true; }
		void setBorders() { _borders = true; }
		void setHide() { _hide = true; }
		void setHideZero() { _hideZero = true; }
		void setFirstletterupper() { _firstletterupper = true; }
		void setGrid() { _grid = true; }
		void setPrintGrid() { _printGrid = true; }
		void setPrintCommentIndicator() { _printCommentIndicator = true; }
		void setPrintFormulaIndicator() { _printFormulaIndicator = true; }
		void setShowFormula() { _showFormula = true; }
		void setShowFormulaIndicator() { _showFormulaIndicator = true; }
		void setLCMode() { _lcMode = true; }
	
		/**
		 * Helpfull functions
		 */

		/**
		 * Return one specific cell.
		 * 
		 * @param col Cell column.
		 * @param row Row cell.
		 */
		Cell* searchCell(int col,int row);
		
		/**
		 * Return one specific column which describe the format of the column.
		 * 
		 * @param col the column.
		 */
		Column* searchColumn(int col);

		/**
		 * Return one specific row which describe the format of the row.
		 *
		 * @param row The row number.
		 */
		Row* searchRow(int row);
                
		void     analyse (const QDomNode);
		void     analysePaper (const QDomNode);
		void     generate  (QTextStream&);

	private:
		void generateCell(QTextStream&, int, int);
		void generateTableHeader(QTextStream&);
		void generateTopLineBorder(QTextStream&, int);
		void generateBottomLineBorder(QTextStream&, int);
};

#endif /* __KSPREAD_LATEX_TABLE_H__ */


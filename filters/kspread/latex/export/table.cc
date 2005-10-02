/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000 Robert JACOLIN
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

#include <kdebug.h>		/* for kdDebug stream */
#include <qbitarray.h>
#include "cell.h"
#include "column.h"
#include "row.h"
#include "table.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Table::Table()
{
	_maxCol = 0;
	_maxRow = 0;
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Table::~Table()
{
}

void Table::setMaxColumn(int col)
{
	if(_maxCol < col) _maxCol = col;
}

void Table::setMaxRow(int row)
{
	if(_maxRow < row) _maxRow = row;
}

void Table::analyse(const QDomNode balise)
{
	kdDebug(30522) << "New table" << endl;
	if(getAttr(balise, "columnnumber") == "1")
		setColumnNumber();
	if(getAttr(balise, "borders") == "1")
		setBorders();
	if(getAttr(balise, "hide") == "1")
		setHide();
	if(getAttr(balise, "hidezero") == "1")
		setHideZero();
	if(getAttr(balise, "firstletterupper") == "1")
		setFirstletterupper();
	if(getAttr(balise, "grid") == "1")
		setGrid();
	if(getAttr(balise, "printgrid") == "1")
		setPrintGrid();
	if(getAttr(balise, "printCommentIndicator") == "1")
		setPrintCommentIndicator();
	if(getAttr(balise, "printFormulaIndicator") == "1")
		setPrintFormulaIndicator();
	if(getAttr(balise, "showFormula") == "1")
		setShowFormula();
	if(getAttr(balise, "showFormulaIndicator") == "1")
		setShowFormulaIndicator();
	if(getAttr(balise, "lcmode") == "1")
		setLCMode();
	setName(getAttr(balise, "name"));
	
	analysePaper(getChild(balise, "paper"));

	int max = getNbChild(balise);
	for(int index = 0; index < max; index++)
	{
		QString name = getChildName(balise, index);		
		if(name == "cell")
		{
			kdDebug(30522) << "----- cell -----" << endl;
			Cell* cell = new Cell();
			cell->analyse(getChild(balise, index));
			_cells.append(cell);
			setMaxColumn(cell->getCol());
			setMaxRow(cell->getRow());
		}
		else if(name == "column")
		{
			kdDebug(30522) << "----- column -----" << endl;
			Column* column = new Column();
			column->analyse(getChild(balise, index));
			_columns.append(column);
		}
		else if(name == "row")
		{
			kdDebug(30522) << "----- row -----" << endl;
			Row* row = new Row();
			row->analyse(getChild(balise, index));
			_rows.append(row);
		}
		else
			kdDebug(30522) << "name : " << name << endl;
	}
}

void Table::analysePaper(const QDomNode balise)
{
	setFormat(getAttr(balise, "format"));
	setOrientation(getAttr(balise, "orientation"));

	/* borders */
	QDomNode border = getChild(balise, "borders");
	setBorderRight(getAttr(balise, "right").toLong());
	setBorderLeft(getAttr(balise, "left").toLong());
	setBorderBottom(getAttr(balise, "bottom").toLong());
	setBorderTop(getAttr(balise, "top").toLong());
}

Cell* Table::searchCell(int col, int row)
{
	QPtrListIterator<Cell> it(_cells);

	kdDebug(30522) << "search in list of " << _cells.count() << " cells" << endl;
	Cell *cell = 0;
	while ( (cell = it.current()) != 0 )
	{
		++it;
		kdDebug(30522) << "cell: " << cell->getRow() << "-" << cell->getCol() << endl;
		if(cell->getCol() == col && cell->getRow() == row)
			return cell;
	}
	return NULL;
}

Column* Table::searchColumn(int col)
{
	QPtrListIterator<Column> it(_columns);

	Column *column;
	while ( (column = it.current()) != 0 )
	{
		++it;
		if(column->getCol() == col)
			return column;
	}
	return NULL;
}

Row* Table::searchRow(int rowNumber)
{
	QPtrListIterator<Row> it(_rows);

	Row *row;
	while ( (row = it.current()) != 0 )
	{
		++it;
		if(row->getRow() == rowNumber)
			return row;
	}
	return NULL;
}

/*******************************************/
/* generate                                */
/*******************************************/
void Table::generate(QTextStream& out)
{
	kdDebug(30522) << "GENERATION OF A TABLE " << getMaxRow() << " - " << getMaxColumn()
		<< endl;
	out << endl << "%% " << getName() << endl;
	if(getOrientation() == "Portrait")
	{
		out << "\\begin{sidewaystable}" << endl << endl;
		indent();
		writeIndent(out);
	}
	
	out << "\\begin{tabular}";
	generateTableHeader(out);
	out << endl;
	indent();
	int rowNumber = 1;
	while(rowNumber <= getMaxRow())
	{
		generateTopLineBorder(out, rowNumber);
		Row* row = searchRow(rowNumber);
		if(row != NULL)
			row->generate(out);
		
		for(int col = 1; col <= getMaxColumn(); col++)
		{
			writeIndent(out);
			generateCell(out, rowNumber, col);
			
			if(col < getMaxColumn())
				out << " & "<< endl;
		}
		out << "\\\\" << endl;
		rowNumber++;
	}
	generateBottomLineBorder(out, rowNumber - 1);
	desindent();
	writeIndent(out);
	out << "\\end{tabular}" << endl << endl;
	desindent();
	
	if(getOrientation() == "Portrait")
	{
		out << "\\end{sidewaystable}" << endl;
		desindent();
	}
	/*Element* elt = 0;
	kdDebug(30522) << "GENERATION OF A TABLE " << count() << endl;
	out << endl << "\\begin{tabular}";
	generateTableHeader(out);
	out << endl;
	indent();

	int row= 0;
	while(row <= getMaxRow())
	{
		generateTopLineBorder(out, row);
		for(int col= 0; col <= getMaxCol(); col++)
		{
			writeIndent(out);
	*/
			/* Search the cell in the list */
		/*	elt = searchCell(row, col);

			out << "\\multicolumn{1}{";
			if(elt->hasLeftBorder())
				out << "|";
			out << "m{" << getCellSize(col) << "pt}";
			
			if(elt->hasRightBorder())
				out << "|";
			out << "}{" << endl;

			generateCell(out, row, col);
			out << "}" << endl;
			if(col < getMaxCol())
				out << "&" << endl;
		}
		out << "\\\\" << endl;
		writeIndent(out);
		row = row + 1;
	}
	generateBottomLineBorder(out, row - 1);
	out << "\\end{tabular}" << endl << endl;
	desindent();*/
	kdDebug(30522) << "END OF GENERATINO OF A TABLE" << endl;
}

/*******************************************/
/* generateTopLineBorder                   */
/*******************************************/
void Table::generateTopLineBorder(QTextStream& out, int row)
{
	
	Cell* cell = 0;
	QBitArray border( getMaxColumn() );
	bool fullLine = true;
	for(int index = 1; index <= getMaxColumn(); index++)
	{
		/* Search the cell in the list */
		kdDebug(30522) << "search " << row << ", " << index << endl;
		cell = searchCell(index, row);

		if(cell == NULL)
			cell = new Cell(row, index);

		/* If the element has a border display it here */
		border[ index ] = cell->hasTopBorder();
		if( ! cell->hasTopBorder() )
			fullLine = false;
	}

	if(fullLine)
	{
		/* All column have a top border */
		writeIndent(out);
		out << "\\hline" << endl;
	}
	else
	{
		int index = 0;
		while(index < getMaxColumn())
		{
			if(border[index])
			{
				int begin = index;
				int end;
				index = index + 1;
				while(border[index] && index < getMaxColumn())
				{
					index = index + 1;
				}
				end = index - 1;
				out << "\\cline{" << begin << "-" << end << "} " << endl;
			}
			index = index + 1;
		}
	}
	
	/*Row * row;
	row = searchRow(row);
	if(row != NULL)
		row->generate(out);*/
}

/*******************************************/
/* generateBottomLineBorder                */
/*******************************************/
void Table::generateBottomLineBorder(QTextStream& out, int row)
{
	Cell* cell = 0;
	QBitArray border( getMaxColumn() );
	bool fullLine = true;

	for(int index = 1; index <= getMaxColumn(); index++)
	{
		/* Search the cell in the list */
		cell = searchCell(index, row);

		if(cell == NULL)
			cell = new Cell(row, index);

		/* If the element has a border display it here */
		border[ index ] = cell->hasBottomBorder();
		if( ! cell->hasBottomBorder() )
			fullLine = false;
	}

	if(fullLine)
	{
		/* All column have a bottom border */
		writeIndent(out);
		out << "\\hline" << endl;
	}
	else
	{
		int index = 0;
		while(index < getMaxColumn())
		{
			if(border[index])
			{
				int begin = index;
				int end;
				index = index + 1;
				while(border[index] && index < getMaxColumn())
				{
					index = index + 1;
				}
				end = index - 1;
				out << "\\cline{" << begin << "-" << end << "} " << endl;
			}
			index = index + 1;
		}
	}
}

/*******************************************/
/* generateCell                            */
/*******************************************/
void Table::generateCell(QTextStream& out, int row, int col)
{
	kdDebug(30522) << "GENERATE CELL : " << row << "," << col << endl;

	/* Search the cell in the list */
	Cell *cell = searchCell(col, row);
	if(cell != NULL)
	{	
		kdDebug(30522) << "generate cell with text: " << cell->getText() << endl;
		cell->generate(out, this);
	}

	kdDebug(30522) << "END OF A CELL" << endl;
}

/*******************************************/
/* generateTableHeader                     */
/*******************************************/
void Table::generateTableHeader(QTextStream& out)
{
	Column* column = 0;

	out << "{";

	for(int col = 1; col <= getMaxColumn(); col++)
	{
		column = searchColumn(col);
		if(column != NULL)
			column->generate(out);
		else
		{
			out << "m{20pt}";
		}
	}
	out << "}";

}


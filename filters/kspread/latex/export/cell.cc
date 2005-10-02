/*
** A program to convert the XML rendered by KSpread into LATEX.
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

#include <kdebug.h>		/* for kdDebug stream */

#include "cell.h"
#include "table.h"
#include "column.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Cell::Cell(): Format()
{
	setCol(0);
	setRow(0);
	setText("");
	setTextDataType("none");
	setResultDataType("none");
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Cell::~Cell()
{
}

void Cell::analyse(const QDomNode balise)
{
	_row = getAttr(balise, "row").toLong();
	_col = getAttr(balise, "column").toLong();
	kdDebug(30522) << getRow() << "-" << getCol() << endl;
	Format::analyse(getChild(balise, "format"));
	analyseText(balise);
}

void Cell::analyseText(const QDomNode balise)
{
	setTextDataType( getAttr(getChild(balise, "text"), "dataType"));	
	setText(getData(balise, "text"));
	kdDebug(30522) << "text(" << getTextDataType() << "): " << getText() << endl;
}

/*******************************************/
/* generate                                */
/*******************************************/
void Cell::generate(QTextStream& out, Table* table)
{
	/*if(getMulticol() > 0)
		out << "\\multicol{" << getMulticol() << "}{";
	else*/ if (getMultirow() > 0)
		out << "\\multirow{" << getMultirow() << "}{";
	kdDebug(30522) << "Generate cell..." << endl;

	out << "\\multicolumn{1}{";
	Format::generate(out, table->searchColumn(_col));
	out << "}{" << endl;
	
	if(getTextDataType() == "Str")
	{
		generateTextFormat(out, getText());
		//out << getText();
	}
	
	out << "}" << endl;
	
	/*if(getColSpan() > 0)
		out << "}" << endl;
	else*/ if (getMultirow() > 0)
		out << "}" << endl;
	
		/*Element* elt = 0;
	kdDebug(30522) << "GENERATION OF A TABLE " << count() << endl;
	out << endl << "\\begin{tabular}";
	generateCellHeader(out);
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
	kdDebug(30522) << "END OF GENERATINO OF A CELL" << endl;
}


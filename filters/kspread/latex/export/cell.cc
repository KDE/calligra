/*
** A program to convert the XML rendered by KSpread into LATEX.
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
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#include <kdebug.h>		/* for kdDebug stream */

#include "cell.h"

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
	Format::analyse(getChild(balise, "format"));
	analyseText(balise);
}

void Cell::analyseText(const QDomNode balise)
{
	setTextDataType( getAttr(getChild(balise, "text"), "dataType"));	
	setText(getData(balise, 0));
	kdDebug() << "text: " << getText() << endl;
}

/*******************************************/
/* generate                                */
/*******************************************/
void Cell::generate(QTextStream& out)
{
	/*if(getColSpan() > 0)
		out << "\\multicol{" << getColSpan() << "}{";
	else if (getRowSpan() > 0)
		out << "\\multirow{" << getRowSpan() << "}{";*/
	kdDebug() << "Generate cell..." << endl;
	if(getTextDataType() == "Str")
		out << getText();
	
	/*if(getColSpan() > 0)
		out << "}" << endl;
	else if (getRowSpan() > 0)
		out << "}" << endl;*/
	/*Element* elt = 0;
	kdDebug() << "GENERATION OF A TABLE " << count() << endl;
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
	kdDebug() << "END OF GENERATINO OF A CELL" << endl;
}


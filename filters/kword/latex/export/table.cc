/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000,2002 Robert JACOLIN
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
#include "listtable.h"
#include "textFrame.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Table::Table()
{
	setMaxCol(0);
	setMaxRow(0);
}

Table::Table(QString grpMgr)
{
	setGrpMgr(grpMgr);
	setMaxCol(0);
	setMaxRow(0);
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Table::~Table()
{
	kdDebug(30522) << "Destruction of a list of frames" << endl;
}

/*******************************************/
/* getCellFlow                             */
/*******************************************/
EEnv Table::getCellFlow(int col)
{
	for(int row = 0; row<= getMaxRow(); row++)
	{
		Element* elt = at(row * getMaxRow() + col);
		if(elt->getType() == ST_TEXT)
		{
			kdDebug(30522) << ((TextFrame*) elt)->getFirstPara()->getEnv() << endl;
			return ((TextFrame*) elt)->getFirstPara()->getEnv();
		}
	}
	kdDebug(30522) << "Default flow for cell" << endl;
	return ENV_JUSTIFY;
}

/*******************************************/
/* getCellFlow                             */
/*******************************************/
double Table::getCellSize(int col)
{

	for(int row = 0; row<= getMaxRow(); row++)
	{
		Element* elt = at(row * getMaxRow() + col);
		if(elt->getType() == ST_TEXT)
		{
			kdDebug(30522) << "size : " << ((TextFrame*) elt)->getLeft() << endl;
			return ((TextFrame*) elt)->getRight() - ((TextFrame*) elt)->getLeft();
		}
	}
	kdDebug(30522) << "Default size for cell" << endl;
	return 3;
}

/*******************************************/
/* searchCell                              */
/*******************************************/
Element* Table::searchCell(int row, int col)
{
	Element* current = 0;

	/* Parcourir les tables et tester chaque nom de table */
	for(current = first(); current != 0; current = next())
	{
		kdDebug(30522) << "+" << current->getRow() << "," << current->getCol() << endl;
		if(current->getRow() == row && current->getCol() == col)
			return current;
	}
	return 0;
}

/*******************************************/
/* append                                  */
/*******************************************/
void Table::append(Element* elt)
{
	if(elt->getRow() > getMaxRow())
		setMaxRow(elt->getRow());

	if(elt->getCol() > getMaxCol())
		setMaxCol(elt->getCol());

	QPtrList<Element>::append(elt);
}

/*******************************************/
/* generate                                */
/*******************************************/
void Table::generate(QTextStream& out)
{
	Element* elt = 0;
	kdDebug(30522) << "GENERATION OF A TABLE " << count() << endl;
	out << endl << "\\begin{tabular}";
	generateTableHeader(out);
	out << endl;
	Config::instance()->indent();

	int row= 0;
	while(row <= getMaxRow())
	{
		generateTopLineBorder(out, row);
		for(int col= 0; col <= getMaxCol(); col++)
		{
			Config::instance()->writeIndent(out);
	
			/* Search the cell in the list */
			elt = searchCell(row, col);

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
		Config::instance()->writeIndent(out);
		row = row + 1;
	}
	generateBottomLineBorder(out, row - 1);
	out << "\\end{tabular}" << endl << endl;
	Config::instance()->desindent();
	kdDebug(30522) << "END OF GENERATINO OF A TABLE" << endl;
}

/*******************************************/
/* generateTopLineBorder                   */
/*******************************************/
void Table::generateTopLineBorder(QTextStream& out, int row)
{
	Element* elt = 0;
	QBitArray border(getMaxCol());
	bool fullLine = true;
	
	for(int index = 0; index <= getMaxCol(); index++)
	{
		/* Search the cell in the list */
		elt = searchCell(row, index);
		kdDebug(30522) << endl << "name (" << row << ", " << index << ") = " << elt->getName() << endl << endl;

		/* If the element has a border display it here */
		if(elt->hasTopBorder())
		{
			border[index] = 1;
		}
		else
		{
			border[index] = 0;
			fullLine = false;
		}
	}

	if(fullLine)
	{
		/* All column have a top border */
		Config::instance()->writeIndent(out);
		out << "\\hline" << endl;
	}
	else
	{
		int index = 0;
		while(index <= getMaxCol())
		{
			if(border[index])
			{
				int begin = index;
				int end = index;
				while(border[index] && index < getMaxCol())
				{
					index = index + 1;
				}
				end = index - 1;
				out << "\\cline{" << (begin + 1) << "-" << (end + 1) << "} " << endl;
			}
			index = index + 1;
		}
	}
}

/*******************************************/
/* generateBottomLineBorder                */
/*******************************************/
void Table::generateBottomLineBorder(QTextStream& out, int row)
{
	Element* elt = 0;
	QBitArray border(getMaxCol());
	bool fullLine = true;

	for(int index = 0; index <= getMaxCol(); index++)
	{
		/* Search the cell in the list */
		elt = searchCell(row, index);

		/* If the element has a border display it here */
		if(elt->hasBottomBorder())
		{
			border[index] = 1;
		}
		else
		{
			border[index] = 0;
			fullLine = false;
		}
	}

	if(fullLine)
	{
		/* All column have a top border */
		Config::instance()->writeIndent(out);
		out << "\\hline" << endl;
	}
	else
	{
		int index = 0;
		while(index <= getMaxCol())
		{
			if(border[index])
			{
				int begin = index;
				int end = index;
				while(border[index] && index <= getMaxCol())
				{
					index = index + 1;
				}
				end = index - 1;
				out << "\\cline{" << (begin + 1) << "-" << (end + 1) << "} " << endl;
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
	Element* elt = 0;

	kdDebug(30522) << "NEW CELL : " << row << "," << col << endl;

	/* Search the cell in the list */
	elt = searchCell(row, col);

	/* Generate it */
	if(elt != 0)
		elt->generate(out);
	kdDebug(30522) << "END OF A CELL" << endl;
}

/*******************************************/
/* generateTableHeader                     */
/*******************************************/
void Table::generateTableHeader(QTextStream& out)
{
	Element* elt = 0;
	bool fullRightBorder = true;
	bool fullLeftBorder = true;

	out << "{";

	for(int col = 0; col <= getMaxCol(); col++)
	{
		for(int row = 0; row < getMaxRow(); row++)
		{
			/* Search the cell in the list */
			elt = searchCell(row, col);

			/* If the element has a border display it here */
			if(!elt->hasRightBorder())
				fullRightBorder = false;
			if(!elt->hasLeftBorder())
				fullLeftBorder = false;
		}
		if(fullLeftBorder)
			out << "|";
		out << "m{" << getCellSize(col) << "pt}";
		if(fullRightBorder)
			out << "|";
	}
	out << "}";
}


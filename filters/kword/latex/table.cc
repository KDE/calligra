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
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#include <kdebug.h>		/* for kdDebug stream */
#include "listtable.h"
#include "texte.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Table::Table()
{
	_grpMgr = "";
	_name   = "";
	setMaxCol(0);
	setMaxRow(0);
}

Table::Table(QString grpMgr)
{
	_grpMgr = grpMgr;
	_name   = "";
	setMaxCol(0);
	setMaxRow(0);
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Table::~Table()
{
	//Element *elt = 0;
	kdDebug() << "Destruction of a list of frames" << endl;
	/*while(_start != 0)
	{
		elt    = _start;
		_start = _start->getNext();
		delete elt;
		_size = _size - 1;
	}*/
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
			kdDebug() << ((Texte*) elt)->getFirstPara()->getEnv() << endl;
			return ((Texte*) elt)->getFirstPara()->getEnv();
		}
	}
	kdDebug() << "Default flow for cell" << endl;
	return ENV_JUSTIFY;
}

/*******************************************/
/* getCellFlow                             */
/*******************************************/
int Table::getCellSize(int col)
{

	for(int row = 0; row<= getMaxRow(); row++)
	{
		Element* elt = at(row * getMaxRow() + col);
		if(elt->getType() == ST_TEXT)
		{
			kdDebug() << ((Texte*) elt)->getLeft() << endl;
			return ((Texte*) elt)->getRight() - ((Texte*) elt)->getLeft();
		}
	}
	kdDebug() << "Default size for cell" << endl;
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
		kdDebug() << "+" << current->getRow() << "," << current->getCol() << endl;
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

	QList<Element>::append(elt);
}

/*******************************************/
/* generate                                */
/*******************************************/
void Table::generate(QTextStream& out)
{
	kdDebug() << "GENERATION OF A TABLE " << count() << endl;
	out << endl << "\\begin{tabular}";
	generateTableHeader(out);
	out << endl;
	for(int row= 0; row <= getMaxRow(); row++)
	{
		for(int col= 0; col <= getMaxCol(); col++)
		{
			generateCell(out, row, col);
			if(col < getMaxCol())
				out << "&" << endl;
		}
		out << "\\\\" << endl;
	}
	out << "\\end{tabular}" << endl << endl;
	kdDebug() << "END OF GENERATINO OF A TABLE" << endl;
}

/*******************************************/
/* generateCell                            */
/*******************************************/
void Table::generateCell(QTextStream& out, int row, int col)
{
	Element* elt = 0;

	kdDebug() << "NEW CELL : " << row << "," << col << endl;

	/* Search the cell in the list */
	elt = searchCell(row, col);

	/* Generate it */
	if(elt != 0)
		elt->generate(out);
	kdDebug() << "END OF A CELL" << endl;
}

/*******************************************/
/* generateTableHeader                     */
/*******************************************/
void Table::generateTableHeader(QTextStream& out)
{
	out << "{";

	for(int col = 0; col <= getMaxCol(); col++)
	{
		out << "m{" << getCellSize(col) << "pt}";
		/*switch(getCellFlow(col))
		{
			case ENV_JUSTIFY:
					out << "m{5cm}";
				break;
			case ENV_CENTER:
					out << "c";
				break;
			case ENV_LEFT:
					out << "l";
				break;
			case ENV_RIGHT:
					out << "r";
				break;
		}*/
	}
	out << "}";
}



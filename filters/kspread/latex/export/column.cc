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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include <kdebug.h>		/* for kDebug stream */

#include "column.h"
//Added by qt3to4:
#include <QTextStream>

/*******************************************/
/* Constructor                             */
/*******************************************/
Column::Column(): Format()
{
	setCol(0);
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Column::~Column()
{
}

void Column::analyze(const QDomNode node)
{
	_col = getAttr(node, "column").toLong();
	_width = getAttr(node, "width").toDouble();
	Format::analyze(getChild(node, "format"));
}

/*******************************************/
/* generate                                */
/*******************************************/
void Column::generate(QTextStream& out)
{
	//generateLeftBorder(out);
	if(getBrushStyle() >= 1)
	{
		out << ">{\\columncolor";
		generateColor(out);
		out << "}";
	}
	//generateRightBorder(out);
		
	out << "m{" << getWidth() << "pt}";
	
}



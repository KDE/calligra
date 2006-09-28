/*
** A program to convert the XML rendered by KSpread into LATEX.
**
** Copyright (C) 2003 Robert JACOLIN
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

#include <stdlib.h>

#include <kdebug.h>

//#include "fileheader.h"	/* for the use of _header (color and underlined) */
#include "pen.h"
//Added by qt3to4:
#include <QTextStream>

Pen::Pen()
{
}

/* Get the set of info about a text format */
void Pen::analyze(const QDomNode balise)
{
	/* <pen width="0" style="1" color="#000000" /> */
	setWidth(getAttr(balise, "width").toDouble());
	setStyle(getAttr(balise, "style").toInt());
	setColor(getAttr(balise, "color"));
}

void Pen::generate(QTextStream&)
{
	
}

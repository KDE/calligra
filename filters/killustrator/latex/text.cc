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

#include <kdebug.h>

#include "text.h"
#include "header.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Text::Text()
{
	setAlign(EA_LEFT);
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Text::~Text()
{
	kdDebug() << "Text Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Text::analyse(const QDomNode balise)
{
	Font *font;
	/* Get parameters */
	kdDebug() << "BEGIN OF ANALYSE OF A TEXT" << endl;

	Element::analyse(balise);
	analyseParam(balise);

	Element::analyseGObject(getChild(balise, "gobject"));
	font = new Font;
	font->analyse(getChild(balise, "font"));
	_fonts.append(font);
	_text =  getChild(balise, 1).toCharacterData().data();
	kdDebug() << "TEXT : " << _text << endl;
	kdDebug() << "END OF ANALYSE OF A TEXT" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Text::analyseParam(const QDomNode balise)
{
	/* Get parameters */
	setX(getAttr(balise, "x").toDouble());
	setY(getAttr(balise, "y").toDouble());
	setAlign(getAttr(balise, "align").toInt());
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Text::generatePSTRICKS(QTextStream& out)
{
	double x, y;

	out << "\\put";
	
	/* Options */
	QString param = getBaseContentAttr();
	QString params;

	concat(params, param);
	generateList(out, "[", params, "]");

	getMatrix().map(getX(), getY(), &x, &y);
	y = getFileHeader()->convert(y);
	/* Coord */
	out << "(" << x << "," << y << ")";

	/* text */
	out << "{" << _text << "}";
	out << endl;
}

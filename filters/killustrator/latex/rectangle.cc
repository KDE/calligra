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

#include "rectangle.h"
#include "header.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Rectangle::Rectangle()
{
	setRound(0);
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Rectangle::~Rectangle()
{
	kdDebug() << "Rectangle Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Rectangle::analyse(const QDomNode balise)
{
	kdDebug() << "BEGIN OF ANALYSE OF A RECTANGLE" << endl;

	Element::analyse(balise);
	analyseParam(balise);
	Element::analyseGObject(getChild(balise, "gobject"));
	kdDebug() << "END OF ANALYSE OF A RECTANGLE" << endl;
}

/*******************************************/
/* AnalyseParam                            */
/*******************************************/
void Rectangle::analyseParam(const QDomNode balise)
{
	/* Get parameters */
	setX(getAttr(balise, "x").toInt());
	setY(getAttr(balise, "y").toInt());
	setWidth(getAttr(balise, "width").toInt());
	setHeight(getAttr(balise, "height").toInt());
	setRound(getAttr(balise, "rounding").toInt());
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Rectangle::generatePSTRICKS(QTextStream& out)
{
	double x1, y1, x2, y2;

	kdDebug() << "Generate a rectangle" << endl;
	out << "\\psframe";

	/* Options */
	QString param = getBaseContentAttr();
	QString params;

	concat(params, param);
	if(getRound() != 0)
	{
		concat(params, QString("framearc=") + getRound());
	}
	
	generateList(out, "[", params, "]");

	getMatrix().map(getX(), getY(), &x1, &y1);
	getMatrix().map(getX() + getWidth(), getY() + getHeight(), &x2, &y2);
	y1 = getFileHeader()->convert(y1);
	y2 = getFileHeader()->convert(y2);
	out << "(" << x1 << "," << y1 << ")(";
	out << x2 << ",";
	out << y2 << ")" << endl;
	kdDebug() << "Rectangle generated" << endl;
}


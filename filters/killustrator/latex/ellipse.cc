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

#include <stdlib.h>		/* for atoi function    */

#include <kdebug.h>

#include "ellipse.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Ellipse::Ellipse()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Ellipse::~Ellipse()
{
	kdDebug() << "Ellipse Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Ellipse::analyse(const QDomNode balise)
{

	/* Get parameters */
	kdDebug() << "BEGIN OF ANALYSE OF AN ELLIPSE" << endl;

	Element::analyse(balise);
	setX(getAttr(balise, "x").toDouble());
	setY(getAttr(balise, "y").toDouble());
	setRx(getAttr(balise, "rx").toDouble());
	setRy(getAttr(balise, "ry").toDouble());
	setAngle1(getAttr(balise, "angle1").toDouble());
	setAngle2(getAttr(balise, "angle2").toDouble());
	if(getAttr(balise, "kind") == "full")
		setKind(EK_FULL);
	else if(getAttr(balise, "kind") == "arc")
		setKind(EK_ARC);
	else if(getAttr(balise, "kind") == "pie")
		setKind(EK_PIE);
	Element::analyseGObject(getChild(balise, "gobject"));
	
	kdDebug() << "END OF ANALYSE OF AN ELLIPSE" << endl;
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Ellipse::generatePSTRICKS(QTextStream& out)
{
	double x, y, rx, ry;
	kdDebug() << "Generate an ellipse" << endl;
	switch(getKind())
	{
		case EK_FULL:
			if(getRx() != getRy())
				out << "\\psellipse";
			else
				out << "\\pscircle";
			break;
		case EK_ARC:
			out << "\\psarc";
			break;
		case EK_PIE:
			out << "\\pswedge";
			break;
	}
	
	if(getFillStyle() == "FS_SOLID")
		out << "*";
	
	/* Options */
	QString param = getBaseContentAttr();
	QString params;

	concat(params, param);
	generateList(out, "[", params, "]");

	getMatrix().map(getX(), getY(), &x, &y);
	getMatrix().map(getX() + getRx(), getY() + getRy(), &rx, &ry);
	out << "(" << x << "," << y << ")";
	if(getKind() == EK_FULL && (rx -x) != (ry - y))
		out << "(" << (rx - x) << "," << (ry - y) << ")";
	else
		out << "{" << (rx - x) << "}";
	if(getKind() == EK_ARC || getKind() == EK_PIE)
		out << "{-" << getAngle1() << "}" << "{-" << getAngle2() << "}";
	out << endl;
	kdDebug() << "Ellipse generated" << endl;
}

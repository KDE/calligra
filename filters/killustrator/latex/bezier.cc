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

#include "bezier.h"
#include "header.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Bezier::Bezier()
{
	setArrow1(EF_NONE);
	setArrow2(EF_NONE);
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Bezier::~Bezier()
{
	kdDebug() << "Bezier Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Bezier::analyse(const QDomNode balise)
{

	/* Get parameters */
	kdDebug() << "BEGIN OF ANALYSE OF A BEZIER" << endl;
	//init(balise);
	Element::analyse(balise);
	analyseParam(balise);
	for(int index = 0; index < getNbChild(balise, "point"); index++)
	{
		Point *point = 0;
		point = new Point;
		point->analyse(getChild(balise, "point", index));
		_points.append(point);
	}
	Element::analyseGObject(getChild(balise, "gobject"));
	kdDebug() << "END OF ANALYSE OF A BEZIER" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Bezier::analyseParam(const QDomNode balise)
{
	setArrow1(getAttr(balise, "arrow1").toInt());
	setArrow2(getAttr(balise, "arrow2").toInt());
	setClosed(getAttr(balise, "closed").toInt());
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Bezier::generatePSTRICKS(QTextStream& out)
{
	double x, y;
	kdDebug() << "Generate a bezier" << endl;
	out << "\\psbezier";
	generatePSTRICKSParam(out);
	for(Point* point = _points.first(); point != 0; point = _points.next())
	{
		QString coord;
		getMatrix().map(point->getX(), point->getY(), &x, &y);
		y = getFileHeader()->convert(y);
		concat(coord, x);
		concat(coord, y);
		generateList(out, QString("("), coord, QString(")"));
	}
	out << endl;
	kdDebug() << "Bezier generated" << endl;
	
}

/*******************************************/
/* GeneratePSTRICKSParam                   */
/*******************************************/
void Bezier::generatePSTRICKSParam(QTextStream& out)
{
	bool toClose= false;
	QString param1 = getBaseContentAttr();
	QString params;
	QString arrows;

	concat(params, param1);

	if(!params.isEmpty())
		out << "[" << params << "]";
	if(getArrow1() != EF_NONE)
	{
		concat(arrows, "<");
	}

	if(getArrow2() != EF_NONE)
	{
		concat(arrows, ">");
	}
	generateList(out, "{", arrows, "}");
	/*if(!arrows.isEmpty())
		out << "{" << arrows << "}";*/
}


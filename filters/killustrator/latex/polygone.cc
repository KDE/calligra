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

#include "polygone.h"
#include "header.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Polygone::Polygone()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Polygone::~Polygone()
{
	kdDebug() << "Polygone Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Polygone::analyse(const QDomNode balise)
{
	/* Get parameters */
	kdDebug() << "BEGIN OF ANALYSE OF A POLYGONE" << endl;

	Element::analyse(balise);

	Polyline::analyse(getChild(balise, "polyline"));
	Element::analyseGObject(getChild(balise, "gobject"));
	kdDebug() << "END OF ANALYSE OF A POLYGONE" << endl;
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Polygone::generatePSTRICKS(QTextStream& out)
{
	double x, y;
	kdDebug() << "Generate a polygone" << endl;
	out << "\\pspolygon";
	/* options */
	QString param = getBaseContentAttr();
	QString params;

	concat(params, param);
	generateList(out, "[", params, "]");
	//generatePSTRICKSParam(out);
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
	kdDebug() << "Polygone generated" << endl;
}

/*******************************************/
/* GeneratePSTRICKSParam                   */
/*******************************************/
/*void Polyline::generatePSTRICKSParam(QTextStream& out)
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
}*/

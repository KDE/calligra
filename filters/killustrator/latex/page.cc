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

#include "page.h"
/*#include "rectangle.h"
#include "bezier.h"
#include "polyline.h"
#include "polygone.h"
#include "ellipse.h"
#include "text.h"
#include "group.h"*/

/*******************************************/
/* Constructor                             */
/*******************************************/
Page::Page()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Page::~Page()
{
	kdDebug() << "Page Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Page::analyse(const QDomNode balise)
{

	kdDebug() <<"BEGIN ANALYSE OF A PAGE" << endl;
	analyseLayout(getChild(balise, "layout"));
	_layer.analyse(getChild(balise, "layer"));

	kdDebug() << "END OF ANALYSE OF A PAGE" << endl;
}

void Page::analyseLayout(const QDomNode balise)
{
	/* Get parameters */
	setWidth(getAttr(balise, "width").toInt());
	setHeight(getAttr(balise, "height").toInt());
	setOrientation(getAttr(balise, "orientation").toInt());
	setLeftMargin(getAttr(balise, "lmargin").toInt());
	setRightMargin(getAttr(balise, "rmargin").toInt());
	setBottomMargin(getAttr(balise, "bmargin").toInt());
	setTopMargin(getAttr(balise, "tmargin").toInt());
	setFormat(getAttr(balise, "format").toInt());
	//setUnit(getAttr(balise, "unit").toInt();
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Page::generatePSTRICKS(QTextStream &out)
{
	//out << "\\psset{origin={0,-" << getHeight() << "mm}}" << endl;
	//out << "\\psset{yunit=-1pt}" << endl;
	_layer.generatePSTRICKS(out);
}


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

#include "font.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Font::Font()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Font::~Font()
{
	kdDebug() << "Font Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Font::analyse(const QDomNode balise)
{
	/* Get parameters */
	kdDebug() << "BEGIN OF ANALYSE OF A FONT" << endl;
	//init(balise);

	analyseParam(balise);

	kdDebug() << "END OF ANALYSE OF A FONT" << endl;
}

/*******************************************/
/* AnalyseParam                            */
/*******************************************/
void Font::analyseParam(const QDomNode balise)
{
	/* Get parameters */
	setFace(getAttr(balise, "face"));
	setPointSize(getAttr(balise, "point-size").toInt());
	setWeight(getAttr(balise, "weight").toInt());
}

/*******************************************/
/* generatePSTRICKS                        */
/*******************************************/
void Font::generatePSTRICKS(QTextStream& out)
{

}


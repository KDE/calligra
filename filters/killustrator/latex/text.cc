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
	/* Get parameters */
	kdDebug() << "BEGIN OF ANALYSE OF A TEXT" << endl;

	Element::analyse(balise);
	analyseParam(balise);

	for(int index= 0; index < getNbChild(balise); index++)
	{
		Font *font = 0;
		kdDebug() << getChildName(balise, index) << endl;
		if(getChildName(balise, index).compare("font")== 0)
		{
			font = new Font;
			font->analyse(balise);
			_fonts.append(font);
		}
		else if(getChild(balise, index).isCDATASection())
		{
			QDomCDATASection txt = getChild(balise, index).toCDATASection();
			kdDebug() << txt.data() << endl;
			_text = txt.data();
		}
		else if(getChildName(balise, index).compare("gobject")== 0)
		{
			Element::analyseGObject(getChild(balise, index));
		}
	}
	
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

	out << "\\rput";
	
	/* Options */
	QString param = getBaseContentAttr();
	QString params;

	concat(params, param);
	generateList(out, "[", params, "]");

	getMatrix().map(getX(), getY(), &x, &y);

	/* Coord */
	out << "(" << x << "," << y << ")";

	/* text */
	out << "{" << _text << "}";
	out << endl;
}

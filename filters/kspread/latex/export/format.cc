/*
** A program to convert the XML rendered by KWord into LATEX.
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
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#include <stdlib.h>

#include <kdebug.h>

//#include "fileheader.h"	/* for the use of _header (color and underlined) */
#include "format.h"

Format::Format()
{
	_multirow = -1;
}

Format::~Format()
{
	if(_bottomBorder != NULL)
		delete _bottomBorder;
	if(_topBorder != NULL)
		delete _topBorder;
	if(_leftBorder != NULL)
		delete _leftBorder;
	if(_rightBorder != NULL)
		delete _rightBorder;
}

/* Get the set of info. about a text format */
void Format::analyse(const QDomNode balise)
{
	/* <format brushstyle="5" brushcolor="#a70bc3" bgcolor="#ffffff" alignY="2" align="4" > */
	/*setId(getAttr(balise, "id").toInt());
	setPos(getAttr(balise, "pos").toInt());
	setLength(getAttr(balise, "len").toInt());*/
	setBrushStyle(getAttr(balise, "brushstyle").toInt());
	setBrushColor(getAttr(balise, "brushcolor"));
	setBgColor(getAttr(balise, "bgcolor"));
	setAlignY(getAttr(balise, "alignY").toLong());
	setAlign(getAttr(balise, "align").toLong());
	analysePen(getChild(balise, "pen"));
	if(isChild(balise, "bottom-border"))
	{
		kdDebug() << "bottom-border" << endl;
		_bottomBorder = new Pen();
		_bottomBorder->analyse(getChild(getChild(balise, "bottom-border"), "pen"));
	}
	if(isChild(balise, "top-border"))
	{
		kdDebug() << "top-border" << endl;
		_topBorder = new Pen();
		_topBorder->analyse(getChild(getChild(balise, "top-border"), "pen"));
	}
	if(isChild(balise, "left-border"))
	{
		kdDebug() << "left-border" << endl;
		_leftBorder = new Pen();
		_leftBorder->analyse(getChild(getChild(balise, "left-border"), "pen"));
	}
	if(isChild(balise, "right-border"))
	{
		kdDebug() << "right-border" << endl;
		_rightBorder = new Pen();
		_rightBorder->analyse(getChild(getChild(balise, "right-border"), "pen"));
	}
}

void Format::analysePen(const QDomNode balise)
{
	/* <pen width="0" style="1" color="#000000" /> */
	/*setWidth(getAttr(balise, "width").toDouble());
	setStyle(getAttr(balise, "style").toInt());
	setColor(getAttr(balise, "color"));*/
}

void Format::analyseFont(const QDomNode balise)
{
	/* <font size="18" family="Helvetica" weight="50" /> */
	setFontSize(getAttr(balise, "size").toInt());
	setFontFamily(getAttr(balise, "family"));
	setFontWeight(getAttr(balise, "weight").toInt());
}

void Format::generate(QTextStream&)
{
}

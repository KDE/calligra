/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2002, 2003 Robert JACOLIN
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

#include "column.h"
#include "fileheader.h"	/* for the use of _header (color and underlined) */
#include "format.h"
#include "row.h"
//Added by qt3to4:
#include <QTextStream>

Format::Format()
{
	_multirow = -1;
	_brushStyle = -1;
	_isValidFormat = false;
	_bottomBorder = NULL;
	_topBorder = NULL;
	_rightBorder = NULL;
	_leftBorder = NULL;
}

Format::~Format()
{
    delete _bottomBorder;
    delete _topBorder;
    delete _leftBorder;
    delete _rightBorder;
}

bool Format::hasTopBorder() const
{
	if(_topBorder == NULL)
		return false;
	else
		return (_topBorder->getStyle() > 0);
}

bool Format::hasBottomBorder() const
{
	if(_bottomBorder == NULL)
		return false;
	else
		return (_bottomBorder->getStyle() > 0);
}

bool Format::hasLeftBorder() const
{
	if(_leftBorder == NULL)
		return false;
	else
		return (_leftBorder->getStyle() > 0);
}

bool Format::hasRightBorder() const
{
	if(_rightBorder == NULL)
		return false;
	else
		return (_rightBorder->getStyle() > 0);
}

/* Get the set of info about a text format */
void Format::analyze(const QDomNode node)
{
	/* <format brushstyle="5" brushcolor="#a70bc3" bgcolor="#ffffff" alignY="2" align="4" > */
	if( !getAttr(node, "brushstyle").isEmpty() )
	{
		_isValidFormat = true;
		setBrushStyle(getAttr(node, "brushstyle").toInt());
		FileHeader::instance()->useColor();
		setBrushColor(getAttr(node, "brushcolor"));
		setBgColor(getAttr(node, "bgcolor"));
		setAlignY(getAttr(node, "alignY").toLong());
		setAlign(getAttr(node, "align").toLong());
	}
	if(isChild(node, "pen"))
		analyzePen(getChild(node, "pen"));
	if(isChild(node, "bottom-border"))
	{
		kDebug(30522) << "bottom-border" << endl;
		_isValidFormat = true;
		_bottomBorder = new Pen();
		_bottomBorder->analyze(getChild(getChild(node, "bottom-border"), "pen"));
	}
	if(isChild(node, "top-border"))
	{
		kDebug(30522) << "top-border" << endl;
		_isValidFormat = true;
		_topBorder = new Pen();
		_topBorder->analyze(getChild(getChild(node, "top-border"), "pen"));
	}
	if(isChild(node, "left-border"))
	{
		kDebug(30522) << "left-border" << endl;
		_isValidFormat = true;
		_leftBorder = new Pen();
		_leftBorder->analyze(getChild(getChild(node, "left-border"), "pen"));
	}
	if(isChild(node, "right-border"))
	{
		kDebug(30522) << "right-border" << endl;
		_isValidFormat = true;
		_rightBorder = new Pen();
		_rightBorder->analyze(getChild(getChild(node, "right-border"), "pen"));
	}
}

void Format::analyzePen(const QDomNode node)
{
	/* <pen width="0" style="1" color="#000000" /> */
	_isValidFormat = true;
	setPenWidth(getAttr(node, "width").toDouble());
	setPenStyle(getAttr(node, "style").toInt());
	setPenColor(getAttr(node, "color"));
}

void Format::analyzeFont(const QDomNode node)
{
	/* <font size="18" family="Helvetica" weight="50" /> */
	setFontSize(getAttr(node, "size").toInt());
	setFontFamily(getAttr(node, "family"));
	setFontWeight(getAttr(node, "weight").toInt());
}

void Format::generate(QTextStream& out, Column* col, Row* row)
{
	if(hasLeftBorder())
		out << "|";
	if(isValidFormat() && getBrushStyle() >= 1)
	{
		out << ">{\\columncolor";
		generateColor(out);
		out << "}";
	}
	else if(col != NULL)
	{
		if(col->getBrushStyle() >= 1)
		{
			out << ">{\\columncolor";
			col->generateColor(out);
			out << "}";
		}
	}
	else if(row != NULL)
	{
		if(row->getBrushStyle() >= 1)
		{
			out << ">{\\columncolor";
			row->generateColor(out);
			out << "}";
		}
	}
        if ( col != NULL )
            out << "m{" << col->getWidth() << "pt}";
	if(hasRightBorder())
		out << "|";
}

void Format::generateTextFormat(QTextStream& out, QString text)
{
	if(getPenStyle() > 0)
	{
		float red   = ((float) getPenColor().red()) / 255;
		float green = ((float) getPenColor().green()) / 255;
		float blue  = ((float) getPenColor().blue()) / 255;

		out << "\\textcolor[rgb]{"<< red << ", " << green <<
				", " << blue << "}{" << text << "}" << endl;
	}
}

void Format::generateColor(QTextStream& out)
{
	if(getBrushStyle() >= 1)
	{
		float red   = ((float) getBrushColor().red()) / 255;
		float green = ((float) getBrushColor().green()) / 255;
		float blue  = ((float) getBrushColor().blue()) / 255;

		out << "[rgb]{" << red << ", " << green <<
				", " << blue << "}%" << endl;
	}
}

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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include <stdlib.h>

#include <kdebug.h>

#include "fileheader.h"		/* for the use of _fileHeader (color and underlined) */
#include "variableformat.h"

/*******************************************/
/* getColorBlue                            */
/*******************************************/
/*int VariableFormat::getColorBlue () const
{
	if(_textcolor!= 0)
		return _textcolor->blue();
	else
		return 0;
}*/

/*******************************************/
/* getColorGreen                           */
/*******************************************/
/*int VariableFormat::getColorGreen() const
{
	if(_textcolor!= 0)
		return _textcolor->green();
	else
		return 0;
}*/

/*******************************************/
/* getColorRed                             */
/*******************************************/
/*int VariableFormat::getColorRed  () const
{
	if(_textcolor!= 0)
		return _textcolor->red();
	else
		return 0;
}*/

/*******************************************/
/* setColor                                */
/*******************************************/
/*void VariableFormat::setColor (const int r, const int g, const int b)
{
	if(_textcolor == 0)
		_textcolor = new QColor(r, g, b);
	else
		_textcolor->setRgb(r, g, b);
}*/

/*******************************************/
/* analyzeVariableFormat                       */
/*******************************************/
/* Get the set of info. about a text format*/
/*******************************************/
void VariableFormat::analyzeFormat(const QDomNode node)
{
	/* Markup <FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* Parameter analysis */
	analyzeParam(node);
	kDebug(30522) << "ANALYZE A FORMAT" << endl;
	
	/* Child markup analysis */
	if(isChild(node, "FONT"))
		analyzeFont(getChild(node, "FONT"));
	if(isChild(node, "ITALIC"))
		analyzeItalic(getChild(node, "ITALIC"));
	if(isChild(node, "UNDERLINE"))
		analyzeUnderlined(getChild(node, "UNDERLINE"));
	if(isChild(node, "WEIGHT"))
		analyzeWeight(getChild(node, "WEIGHT"));
	if(isChild(node, "VERTALIGN"))
		analyzeAlign(getChild(node, "VERTALIGN"));
	if(isChild(node, "STRIKEOUT"))
		analyzeStrikeout(getChild(node, "STRIKEOUT"));
	if(isChild(node, "COLOR"))
		analyzeColor(getChild(node, "COLOR"));
	if(isChild(node, "SIZE"))
		analyzeSize(getChild(node, "SIZE"));
	if(isChild(node, "DATE"))
		analyzeDate(getChild(node, "DATE"));
	if(isChild(node, "FOOTNOTE"))
		analyzeFootnote(getChild(node, "FOOTNOTE"));
	if(isChild(node, "NOTE"))
		analyzeNote(getChild(node, "NOTE"));
	if(isChild(node, "TYPE"))
		analyzeType(getChild(node, "TYPE"));
	kDebug(30522) << "END OF A FORMAT" << endl;
}

/*******************************************/
/* analyzeDate                             */
/*******************************************/
/* Get the date.                           */
/*******************************************/
void VariableFormat::analyzeDate(const QDomNode node)
{
	setDay(getAttr(node, "day").toInt());
	setMonth(getAttr(node, "month").toInt());
	setYear(getAttr(node, "year").toInt());
	setFix(getAttr(node, "fix").toInt());
}

/*******************************************/
/* analyzeTime                             */
/*******************************************/
/* Get the time.                           */
/*******************************************/
void VariableFormat::analyzeTime(const QDomNode node)
{
	setHour(getAttr(node, "day").toInt());
	setMinute(getAttr(node, "month").toInt());
	setSeconde(getAttr(node, "year").toInt());
	setFix(getAttr(node, "fix").toInt());
}

void VariableFormat::analyzeFootnote(const QDomNode node)
{
	setNumberingtype(getAttr(node, "numberingtype"));
	setNotetype(getAttr(node, "notetype"));
	setFrameset(getAttr(node, "frameset"));
	setValue(getAttr(node, "value"));
}

void VariableFormat::analyzeNote(const QDomNode node)
{
	setNote(getAttr(node, "note"));
}

/*******************************************/
/* analyzeType                             */
/*******************************************/
/* Get information about variable.         */
/*******************************************/
void VariableFormat::analyzeType(const QDomNode node)
{
	setKey(getAttr(node, "key"));
	setType(getAttr(node, "type").toInt());
	setText(getAttr(node, "text"));
}


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
void VariableFormat::analyzeFormat(const QDomNode balise)
{
	/* MARKUP FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* Parameter Analysis */
	analyzeParam(balise);
	kDebug(30522) << "ANALYZE A FORMAT" << endl;
	
	/* Child markup analysis */
	if(isChild(balise, "FONT"))
		analyzeFont(getChild(balise, "FONT"));
	if(isChild(balise, "ITALIC"))
		analyzeItalic(getChild(balise, "ITALIC"));
	if(isChild(balise, "UNDERLINE"))
		analyzeUnderlined(getChild(balise, "UNDERLINE"));
	if(isChild(balise, "WEIGHT"))
		analyzeWeight(getChild(balise, "WEIGHT"));
	if(isChild(balise, "VERTALIGN"))
		analyzeAlign(getChild(balise, "VERTALIGN"));
	if(isChild(balise, "STRIKEOUT"))
		analyzeStrikeout(getChild(balise, "STRIKEOUT"));
	if(isChild(balise, "COLOR"))
		analyzeColor(getChild(balise, "COLOR"));
	if(isChild(balise, "SIZE"))
		analyzeSize(getChild(balise, "SIZE"));
	if(isChild(balise, "DATE"))
		analyzeDate(getChild(balise, "DATE"));
	if(isChild(balise, "FOOTNOTE"))
		analyzeFootnote(getChild(balise, "FOOTNOTE"));
	if(isChild(balise, "NOTE"))
		analyzeNote(getChild(balise, "NOTE"));
	if(isChild(balise, "TYPE"))
		analyzeType(getChild(balise, "TYPE"));
	kDebug(30522) << "END OF A FORMAT" << endl;
}

/*******************************************/
/* analyzeDate                             */
/*******************************************/
/* Get the date.                           */
/*******************************************/
void VariableFormat::analyzeDate(const QDomNode balise)
{
	setDay(getAttr(balise, "day").toInt());
	setMonth(getAttr(balise, "month").toInt());
	setYear(getAttr(balise, "year").toInt());
	setFix(getAttr(balise, "fix").toInt());
}

/*******************************************/
/* analyzeTime                             */
/*******************************************/
/* Get the time.                           */
/*******************************************/
void VariableFormat::analyzeTime(const QDomNode balise)
{
	setHour(getAttr(balise, "day").toInt());
	setMinute(getAttr(balise, "month").toInt());
	setSeconde(getAttr(balise, "year").toInt());
	setFix(getAttr(balise, "fix").toInt());
}

void VariableFormat::analyzeFootnote(const QDomNode balise)
{
	setNumberingtype(getAttr(balise, "numberingtype"));
	setNotetype(getAttr(balise, "notetype"));
	setFrameset(getAttr(balise, "frameset"));
	setValue(getAttr(balise, "value"));
}

void VariableFormat::analyzeNote(const QDomNode balise)
{
	setNote(getAttr(balise, "note"));
}

/*******************************************/
/* analyzeType                             */
/*******************************************/
/* Get information about variable.         */
/*******************************************/
void VariableFormat::analyzeType(const QDomNode balise)
{
	setKey(getAttr(balise, "key"));
	setType(getAttr(balise, "type").toInt());
	setText(getAttr(balise, "text"));
}


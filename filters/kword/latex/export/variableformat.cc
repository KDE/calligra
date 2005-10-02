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
/* analyseVariableFormat                       */
/*******************************************/
/* Get the set of info. about a text format*/
/*******************************************/
void VariableFormat::analyseFormat(const QDomNode balise)
{
	/* MARKUPS FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* Parameters Analyse */
	analyseParam(balise);
	kdDebug(30522) << "ANALYSE A FORMAT" << endl;
	
	/* Children Markups Analyse */
	if(isChild(balise, "FONT"))
		analyseFont(getChild(balise, "FONT"));
	if(isChild(balise, "ITALIC"))
		analyseItalic(getChild(balise, "ITALIC"));
	if(isChild(balise, "UNDERLINE"))
		analyseUnderlined(getChild(balise, "UNDERLINE"));
	if(isChild(balise, "WEIGHT"))
		analyseWeight(getChild(balise, "WEIGHT"));
	if(isChild(balise, "VERTALIGN"))
		analyseAlign(getChild(balise, "VERTALIGN"));
	if(isChild(balise, "STRIKEOUT"))
		analyseStrikeout(getChild(balise, "STRIKEOUT"));
	if(isChild(balise, "COLOR"))
		analyseColor(getChild(balise, "COLOR"));
	if(isChild(balise, "SIZE"))
		analyseSize(getChild(balise, "SIZE"));
	if(isChild(balise, "DATE"))
		analyseDate(getChild(balise, "DATE"));
	if(isChild(balise, "FOOTNOTE"))
		analyseFootnote(getChild(balise, "FOOTNOTE"));
	if(isChild(balise, "NOTE"))
		analyseNote(getChild(balise, "NOTE"));
	if(isChild(balise, "TYPE"))
		analyseType(getChild(balise, "TYPE"));
	kdDebug(30522) << "END OF A FORMAT" << endl;
}

/*******************************************/
/* analyseDate                             */
/*******************************************/
/* Get the date.                           */
/*******************************************/
void VariableFormat::analyseDate(const QDomNode balise)
{
	setDay(getAttr(balise, "day").toInt());
	setMonth(getAttr(balise, "month").toInt());
	setYear(getAttr(balise, "year").toInt());
	setFix(getAttr(balise, "fix").toInt());
}

/*******************************************/
/* analyseTime                             */
/*******************************************/
/* Get the time.                           */
/*******************************************/
void VariableFormat::analyseTime(const QDomNode balise)
{
	setHour(getAttr(balise, "day").toInt());
	setMinute(getAttr(balise, "month").toInt());
	setSeconde(getAttr(balise, "year").toInt());
	setFix(getAttr(balise, "fix").toInt());
}

void VariableFormat::analyseFootnote(const QDomNode balise)
{
	setNumberingtype(getAttr(balise, "numberingtype"));
	setNotetype(getAttr(balise, "notetype"));
	setFrameset(getAttr(balise, "frameset"));
	setValue(getAttr(balise, "value"));
}

void VariableFormat::analyseNote(const QDomNode balise)
{
	setNote(getAttr(balise, "note"));
}

/*******************************************/
/* analyseType                             */
/*******************************************/
/* Get information about variable.         */
/*******************************************/
void VariableFormat::analyseType(const QDomNode balise)
{
	setKey(getAttr(balise, "key"));
	setType(getAttr(balise, "type").toInt());
	setText(getAttr(balise, "text"));
}


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
void VariableFormat::analyseVariableFormat(const QDomNode balise)
{
	/* MARKUPS FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* Parameters Analyse */
	analyseParam(balise);
	kdDebug() << "ANALYSE A FORMAT" << endl;
	
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
	if(isChild(balise, "TYPE"))
		analyseType(getChild(balise, "TYPE"));
	kdDebug() << "END OF A FORMAT" << endl;
}

/*******************************************/
/* analyseParam                            */
/*******************************************/
/* Get the zone where the format is applied*/
/*******************************************/
/*void VariableFormat::analyseParam(const QDomNode balise)
{
	/ <FORMAT id="1" pos="0" len="17"> /

	//setId(getAttr(balise, "id").toInt());
	//setPos(getAttr(balise, "pos").toInt());
	//setLength(getAttr(balise, "len").toInt());
	Format::analyse(balise);
}*/

/*******************************************/
/* analyseFont                             */
/*******************************************/
/* Get the text font!                      */
/*******************************************/
/*void VariableFormat::analyseFont(const QDomNode balise)
{
	/ <FONT name="times"> /
	setPolice(getAttr(balise, "name"));
}*/

/*******************************************/
/* analyseItalic                           */
/*******************************************/
/* Verify if it's a italic text.           */
/*******************************************/
/*void VariableFormat::analyseItalic(const QDomNode balise)
{
	/ <ITALIC value="1"> /
	setItalic(getAttr(balise, "value").toInt());
}*/

/*******************************************/
/* analyseUnderlined                       */
/*******************************************/
/* Verify if it's a underlined text.       */
/*******************************************/
/*void VariableFormat::analyseUnderlined(const QDomNode balise)
{
	/ <UNDERLINE value="1"> /

	setUnderlined(getAttr(balise, "value").toInt());
	if(isUnderlined())
		_fileHeader->useUnderline();
	kdDebug() << "Underlined ? " << isUnderlined() << endl;
}*/

/*******************************************/
/* analyseStrikeout                        */
/*******************************************/
/* Verify if it's a strikeout text.        */
/*******************************************/
/*void VariableFormat::analyseStrikeout(const QDomNode balise)
{
	/ <STRIKEOUT value="1" /> /
	setStrikeout(getAttr(balise, "value").toInt());
	if(isStrikeout())
		_fileHeader->useUnderline();
	kdDebug() << "Strikeout ? " << isUnderlined() << endl;
}*/

/*******************************************/
/* analyseWeigth                           */
/*******************************************/
/* Get the text weigth.                    */
/*******************************************/
/*void VariableFormat::analyseWeight(const QDomNode balise)
{
	/ <WEIGHT value="75" /> /
	setWeight(getAttr(balise, "value").toInt());
}*/

/*******************************************/
/* analyseAlign                            */
/*******************************************/
/* Get the text align.                     */
/*******************************************/
/*void VariableFormat::analyseAlign(const QDomNode balise)
{
	/ <VERTALIGN value="0"> /

	setAlign(getAttr(balise, "value").toInt());
}*/

/*******************************************/
/* analyseColor                            */
/*******************************************/
/* Get the text color.                     */
/*******************************************/
/*void VariableFormat::analyseColor(const QDomNode balise)
{
	/ <COLOR red="0" green="0" blue="0"> /
	int  red   = 0, 
	     blue  = 0,
	     green = 0;

	red = getAttr(balise, "red").toInt();
	green = getAttr(balise, "green").toInt();
	blue = getAttr(balise, "blue").toInt();

	if(!(red == green == blue == 0))
	{
		/ black color is default value /
		setColor(red, green, blue);
		_fileHeader->useColor();
	}
}*/

/*******************************************/
/* analyseSize                             */
/*******************************************/
/* Get the text size.                      */
/*******************************************/
/*void VariableFormat::analyseSize(const QDomNode balise)
{
	/ <SIZE value="11"> /
	setSize(getAttr(balise, "value").toInt());
}*/

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


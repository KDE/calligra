/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000, 2001, 2002 Robert JACOLIN
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

#include "fileheader.h"		/* for the use of FileHeader::instance() (color and underlined) */
#include "textformat.h"

/*******************************************/
/* getColorXXXX                            */
/*******************************************/
int TextFormat::getColorBlue () const
{
	if(_textcolor!= 0)
		return _textcolor->blue();
	else
		return 0;
}

int TextFormat::getColorGreen() const
{
	if(_textcolor!= 0)
		return _textcolor->green();
	else
		return 0;
}

int TextFormat::getColorRed  () const
{
	if(_textcolor!= 0)
		return _textcolor->red();
	else
		return 0;
}

/*******************************************/
/* getBkColorXXX                           */
/*******************************************/
int TextFormat::getBkColorBlue () const
{
	if(_backcolor!= 0)
		return _backcolor->blue();
	else
		return 0;
}

int TextFormat::getBkColorGreen() const
{
	if(_backcolor!= 0)
		return _backcolor->green();
	else
		return 0;
}

int TextFormat::getBkColorRed  () const
{
	if(_backcolor!= 0)
		return _backcolor->red();
	else
		return 0;
}

/*******************************************/
/* setColor                                */
/*******************************************/
void TextFormat::setColor (const int r, const int g, const int b)
{
	if(_textcolor == 0)
		_textcolor = new QColor(r, g, b);
	else
		_textcolor->setRgb(r, g, b);
}

/*******************************************/
/* setBkColor                              */
/*******************************************/
void TextFormat::setBkColor (const int r, const int g, const int b)
{
	if(_backcolor == 0)
		_backcolor = new QColor(r, g, b);
	else
		_backcolor->setRgb(r, g, b);
}

/*******************************************/
/* analyseTextFormat                       */
/*******************************************/
/* Get the set of info. about a text format*/
/*******************************************/
void TextFormat::analyseFormat(const QDomNode balise)
{
	/* MARKUPS FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* Parameters Analyse */
	analyseParam(balise);
	kDebug(30522) << "ANALYSE A FORMAT" << endl;
	
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
	if(isChild(balise, "TEXTBACKGROUNDCOLOR"))
		analyseBackgroundColor(getChild(balise, "TEXTBACKGROUNDCOLOR"));

	kDebug(30522) << "END OF A FORMAT" << endl;
}

void TextFormat::analyseBackgroundColor(const QDomNode balise)
{
	/* <TEXTBACKGROUNDCOLOR red="0" green="0" blue="0"/> */
	int  red   = 0, 
	     blue  = 0,
	     green = 0;

	red = getAttr(balise, "red").toInt();
	green = getAttr(balise, "green").toInt();
	blue = getAttr(balise, "blue").toInt();

	if(!(red == 255 && green == 255 && blue == 255))
	{
		kDebug(30522) << "bk color = " << red << "," << green << "," << blue << endl;
		/* white color is default value */
		setBkColor(red, green, blue);
		FileHeader::instance()->useColor();
	}
}

/*******************************************/
/* analyseParam                            */
/*******************************************/
/* Get the zone where the format is applied*/
/*******************************************/
void TextFormat::analyseParam(const QDomNode balise)
{
	/* <FORMAT id="1" pos="0" len="17"> */

	//setId(getAttr(balise, "id").toInt());
	//setPos(getAttr(balise, "pos").toInt());
	//setLength(getAttr(balise, "len").toInt());
	Format::analyse(balise);
}

/*******************************************/
/* analyseFont                             */
/*******************************************/
/* Get the text font!                      */
/*******************************************/
void TextFormat::analyseFont(const QDomNode balise)
{
	/* <FONT name="times"> */
	setPolice(getAttr(balise, "name"));
}

/*******************************************/
/* analyseItalic                           */
/*******************************************/
/* Verify if it's a italic text.           */
/*******************************************/
void TextFormat::analyseItalic(const QDomNode balise)
{
	/* <ITALIC value="1"> */
	setItalic(getAttr(balise, "value").toInt());
}

/*******************************************/
/* analyseUnderlined                       */
/*******************************************/
/* Verify if it's a underlined text.       */
/*******************************************/
void TextFormat::analyseUnderlined(const QDomNode balise)
{
	/* <UNDERLINE value="1"> */

	setUnderlined(getAttr(balise, "value"));
	if(isUnderlined())
		FileHeader::instance()->useUnderline();
	kDebug(30522) << "Underlined ? " << isUnderlined() << endl;
}

/*******************************************/
/* analyseStrikeout                        */
/*******************************************/
/* Verify if it's a strikeout text.        */
/*******************************************/
void TextFormat::analyseStrikeout(const QDomNode balise)
{
	/* <STRIKEOUT value="1" /> */
	setStrikeout(getAttr(balise, "value").toInt());
	if(isStrikeout())
		FileHeader::instance()->useUnderline();
	kDebug(30522) << "Strikeout ? " << isUnderlined() << endl;
}

/*******************************************/
/* analyseWeight                           */
/*******************************************/
/* Get the text weight.                    */
/*******************************************/
void TextFormat::analyseWeight(const QDomNode balise)
{
	/* <WEIGHT value="75" /> */
	setWeight(getAttr(balise, "value").toInt());
	kDebug(30522) << "Weight = " << getWeight() << endl;
}

/*******************************************/
/* analyseAlign                            */
/*******************************************/
/* Get the text align.                     */
/*******************************************/
void TextFormat::analyseAlign(const QDomNode balise)
{
	/* <VERTALIGN value="0"> */

	setAlign(getAttr(balise, "value").toInt());
}

/*******************************************/
/* analyseColor                            */
/*******************************************/
/* Get the text color.                     */
/*******************************************/
void TextFormat::analyseColor(const QDomNode balise)
{
	/* <COLOR red="0" green="0" blue="0"/> */
	int  red   = 0, 
	     blue  = 0,
	     green = 0;

	red = getAttr(balise, "red").toInt();
	green = getAttr(balise, "green").toInt();
	blue = getAttr(balise, "blue").toInt();

	if(!(red == 0 && green == 0 && blue == 0))
	{
		/* black color is default value */
		kDebug(30522) << "color = " << red << "," << green << "," << blue << endl;
		setColor(red, green, blue);
		FileHeader::instance()->useColor();
	}
}

/*******************************************/
/* analyseSize                             */
/*******************************************/
/* Get the text size.                      */
/*******************************************/
void TextFormat::analyseSize(const QDomNode balise)
{
	/* <SIZE value="11"> */
	setSize(getAttr(balise, "value").toInt());
	kDebug(30522) << "font size : " << getSize() << endl;
}

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
#include "textformat.h"

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

void TextFormat::setColor (const int r, const int g, const int b)
{
	if(_textcolor == 0)
		_textcolor = new QColor(r, g, b);
	else
		_textcolor->setRgb(r, g, b);
}

/* Get the set of info. about a text format */
void TextFormat::analyseTextFormat(const Markup * balise_initiale)
{
	Token*  savedToken = 0;
	Markup* balise     = 0;

	// MARKUPS FORMAT id="1" pos="0" len="17">...</FORMAT>
	
	// Parameters Analyse
	analyseParam(balise_initiale);
	kdDebug() << "ANALYSE A ZONE" << endl;
	
	// Children Markups Analyse
	savedToken = enterTokenChild(balise_initiale);
	
	while((balise = getNextMarkup()) != NULL)
	{
		if(strcmp(balise->token.zText, "FONT")== 0)
		{
			kdDebug() << "FONT : " << endl;
			analyseFont(balise);
		}
		else if(strcmp(balise->token.zText, "ITALIC")== 0)
		{
			kdDebug() << "ITALIC : " << endl;
			analyseItalic(balise);
		}
		else if(strcmp(balise->token.zText, "UNDERLINE")== 0)
		{
			kdDebug() << "UNDERLINED : " << endl;
			analyseUnderlined(balise);
		}
		else if(strcmp(balise->token.zText, "WEIGHT")== 0)
		{
			kdDebug() << "WEIGTH : " << endl;
			analyseWeigth(balise);
		}
		else if(strcmp(balise->token.zText, "VERTALIGN")== 0)
		{
			kdDebug() << "VERTALIGN : " << endl;
			analyseAlign(balise);
		}
		else if(strcmp(balise->token.zText, "COLOR")== 0)
		{
			kdDebug() << "COLOR : " << endl;
			analyseColor(balise);
		}
	}
	kdDebug() << "END OF A ZONE" << endl;
}

/* Get the zone where the format is applied */
void TextFormat::analyseParam(const Markup *balise)
{
	//<FORMAT id="1" pos="0" len="17">
	Arg *arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "ID")== 0)
		{
			//setId(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "POS")== 0)
		{
			setPos(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "LEN")== 0)
		{
			setTaille (atoi(arg->zValue));
		}
	}
}

/* Get the text font! */
void TextFormat::analyseFont(const Markup *balise)
{
	//<FONT name="times">
	Arg *arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		//kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "NAME")== 0)
		{
			kdDebug() << arg->zName << endl;
			setPolice(arg->zValue);
		}
	}
}

/* Verift if it's a italic text */
void TextFormat::analyseItalic(const Markup *balise)
{
	//<FONT name="times">
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		//kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "VALUE")== 0)
		{
			kdDebug() << arg->zName << endl;
			setItalic(arg->zValue);
		}
	}
}

/* Verift if it's a underlined text */
void TextFormat::analyseUnderlined(const Markup *balise)
{
	//<FONT name="times">
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		//kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "VALUE")== 0)
		{
			kdDebug() << arg->zName << endl;
			setUnderlined(arg->zValue);
			_fileHeader->useUnderline();
		}
	}
}

/* Get the text weigth */
void TextFormat::analyseWeigth(const Markup *balise)
{
	//<FONT name="times">
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		//kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "VALUE")== 0)
		{
			kdDebug() << arg->zName << endl;
			setWeight(atoi(arg->zValue));
		}
	}
}

/* Get the text align */
void TextFormat::analyseAlign(const Markup *balise)
{
	//<VERTALIGN name="0">
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		//kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "VALUE")== 0)
		{
			kdDebug() << arg->zName << endl;
			setAlign(atoi(arg->zValue));
		}
	}
}

/* Get the text color */
void TextFormat::analyseColor(const Markup *balise)
{
	//<COLOR red="0" green="0" blue="0">
	Arg* arg   = 0;
	int  red   = 0, 
	     blue  = 0,
	     green = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		//kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "RED")== 0)
		{
			kdDebug() << arg->zName << endl;
			red = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "GREEN")== 0)
		{
			kdDebug() << arg->zName << endl;
			green = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "BLUE")== 0)
		{
			kdDebug() << arg->zName << endl;
			blue = atoi(arg->zValue);
		}
	}
	setColor(red, green, blue);
	_fileHeader->useColor();
}

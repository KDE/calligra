/* BUGS : If it's a title, I have a format command : test
 * the parag. style 
 */

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

#include <kdebug.h>		/* for kdDebug() stream */

#include "textzone.h"

TextZone::TextZone(): _useformat(true)
{

}

TextZone::TextZone(QString texte): _texte(texte), _useformat(true)
{
}

TextZone::~TextZone()
{
	kdDebug() << "Destruction of a area" << endl;
}

void TextZone::analyse(const Markup * balise_initiale)
{
	kdDebug() << "FORMAT" << endl;
	// Get infos. to format the text
	analyseFormat(balise_initiale);
	
	// Format the text
	_texte = _texte.mid(getPos(), getLength());
	
	kdDebug() << _texte.length() << endl;
	kdDebug() << _texte.local8Bit() << endl;
	kdDebug() << "END FORMAT" << endl;
}

void TextZone::generate(QTextStream &out)
{
	kdDebug() << "." << endl;

	if(_useformat)
		generate_format_begin(out);

	// Text
	out << _texte.latin1();
	// Text
	if(_useformat)
		generate_format_end(out);
}

void TextZone::generate_format_begin(QTextStream & out)
{
	kdDebug() << "GENERATE FORMAT" << endl;
	// Bold, Italic or underlined
	if(getWeight() > 0)
		out << " \\textbf{";
	if(isItalic())
		out << " \\textit{";
	if(isUnderlined())
		out << " \\uline{";

	// Size
	if(getSize() != 11)
	{
		out << "\\fontsize{" << getLength() << "}{1}%" << endl;
		out << "\\selectfont" << endl;
	}

	// Color
	if(isColor())
	{
		out << "\\textcolor{rgb}{";
		out << getColorRed() << ", " << getColorGreen() << ", ";
		out << getColorBlue() << "}{";
	}

	// Alignement
	switch(getAlign())
	{
		case EA_NONE:
			break;
		case EA_SUB: // pass in math mode !!
			out << " $_{";
			break;
		case EA_SUPER:
			out << " \\textsuperscript{";
			break;
	}
}

void TextZone::generate_format_end(QTextStream & out)
{
	// Alignement
	if(getAlign() == EA_SUPER)
		out << "}";
	if(getAlign() == EA_SUB)
		out << "}$";

	// Color
	if(isColor())
		out << "}";

	// Size
	if(getSize() != 11)
	{
		out << "\\fontsize{11}{1}%" << endl;
		out << "\\selectfont" << endl;
	}

	// Bold, Italic or underlined
	if(isUnderlined())
		out << "}";
	if(isItalic())
		out << "}";
	if(getWeight() > 0)
		out << "}";
}

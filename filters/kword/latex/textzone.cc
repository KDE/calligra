/* MEMO: to see the unicode table
 * xset +fp /usr/X11R6/lib/X11/fonts/ucs/
 * xfd -fn '-misc-fixed-medium-r-semicondensed--13-120-75-75-c-60-iso10646-1'
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
#include <qregexp.h>		/* for QRegExp() --> escapeLatin1 */

#include "textzone.h"
#include "para.h"

#define CSTART 0xC0

/*******************************************/
/* TextZone                                */
/*******************************************/
TextZone::TextZone(Para *para)
{
	setPara(para);
}

/*******************************************/
/* TextZone                                */
/*******************************************/
TextZone::TextZone(QString texte, Para *para): _texte(texte)
{
	setPara(para);
}

/*******************************************/
/* ~TextZone                               */
/*******************************************/
TextZone::~TextZone()
{
	kdDebug() << "Destruction of a area" << endl;
}

/*******************************************/
/* useFormat                               */
/*******************************************/
/* Use the format only if teh user wants   */
/* that and it's not a title.              */
/*******************************************/
bool TextZone::useFormat() const
{
	return !getPara()->isChapter();
}

/*******************************************/
/* escapeLatin1                            */
/*******************************************/
/* Convert special caracters (unicode) in  */
/* latex usable caracters.                 */
/*******************************************/
QString TextZone::escapeLatin1(QString text)
{
	static const char *escapes[64] =
	{
		"\\`{A}", "\\'{A}", "\\^{A}", "\\~{A}",
		"\\\"{A}", "\\AA", "\\AE", "\\c{C}",
		"\\`{E}", "\\'{E}", "\\^{E}", "\\\"{E}",
		"\\`{I}", "\\'{I}", "\\^{I}", "\\\"{I}",

		"", "\\~{N}", "\\`{O}", "\\'{O}",
		"\\^{O}", "\\~{O}", "\\\"{O}", "",
		"\\O", "\\`{U}", "\\'{U}", "\\^{U}",
		"\\\"{U}", "\\'{Y}", "", "\\ss",

		"\\`{a}", "\\'{a}", "\\^{a}", "\\~{a}",
		"\\\"{a}", "\\aa", "\\ae", "\\c{c}",
		"\\`{e}", "\\'{e}", "\\^{e}", "\\\"{e}",
		"\\`{\\i}", "\\'{\\i}", "\\^{\\i}", "\\\"{\\i}",

		"", "\\~{n}", "\\`{o}", "\\'{o}",
		"\\^{o}", "\\~{o}", "\\\"{o}", "",
		"\\o", "\\`{u}", "\\'{u}", "\\^{u}",
		"\\\"{u}", "\\'{y}", "", "\\\"{y}"
	};

	QString escapedText;
	int unicode;         /* the character to be escaped */

	escapedText = text;  /* copy input text */
	
	/***************************************************************************
	 * Escape the special punctuation and other symbols in the Latin1 supplement
	****************************************************************************/
	convert(escapedText, 0X23, "\\#");
	//convert(escapedText, 0X24, "\\$");
	convert(escapedText, 0X25, "\\%");
	convert(escapedText, 0X26, "\\&");
	
	convert(escapedText, 0X3C, "\\textless");
	convert(escapedText, 0X3E, "\\textgreater");

	//convert(escapedText, 0X5C, "\\textbackslash");
	//convert(escapedText, 0X5E, "\\^");
	convert(escapedText, 0X5F, "\\_");
	
	convert(escapedText, 0X7B, "\\{");
	convert(escapedText, 0X7D, "\\}");
	convert(escapedText, 0X7E, "\\~");
	
	convert(escapedText, 0XA1, "!`");
	convert(escapedText, 0XA3, "\\pounds");
	convert(escapedText, 0XA6, "\\textbar");
	convert(escapedText, 0XA7, "\\S");
	convert(escapedText, 0XA9, "\\copyright");
	convert(escapedText, 0XAE, "\\textregistered");
	convert(escapedText, 0XB6, "\\P");
	convert(escapedText, 0XBF, "?`");
	
	/* begin making escape sequences for the 64 consecutive letters starting at C0
	 * LaTeX has a different escape code when a char is followed by a space so
	 * two escape sequences are needed for each character.
	 */

	for(int index = 0; index < 64; index++)
	{
		unicode = CSTART + index;
		convert(escapedText, unicode, escapes[index]);
	}

	return escapedText;
}

/*******************************************/
/* convert                                 */
/*******************************************/
/* Convert all the instance of one         */
/* character in latex usable caracter.     */
/*******************************************/
void TextZone::convert(QString& texte, char unicode, const char* escape)
{
	QString expression;
	QString texte_temp;

	expression = QChar(unicode);
	
	if(QString(escape) != "")
	{
		/*1. translate special characters with a space after. */
		texte = texte.replace( QRegExp( expression), QString(escape));
	}
}

/*******************************************/
/* analyse                                 */
/*******************************************/
/* Analyse a text format, get the text used*/
/* by this format.                         */
/*******************************************/
void TextZone::analyse(const Markup * balise_initiale)
{
	kdDebug() << "FORMAT" << endl;
	/* Get header information (size, position)
	 * Get infos. to format the text
	 */
	
	if(balise_initiale != 0)
		analyseTextFormat(balise_initiale);
	
	/* Format the text */
	_texte = _texte.mid(getPos(), getLength());
	
	kdDebug() << _texte.length() << endl;
	kdDebug() << _texte.latin1() << endl;
	kdDebug() << "END FORMAT" << endl;
}

/*******************************************/
/* generate                                */
/*******************************************/
/* Generate the text formated (if needed). */
/*******************************************/
void TextZone::generate(QTextStream &out)
{

	if(useFormat())
		generate_format_begin(out);

	/* Display the text */
	display(escapeLatin1(_texte), out);

	if(useFormat())
		generate_format_end(out);
}

/*******************************************/
/* display                                 */
/*******************************************/
/* Trunc the text in about 80 caracters of */
/* width except if there are not spaces.   */
/*******************************************/
void TextZone::display(QString texte, QTextStream& out)
{
	QString line;
	int index = 0, end = 0;
	end = texte.find(' ', 60, false);
	if(end != -1)
		line = texte.mid(index, end - index);
	else
		line = texte;
	while(end < (signed int) texte.length() && end != -1)
	{
		/* There are something to display */
		out << line << endl;
		index = end;
		end = texte.find(' ', index + 60, false);
		line = texte.mid(index, end - index);
	}
	out << line;
}

/*******************************************/
/* generate_format_begin                   */
/*******************************************/
/* Write the begining format markup.       */
/*******************************************/
void TextZone::generate_format_begin(QTextStream & out)
{
	kdDebug() << "GENERATE FORMAT BEGIN" << endl;
	/* Bold, Italic or underlined */
	if(getWeight() > 0)
		out << " \\textbf{";
	if(isItalic())
		out << " \\textit{";
	if(isUnderlined())
		out << " \\uline{";
	if (isStrikeout())
		out << " \\sout{";
	/* Size */
	if(getSize() != 11)
	{
		out << "\\fontsize{" << getLength() << "}{1}%" << endl;
		out << "\\selectfont" << endl;
	}

	/* Color */
	if(isColor())
	{
		out << "\\textcolor{rgb}{";
		out << getColorRed() << ", " << getColorGreen() << ", ";
		out << getColorBlue() << "}{";
	}

	/* Alignement */
	switch(getAlign())
	{
		case EA_NONE:
			break;
		case EA_SUB: /* pass in math mode !! */
			out << " $_{";
			break;
		case EA_SUPER:
			out << " \\textsuperscript{";
			break;
	}
}

/*******************************************/
/* generate_format_end                     */
/*******************************************/
/* Write the format end markup.            */
/*******************************************/
void TextZone::generate_format_end(QTextStream & out)
{
	kdDebug() << "GENERATE FORMAT END" << endl;
	/* Alignement */
	if(getAlign() == EA_SUPER)
		out << "}";
	if(getAlign() == EA_SUB)
		out << "}$";

	/* Color */
	if(isColor())
		out << "}";

	/* Size */
	if(getSize() != 11)
	{
		out << "\\fontsize{11}{1}%" << endl;
		out << "\\selectfont" << endl;
	}

	/* Bold, Italic or underlined */
	if(isUnderlined())
		out << "}";
	if(isItalic())
		out << "}";
	if(getWeight() > 0)
		out << "}";
	if(isStrikeout())
		out << "}";
}


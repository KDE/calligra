/* MEMO: to see the unicode table
 * xset +fp /usr/X11R6/lib/X11/fonts/ucs/
 * xfd -fn '-misc-fixed-medium-r-semicondensed--13-120-75-75-c-60-iso10646-1'
 */
/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000 - 2003 Robert JACOLIN
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

#include <kdebug.h>		/* for kdDebug() stream */
#include <qregexp.h>		/* for QRegExp() --> escapeLatin1 */

#include "textzone.h"
#include "para.h"

#define CSTART 0x00C0

/*******************************************/
/* TextZone                                */
/*******************************************/
TextZone::TextZone(Para *para)
{
	setPara(para);
	if(para != NULL)
	{
		setSize(para->getSize());
		setWeight(para->getWeight());
		setItalic(para->isItalic());
		setUnderlined(para->getUnderlineType());
		setStrikeout(para->isStrikeout());
	}
}

/*******************************************/
/* TextZone                                */
/*******************************************/
TextZone::TextZone(QString texte, Para *para): _texte(texte)
{
	setPara(para);
	if(para != NULL)
	{
		setSize(para->getSize());
		setWeight(para->getWeight());
		setItalic(para->isItalic());
		setUnderlined(para->getUnderlineType());
		setStrikeout(para->isStrikeout());
	}
}

/*******************************************/
/* ~TextZone                               */
/*******************************************/
TextZone::~TextZone()
{
	kdDebug(30522) << "Destruction of a area" << endl;
}

/*******************************************/
/* useFormat                               */
/*******************************************/
/* Use the format only if the user wants   */
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

		"\\DH{}", "\\~{N}", "\\`{O}", "\\'{O}",
		"\\^{O}", "\\~{O}", "\\\"{O}", "\\texttimes{}",
		"\\O{}", "\\`{U}", "\\'{U}", "\\^{U}",
		"\\\"{U}", "\\'{Y}", "\\TH{}", "\\ss{}",

		"\\`{a}", "\\'{a}", "\\^{a}", "\\~{a}",
		"\\\"{a}", "\\aa", "\\ae{}", "\\c{c}",
		"\\`{e}", "\\'{e}", "\\^{e}", "\\\"{e}",
		"\\`{\\i}", "\\'{\\i}", "\\^{\\i}", "\\\"{\\i}",

		"\\dh{}", "\\~{n}", "\\`{o}", "\\'{o}",
		"\\^{o}", "\\~{o}", "\\\"{o}", "\\textdiv{}",
		"\\o{}", "\\`{u}", "\\'{u}", "\\^{u}",
		"\\\"{u}", "\\'{y}", "\\th{}", "\\\"{y}"
	};

	QString escapedText;
	int unicode;         /* the character to be escaped */

	escapedText = text;  /* copy input text */
	
	/***************************************************************************
	 * Escape the special punctuation and other symbols in the Latin1 supplement
	****************************************************************************/
	/* We must begin by this char because else, all special char will
	 * be backslahed !
	 */
	convert(escapedText, 0X005C, "\\textbackslash{}");

	//convert(escapedText, 0X22, "\\textquotestraightdblbase");/* textcomp */
	convert(escapedText, 0X0023, "\\#{}");
	convert(escapedText, 0X0024, "\\${}");	/* add a \$ at the end of the paragraphes ! */
	convert(escapedText, 0X0025, "\\%{}");
	convert(escapedText, 0X0026, "\\&{}");
	//convert(escapedText, 0X0027, "\\textquotestraightbase");	/* textcomp */
//	convert(escapedText, 0X002A, "\\textasteriskcentered");	/* textcomp */

	convert(escapedText, 0X003C, "\\textless{}");
	convert(escapedText, 0X003E, "\\textgreater{} ");

	convert(escapedText, 0X005E, "\\^{}");
	convert(escapedText, 0X005F, "\\_{}");		
	
	convert(escapedText, 0X007B, "\\{");
	convert(escapedText, 0X007C, "\\textbar{}");
	convert(escapedText, 0X007D, "\\}");
	convert(escapedText, 0X007E, "\\textasciitilde{}");
	
	convert(escapedText, 0X00A1, "!`{}");
	convert(escapedText, 0X00A2, "\\textcent{}");		/* textcomp */
	convert(escapedText, 0X00A3, "\\pounds{}");
	convert(escapedText, 0X00A4, "\\textcurrency{}");	/* textcomp */
	convert(escapedText, 0X00A5, "\\textyen{}");		/* textcomp */
	convert(escapedText, 0X00A6, "\\textbrokenbar{}");
	convert(escapedText, 0X00A7, "\\S{}");
	convert(escapedText, 0X00A8, "\\textasciidieresis{}");	/*? not good */
	convert(escapedText, 0X00A9, "\\copyright{}");
	convert(escapedText, 0X00AA, "\\textordfeminine{}");	/* textcomp */
	convert(escapedText, 0X00AB, "\\guillemotleft{}");	/* textcomp */
	convert(escapedText, 0X00AC, "\\textlnot{}");		/* textcomp */

	convert(escapedText, 0X00AE, "\\textregistered{}");
	convert(escapedText, 0X00AF, "\\textmacron{}");		/* textcomp */
	convert(escapedText, 0X00B0, "\\textdegree{}");		/* textcomp */
	convert(escapedText, 0X00B1, "\\textpm{}");		/* textcomp */
	convert(escapedText, 0X00B2, "\\texttwosuperior{}");	/* textcomp */
	convert(escapedText, 0X00B3, "\\textthreesuperior{}");	/* textcomp */
	convert(escapedText, 0X00B4, "' ");			/* textcomp */
	convert(escapedText, 0X00B5, "\\textmu{}");		/* textcomp */
	convert(escapedText, 0X00B6, "\\P{}");
	convert(escapedText, 0X00B7, "\\textperiodcentered{}");	/* not good textcomp */
//	convert(escapedText, 0X00B8, "\\textthreesuperior{}");	/* textcomp */
	convert(escapedText, 0X00B9, "\\textonesuperior{}");	/* textcomp */
	convert(escapedText, 0X00BA, "\\textordmasculine{}");	/* textcomp */
	convert(escapedText, 0X00BB, "\\guillemotright{}");	/* textcomp */
	convert(escapedText, 0X00BC, "\\textonequarter{}");	/* textcomp */
	convert(escapedText, 0X00BD, "\\textonehalf{}");	/* textcomp */
	convert(escapedText, 0X00BE, "\\textthreequarters{}");	/* textcomp */
	convert(escapedText, 0X00BF, "?`{}");
	
	
	/* begin making escape sequences for the 64 consecutive letters starting at C0
	 * LaTeX has a different escape code when a char is followed by a space so
	 * two escape sequences are needed for each character.
	 */

	for(int index = 0; index < 64; index++)
	{
		unicode = CSTART + index;
		convert(escapedText, unicode, escapes[index]);
	}

	convert(escapedText, 0X2020, "\\textdied{}");		/* textcomp */
	convert(escapedText, 0X2021, "\\textdaggerdbl{}");	/* textcomp */
	convert(escapedText, 0X2022, "'' ");			/* textcomp */
	convert(escapedText, 0X2023, "\\textdaggerdbl{}");	/* textcomp */
	convert(escapedText, 0X2024, "\\textdaggerdbl{}");	/* textcomp */
	convert(escapedText, 0X2025, "\\textdaggerdbl{}");	/* textcomp */
	convert(escapedText, 0X2026, "\\&{}");			/* textcomp */
	convert(escapedText, 0X2027, "\\textperiodcentered{}");	/* textcomp */
	convert(escapedText, 0X2030, "\\textperthousand{}");	/* textcomp */
	convert(escapedText, 0X2031, "\\textpertenthousand{}");	/* textcomp */
	convert(escapedText, 0X2032, "\\textasciiacute{}");	/* textcomp */
	convert(escapedText, 0X2033, "\\textgravedbl{}");	/* textcomp */
	convert(escapedText, 0X2034, "\\textdaggerdbl{}");	/* textcomp */
	convert(escapedText, 0X2035, "\\textasciigrave{}");	/* textcomp */
	convert(escapedText, 0X2036, "\\textacutedbl{}");	/* textcomp */
	convert(escapedText, 0X2037, "\\textdaggerdbl{}");	/* textcomp */
	convert(escapedText, 0X2038, "\\textdaggerdbl{}");	/* textcomp */
	convert(escapedText, 0X2039, "\\textdaggerdbl{}");	/* textcomp */
	convert(escapedText, 0X203A, "\\textdaggerdbl{}");	/* textcomp */
	convert(escapedText, 0X203B, "\\textreferencemark{}");	/* textcomp */
	convert(escapedText, 0X203D, "\\textinterrobang{}");	/* textcomp */

	convert(escapedText, 0X2045, "\\textlquill{}");		/* textcomp */
	convert(escapedText, 0X2046, "\\textrquill{}");		/* textcomp */


	convert(escapedText, 0X2080, "\\textzerooldstyle{}");	/* textcomp */
	convert(escapedText, 0X2081, "\\textoneoldstyle{}");	/* textcomp */
	convert(escapedText, 0X2082, "\\texttwooldstyle{}");	/* textcomp */
	convert(escapedText, 0X2083, "\\textthreeoldstyle{}");	/* textcomp */
	convert(escapedText, 0X2084, "\\textfouroldstyle{}");	/* textcomp */
	convert(escapedText, 0X2085, "\\textfiveoldstyle{}");	/* textcomp */
	convert(escapedText, 0X2086, "\\textsixoldstyle{}");	/* textcomp */
	convert(escapedText, 0X2087, "\\textsevenoldstyle{}");	/* textcomp */
	convert(escapedText, 0X2088, "\\texteightoldstyle{}");	/* textcomp */
	convert(escapedText, 0X2089, "\\textnineoldstyle{}");	/* textcomp */
	convert(escapedText, 0X208C, "\\textdblhyphen{}");	/* textcomp */

	convert(escapedText, 0X20A4, "\\textsterling{}");	/* textcomp */
	convert(escapedText, 0X20A6, "\\textnaria{}");		/* textcomp */
	convert(escapedText, 0X20AA, "\\textwon{}");		/* textcomp */
	convert(escapedText, 0X20AB, "\\textdong{}");		/* textcomp */
	convert(escapedText, 0X20AC, "\\texteuro{}");		/* textcomp */

	convert(escapedText, 0X2103, "\\textcelsius{}");	/* textcomp */
	convert(escapedText, 0X2116, "\\textnumero{}");		/* textcomp */
	convert(escapedText, 0X2117, "\\textcircledP{}");	/* textcomp */
	convert(escapedText, 0X2120, "\\textservicemark{}");	/* textcomp */
	convert(escapedText, 0X2122, "\\texttrademark{}");	/* textcomp */
	convert(escapedText, 0X2126, "\\textohm{}");		/* textcomp */
	convert(escapedText, 0X2127, "\\textmho{}");		/* textcomp */
	convert(escapedText, 0X212E, "\\textestimated{}");	/* textcomp */

	convert(escapedText, 0X2190, "\\textleftarrow{}");	/* textcomp */
	convert(escapedText, 0X2191, "\\textuparrow{}");	/* textcomp */
	convert(escapedText, 0X2192, "\\textrightarrow{}");	/* textcomp */
	convert(escapedText, 0X2193, "\\textdownarrow{}");	/* textcomp */
//	convert(escapedText, 0X2194, "\\texteuro{}");		/* textcomp */
//	convert(escapedText, 0X2195, "\\texteuro{}");		/* textcomp */
//	convert(escapedText, 0X2196, "\\texteuro{}");		/* textcomp */

	return escapedText;
}

/*******************************************/
/* convert                                 */
/*******************************************/
/* Convert all the instance of one         */
/* character in latex usable caracter.     */
/*******************************************/
void TextZone::convert(QString& text, int unicode, const char* escape)
{
	QString expression;
	QString value;

	expression = QString("\\x") + value.setNum(unicode, 16);

	if( !QString(escape).isEmpty() )
	{
		/*1. translate special characters with a space after. */
		text = text.replace( QRegExp( expression), QString(escape));
	}
}

/*******************************************/
/* analyse                                 */
/*******************************************/
/* Analyse a text format, get the text used*/
/* by this format.                         */
/*******************************************/
void TextZone::analyse(const QDomNode balise)
{
	kdDebug(30522) << "FORMAT" << endl;
	/* Get header information (size, position)
	 * Get infos. to format the text
	 */
	//if(balise != 0)
		analyseFormat(balise);
	
	/* Format the text */
	setTexte(getTexte().mid(getPos(), getLength()));
	
	kdDebug(30522) << getTexte().length() << endl;
	kdDebug(30522) << getTexte().latin1() << endl;
	kdDebug(30522) << "END FORMAT" << endl;
}

/*******************************************/
/* analyse                                 */
/*******************************************/
/* Analyse a text format, get the text used*/
/* by this format.                         */
/*******************************************/
void TextZone::analyse()
{
	kdDebug(30522) << "ZONE" << endl;
	
	/* Format the text */
	setTexte(getTexte().mid(getPos(), getLength()));
	
	kdDebug(30522) << "String of " << getTexte().length() << " caracters :" << endl;
	kdDebug(30522) << getTexte().latin1() << endl;
	kdDebug(30522) << "END ZONE" << endl;
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
	if(Config::instance()->getEncoding() == "latin1")
		display(_texte, out);
	else if(Config::instance()->mustUseUnicode())
		display(_texte, out);
	else
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
		if(Config::instance()->mustUseUnicode())
			out << line.utf8() << endl;
		else if(Config::instance()->mustUseLatin1())
			out << line << endl;
		Config::instance()->writeIndent(out);
		index = end;
		end = texte.find(' ', index + 60, false);
		line = texte.mid(index, end - index);
	}
	kdDebug(30522) << line << endl;
	if(Config::instance()->mustUseUnicode())
		out << line.utf8();
	else if(Config::instance()->getEncoding() == "ascii")
		out << line.ascii();
	else
		out << line;
}

/*******************************************/
/* generate_format_begin                   */
/*******************************************/
/* Write the begining format markup.       */
/*******************************************/
void TextZone::generate_format_begin(QTextStream & out)
{
	kdDebug(30522) << "GENERATE FORMAT BEGIN" << endl;

	/* Bold, Italic or underlined */
	if(getWeight() > 50)
		out << "\\textbf{";
	if(isItalic())
		out << "\\textit{";
	if(getUnderlineType() == UNDERLINE_SIMPLE)
		out << "\\uline{";
	else if(getUnderlineType() == UNDERLINE_DOUBLE)
		out << "\\uuline{";
	else if(getUnderlineType() == UNDERLINE_WAVE)
		out << "\\uwave{";
	if (isStrikeout())
		out << "\\sout{";
	
	/* Size */
	if(getSize() != Config::instance()->getDefaultFontSize() &&
			Config::instance()->isKwordStyleUsed())
	{
		out << "\\fontsize{" << getSize() << "}{1}%" << endl;
		Config::instance()->writeIndent(out);
		out << "\\selectfont" << endl;
		Config::instance()->writeIndent(out);
	}

	/* background color */
	if(isBkColored())
	{
		float red, green, blue;

		red   = ((float) getBkColorRed()) / 255;
		green = ((float) getBkColorGreen()) / 255;
		blue  = ((float) getBkColorBlue()) / 255;

		out << "\\colorbox[rgb]{";
		out << red << ", " << green << ", " << blue << "}{";
	}
	
	/* Color */
	if(isColor())
	{
		float red, green, blue;

		red   = ((float) getColorRed()) / 255;
		green = ((float) getColorGreen()) / 255;
		blue  = ((float) getColorBlue()) / 255;

		out << "\\textcolor[rgb]{";
		out << red << ", " << green << ", " << blue << "}{";
	}

	/* Alignement */
	switch(getAlign())
	{
		case EA_NONE:
			break;
		case EA_SUB: /* pass in math mode !! */
			out << "$_{";
			break;
		case EA_SUPER:
			out << "\\textsuperscript{";
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
	kdDebug(30522) << "GENERATE FORMAT END" << endl;
	
	/* Alignement */
	if(getAlign() == EA_SUPER)
		out << "}";
	if(getAlign() == EA_SUB)
		out << "}$";

	/* Color */
	if(isColor() || isBkColored())
		out << "}";

	/* Size */
	if(getSize() != Config::instance()->getDefaultFontSize() && Config::instance()->isKwordStyleUsed())
	{
		out << "\\fontsize{" << Config::instance()->getDefaultFontSize() << "}{1}%" << endl;
		Config::instance()->writeIndent(out);
		out << "\\selectfont" << endl;
		Config::instance()->writeIndent(out);
	}

	/* Bold, Italic or underlined */
	if(isUnderlined())
		out << "}";
	if(isItalic())
		out << "}";
	if(getWeight() > 50)
		out << "}";
	if(isStrikeout())
		out << "}";
}

QString convertSpecialChar(int c)
{
	QString output;

	switch(c)
	{
		case 183: return output = "\\textminus";
			break;
		default: return output.setNum(c);
	}
}

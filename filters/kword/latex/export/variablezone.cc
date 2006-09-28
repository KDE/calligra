/* MEMO: to see the unicode table
 * xset +fp /usr/X11R6/lib/X11/fonts/ucs/
 * xfd -fn '-misc-fixed-medium-r-semicondensed--13-120-75-75-c-60-iso10646-1'
 */
/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2002 - 2003 Robert JACOLIN
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

#include <kdebug.h>				/* for kDebug() stream */

#include "variablezone.h"
#include "para.h"
#include "document.h"
//Added by qt3to4:
#include <QTextStream>

/*******************************************/
/* VariableZone                            */
/*******************************************/
VariableZone::VariableZone(Para* para): VariableFormat(para)
{
	setPara(para);
	setSize(para->getSize());
	setWeight(para->getWeight());
	setItalic(para->isItalic());
	setUnderlined(para->getUnderlineType());
	setStrikeout(para->isStrikeout());
}

/*******************************************/
/* VariableZone                             */
/*******************************************/
VariableZone::VariableZone(QString text, Para* para): VariableFormat(para)
{
	setText(text);
	/*setPara(para);
	setSize(para->getSize());
	setWeight(para->getWeight());
	setItalic(para->isItalic());
	setUnderlined(para->getUnderlineType());
	setStrikeout(para->isStrikeout());*/
}

/*******************************************/
/* ~VariableZone                           */
/*******************************************/
VariableZone::~VariableZone()
{
	kDebug(30522) << "Destruction of an area" << endl;
}

/*******************************************/
/* analyze                                 */
/*******************************************/
/* Analyze a text format, get the text used*/
/* by this format.                         */
/*******************************************/
void VariableZone::analyze(const QDomNode node)
{
	kDebug(30522) << "FORMAT" << endl;
	/* Get header information (size, position)
	 * Get infos. to format the text
	 */
	//if(node != 0)
		analyzeFormat(node);
	
	/* Format the text */
	setText(getText().mid(getPos(), getLength()));
	
	kDebug(30522) << getText().length() << endl;
	kDebug(30522) << getText().latin1() << endl;
	kDebug(30522) << "END FORMAT" << endl;
}

/*******************************************/
/* generate                                */
/*******************************************/
/* Generate the text formatted (if needed).*/
/*******************************************/
void VariableZone::generate(QTextStream &out)
{

	if(useFormat())
		generate_format_begin(out);

	/* Display the text */
	kDebug(30522) << "type : " << getType() << endl;
	if((getType() == VAR_DATE) && !isFix())
		out << "\\today" << endl;
	else if(getType() == VAR_FOOTNOTE)
	{
		if(getNotetype() == "footnote")
			out << "\\,\\footnote{";
		else if(getNotetype() == "endnote")
			out << "\\,\\endnote{";
		/* Get the footnote and generate it. */
		Element* footnote = getRoot()->searchFootnote(getFrameset());
		if(footnote != NULL)
		{
			footnote->generate(out);
		}
		Config::instance()->writeIndent(out);
		out << "}";
	}
	else if(getType() == VAR_NOTE)
	{
		out << "\\marginpar{\\scriptsize ";
		if(Config::instance()->mustUseLatin1())
			display(escapeLatin1(getNote()), out);
		else if(Config::instance()->mustUseUnicode())
			display(getNote(), out);
		out << "}" << endl;
	}
	else
	{
		if(Config::instance()->mustUseLatin1())
			display(escapeLatin1(getText()), out);
		else if(Config::instance()->mustUseUnicode())
			display(getText(), out);
	}
	if(useFormat())
		generate_format_end(out);
	
}


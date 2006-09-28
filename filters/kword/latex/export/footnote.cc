/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000,2002 Robert JACOLIN
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

#include <stdlib.h>		/* for atoi function */
#include <kdebug.h>		/* for kDebug() stream */
#include "footnote.h"
#include "textFrame.h"		/* for generate function (catch text footnote) */
#include "document.h"
//Added by qt3to4:
#include <QTextStream>

Footnote::Footnote(Para* para): Format(para)
{
}

Footnote::~Footnote()
{
	kDebug(30522) << "Destruction of a footnote." << endl;
}

/* Modifiers */
void Footnote::setSpace (QString new_space)
{
	_space = new_space;
}

void Footnote::setBefore(QString new_before)
{
	_before = new_before;

}

void Footnote::setAfter(QString new_after)
{
	_after = new_after;
}

void Footnote::setRef(QString new_ref)
{
	_ref = new_ref;
}

void Footnote::analyze(const QDomNode node)
{
	/* Markup <FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* Parameter analysis */
	kDebug(30522) << "ANALYZE A FOOTNOTE" << endl;
	
	/* Child markup analysis */
	for(int index= 0; index < getNbChild(node); index++)
	{
		if(getChildName(node, index).compare("INTERNAL")== 0)
		{
			kDebug(30522) << "INTERNAL: " << endl;
			analyzeInternal(node);
		}
		else if(getChildName(node, index).compare("RANGE")== 0)
		{
			kDebug(30522) << "RANGE: " << endl;
			analyzeRange(node);
		}
		else if(getChildName(node, index).compare("TEXT")== 0)
		{
			kDebug(30522) << "TEXT: " << endl;
			analyzeText(node);
		}
		else if(getChildName(node, index).compare("DESCRIPT")== 0)
		{
			kDebug(30522) << "DESCRIPT: " << endl;
			analyzeDescript(node);
		}
		else if(getChildName(node, index).compare("FORMAT")== 0)
		{
			kDebug(30522) << "SUBFORMAT: " << endl;
			Format::analyze(node);
		}
	}
	kDebug(30522) << "END OF FOOTNOTE" << endl;
}

void Footnote::analyzeInternal(const QDomNode node)
{
	QDomNode childNode;
	/* Markup <INTERNAL> <PART from="1" to="-1" space="-"/> */

	/* Child markup analysis */
	childNode = getChild(node, "PART");
	for(int index= 0; index < getNbChild(node); index++)
	{
		if(getChildName(node, index).compare("PART")== 0)
		{
			kDebug(30522) << "PART : " << endl;
			setFrom(getAttr(node, "FROM").toInt());
			setTo(getAttr(node, "TO").toInt());
			setSpace(getAttr(node, "SPACE"));

		}
	}
}

void Footnote::analyzeRange(const QDomNode node)
{
	kDebug(30522) << "PARAM" << endl;
	setStart(getAttr(node, "START").toInt());
	setEnd(getAttr(node, "END").toInt());
}

void Footnote::analyzeText(const QDomNode node)
{
	kDebug(30522) << "PARAM" << endl;
	setBefore(getAttr(node, "BEFORE"));
	setAfter(getAttr(node, "AFTER"));
}

void Footnote::analyzeDescript(const QDomNode node)
{
	kDebug(30522) << "PARAM" << endl;
	setRef(getAttr(node, "REF"));
}

void Footnote::generate(QTextStream &out)
{
	Element *footnote = 0;

	kDebug(30522) << "  GENERATION FOOTNOTE" << endl;
	// Go to keep the footnote parag.
	// then write it with this format.
	// like this: \,\footnote{the parag. }
	out << "\\,\\footnote{";
	kDebug(30522) << "footnote : " << _ref << endl;
	if((footnote = getRoot()->searchFootnote(_ref)) != 0)
		footnote->generate(out);
	out << "}";
	kDebug(30522) << "FOOTNOTE GENERATED" << endl;
}


